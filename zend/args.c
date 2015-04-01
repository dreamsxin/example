static void phalcon_di_call_method(INTERNAL_FUNCTION_PARAMETERS)
{
	zval *param, *element;
	void **p;
	int arg_count;

	if (!ZEND_NUM_ARGS()) {
		param = PHALCON_GLOBAL(z_null);
	} else {
		p           = zend_vm_stack_top(TSRMLS_C) - 1;
		arg_count   = (int)(zend_uintptr_t)*p;

		ALLOC_ZVAL(param);
		array_init_size(param, arg_count);

		while (arg_count > 0) {
			ALLOC_ZVAL(element);
			*element = **((zval**)(p - arg_count));
			zval_copy_ctor(element);
			INIT_PZVAL(element);
			zend_hash_next_index_insert(param->value.ht, &element, sizeof(zval *), NULL);
			--arg_count;
		}
	}
}