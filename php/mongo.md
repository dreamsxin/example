# 进程内有效的连接池

`php_phongo.h`
```c
ZEND_BEGIN_MODULE_GLOBALS(mongodb)
	char *debug;
	FILE *debug_fd;
	bson_mem_vtable_t bsonMemVTable;
	HashTable pclients;
	HashTable subscribers;
ZEND_END_MODULE_GLOBALS(mongodb)

#if PHP_VERSION_ID >= 70000
# define MONGODB_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(mongodb, v)
# if defined(ZTS) && defined(COMPILE_DL_MONGODB)
ZEND_TSRMLS_CACHE_EXTERN()
# endif
#else
# ifdef ZTS
#  define MONGODB_G(v) TSRMG(mongodb_globals_id, zend_mongodb_globals *, v)
#  define mglo mongodb_globals_id
# else
#  define MONGODB_G(v) (mongodb_globals.v)
#  define mglo mongodb_globals
# endif
#endif
```

`php_phongo.c`
```c

/* {{{ PHP_GINIT_FUNCTION */
PHP_GINIT_FUNCTION(mongodb)
{
	bson_mem_vtable_t bsonMemVTable = {
		php_phongo_malloc,
		php_phongo_calloc,
		php_phongo_realloc,
		php_phongo_free,
	};
#if PHP_VERSION_ID >= 70000
#if defined(COMPILE_DL_MONGODB) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
#endif
	memset(mongodb_globals, 0, sizeof(zend_mongodb_globals));
	mongodb_globals->bsonMemVTable = bsonMemVTable;

	/* Initialize HashTable for persistent clients */
	zend_hash_init_ex(&mongodb_globals->pclients, 0, NULL, php_phongo_pclient_dtor, 1, 0);
}
/* }}} */


/* {{{ PHP_MSHUTDOWN_FUNCTION */
PHP_MSHUTDOWN_FUNCTION(mongodb)
{
	(void)type; /* We don't care if we are loaded via dl() or extension= */

	/* Destroy HashTable for persistent clients. The HashTable destructor will
	 * destroy any mongoc_client_t objects that were created by this process. */
	zend_hash_destroy(&MONGODB_G(pclients));

	bson_mem_restore_vtable();
	/* Cleanup after libmongoc */
	mongoc_cleanup();

	UNREGISTER_INI_ENTRIES();

	return SUCCESS;
}
/* }}} */

static void php_phongo_persist_client(const char *hash, size_t hash_len, mongoc_client_t *client TSRMLS_DC)
{
	php_phongo_pclient_t *pclient = (php_phongo_pclient_t *) pecalloc(1, sizeof(php_phongo_pclient_t), 1);

	pclient->pid = (int) getpid();
	pclient->client = client;

#if PHP_VERSION_ID >= 70000
	zend_hash_str_update_ptr(&MONGODB_G(pclients), hash, hash_len, pclient);
#else
	zend_hash_update(&MONGODB_G(pclients), hash, hash_len + 1, &pclient, sizeof(php_phongo_pclient_t *), NULL);
#endif
}

static mongoc_client_t *php_phongo_find_client(const char *hash, size_t hash_len TSRMLS_DC)
{
#if PHP_VERSION_ID >= 70000
	php_phongo_pclient_t *pclient;

	if ((pclient = zend_hash_str_find_ptr(&MONGODB_G(pclients), hash, hash_len)) != NULL) {
		return pclient->client;
	}
#else
	php_phongo_pclient_t **pclient;

	if (zend_hash_find(&MONGODB_G(pclients), hash, hash_len + 1, (void**) &pclient) == SUCCESS) {
		return (*pclient)->client;
	}
#endif

	return NULL;
}

void phongo_manager_init(php_phongo_manager_t *manager, const char *uri_string, zval *options, zval *driverOptions TSRMLS_DC) /* {{{ */
{
	char             *hash = NULL;
	size_t            hash_len = 0;
	bson_t            bson_options = BSON_INITIALIZER;
	mongoc_uri_t     *uri = NULL;
	bson_iter_t       iter;
#ifdef MONGOC_ENABLE_SSL
	mongoc_ssl_opt_t *ssl_opt = NULL;
#endif

	if (!(hash = php_phongo_manager_make_client_hash(uri_string, options, driverOptions, &hash_len TSRMLS_CC))) {
		/* Exception should already have been thrown and there is nothing to free */
		return;
	}

	if ((manager->client = php_phongo_find_client(hash, hash_len TSRMLS_CC))) {
		MONGOC_DEBUG("Found client for hash: %s\n", hash);
		goto cleanup;
	}

	if (options) {
		php_phongo_zval_to_bson(options, PHONGO_BSON_NONE, &bson_options, NULL TSRMLS_CC);
	}

	/* An exception may be thrown during BSON conversion */
	if (EG(exception)) {
		goto cleanup;
	}

	if (!(uri = php_phongo_make_uri(uri_string, &bson_options TSRMLS_CC))) {
		/* Exception should already have been thrown */
		goto cleanup;
	}

	if (!php_phongo_apply_options_to_uri(uri, &bson_options TSRMLS_CC) ||
	    !php_phongo_apply_rc_options_to_uri(uri, &bson_options TSRMLS_CC) ||
	    !php_phongo_apply_rp_options_to_uri(uri, &bson_options TSRMLS_CC) ||
	    !php_phongo_apply_wc_options_to_uri(uri, &bson_options TSRMLS_CC)) {
		/* Exception should already have been thrown */
		goto cleanup;
	}

	if (bson_iter_init_find_case(&iter, &bson_options, MONGOC_URI_APPNAME) && BSON_ITER_HOLDS_UTF8(&iter)) {
		const char *str = bson_iter_utf8(&iter, NULL);

		if (!mongoc_uri_set_appname(uri, str)) {
			phongo_throw_exception(PHONGO_ERROR_INVALID_ARGUMENT TSRMLS_CC, "Invalid appname value: '%s'", str);
			goto cleanup;
		}
	}

#ifdef MONGOC_ENABLE_SSL
	/* Construct SSL options even if SSL is not enabled so that exceptions can
	 * be thrown for unsupported driver options. */
	ssl_opt = php_phongo_make_ssl_opt(driverOptions TSRMLS_CC);

	/* An exception may be thrown during SSL option creation */
	if (EG(exception)) {
		goto cleanup;
	}
#else
	if (mongoc_uri_get_ssl(uri)) {
		phongo_throw_exception(PHONGO_ERROR_INVALID_ARGUMENT TSRMLS_CC, "Cannot create SSL client. SSL is not enabled in this build.");
		goto cleanup;
	}
#endif

	manager->client = php_phongo_make_mongo_client(uri TSRMLS_CC);

	if (!manager->client) {
		phongo_throw_exception(PHONGO_ERROR_RUNTIME TSRMLS_CC, "Failed to create Manager from URI: '%s'", uri_string);
		goto cleanup;
	}

#ifdef MONGOC_ENABLE_SSL
	if (ssl_opt && mongoc_uri_get_ssl(uri)) {
		mongoc_client_set_ssl_opts(manager->client, ssl_opt);
	}
#endif

	MONGOC_DEBUG("Created client hash: %s\n", hash);
	php_phongo_persist_client(hash, hash_len, manager->client TSRMLS_CC);

cleanup:
	if (hash) {
		pefree(hash, 1);
	}

	bson_destroy(&bson_options);

	if (uri) {
		mongoc_uri_destroy(uri);
	}

#ifdef MONGOC_ENABLE_SSL
	if (ssl_opt) {
		php_phongo_free_ssl_opt(ssl_opt);
	}
#endif
} /* }}} */
```

