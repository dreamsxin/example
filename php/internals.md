
## ZEND_API int zend_set_user_opcode_handler(zend_uchar opcode, user_opcode_handler_t handler);

https://phpinternals.net/docs/zend_set_user_opcode_handler

将对应的 `zend opcode` 的 `handler` 函数替换成自己定义的函数。

- ZEND_INCLUDE_OR_EVAL
- ZEND_DO_FCALL
- ZEND_DO_FCALL_BY_NAME
- ZEND_INIT_USER_CALL
- ZEND_INIT_DYNAMIC_CALL
- ZEND_ECHO
- ZEND_ECHO_SPEC_CONST
- ZEND_ECHO_SPEC_TMPVAR
- ZEND_ECHO_SPEC_CV
- ZEND_EXIT
- ZEND_CONCAT
- ZEND_FAST_CONCAT
- ZEND_ROPE_END


例如重载 `echo`：

```c
static inline int overload_echo(zend_execute_data *execute_data) {
    php_printf("Echoing: ");
    return ZEND_USER_OPCODE_DISPATCH;
}

PHP_MINIT_FUNCTION(custom_extension)
{
    zend_set_user_opcode_handler(ZEND_ECHO, overload_echo);

    return SUCCESS;
}
```

```c
static int LIGHT_DO_FCALL(ZEND_OPCODE_HANDLER_ARGS)
{
    /* 检测是否为合法调用 */

    if (/*非法调用*/)
    {
        /*拦截掉，不执行。*/
    }
    else
    {
        /*合法调用，继续执行*/
        return ZEND_USER_OPCODE_DISPATHC;
    }   
}

PHP_MINIT_FUNCTION(custom_extension)
{
    
    zend_set_user_opcode_handler(ZEND_DO_FCALL, LIGHT_DO_FCALL);
    return SUCCESS;
}
```

根据 opcode 的返回类型进行重载：

- ZEND_USER_OPCODE_CONTINUE
- ZEND_USER_OPCODE_RETURN
- ZEND_USER_OPCODE_DISPATCH
- ZEND_USER_OPCODE_ENTER
- ZEND_USER_OPCODE_LEAVE

## static const void* ZEND_FASTCALL zend_vm_get_opcode_handler_ex(uint32_t spec, const zend_op* op)

`zend_opcode_handlers` 数据类型 `static const void * const labels[];` 

```c
#if (ZEND_VM_KIND == ZEND_VM_KIND_HYBRID)
#define HYBRID_NEXT()     goto *(void**)(OPLINE->handler)
#define HYBRID_SWITCH()   HYBRID_NEXT();
#define HYBRID_CASE(op)   op ## _LABEL
#define HYBRID_BREAK()    HYBRID_NEXT()
#define HYBRID_DEFAULT    ZEND_NULL_LABEL
#endif
```
