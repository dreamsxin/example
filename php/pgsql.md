# 长连接

`EG(regular_list)`存储着所有的只用在当前请求的{资源}。
`EG(persistent_list)`存储着所有的持久{资源}。

`EG(persistent_list)`与`EG(regular_list)` 有个明显的区别，那就是它每个值的索引都是字符串类型的，而且它的每个值也不会在每次请求结束后被释放掉，
只能我们手动通过`zend_hash_del()`来删除，或者在进程结束后类似于`MSHUTDOWN`阶段将`EG(persistent_list)`整体清除。

```c
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(pgsql)
{
	/* clean up notice messages */
	zend_hash_clean(&PGG(notices));
	/* clean up persistent connection */
	zend_hash_apply(&EG(persistent_list), (apply_func_t) _rollback_transactions);
	return SUCCESS;
}
/* }}} */
```

```c
			/* hash it up */
			new_le.type = le_plink;
			new_le.ptr = pgsql;
			if (zend_hash_str_update_mem(&EG(persistent_list), ZSTR_VAL(str.s), ZSTR_LEN(str.s), &new_le, sizeof(zend_resource)) == NULL) {
				goto err;
			}