# redis 持久连接

`library.c`

```c
/**
 * redis_sock_connect
 */
PHP_REDIS_API int redis_sock_connect(RedisSock *redis_sock TSRMLS_DC)
{
    struct timeval tv, read_tv, *tv_ptr = NULL;
    char host[1024], *persistent_id = NULL;
    const char *fmtstr = "%s:%d";
    int host_len, err = 0;
    php_netstream_data_t *sock;
    int tcp_flag = 1;

    if (redis_sock->stream != NULL) {
        redis_sock_disconnect(redis_sock TSRMLS_CC);
    }

    tv.tv_sec  = (time_t)redis_sock->timeout;
    tv.tv_usec = (int)((redis_sock->timeout - tv.tv_sec) * 1000000);
    if(tv.tv_sec != 0 || tv.tv_usec != 0) {
        tv_ptr = &tv;
    }

    read_tv.tv_sec  = (time_t)redis_sock->read_timeout;
    read_tv.tv_usec = (int)((redis_sock->read_timeout-read_tv.tv_sec)*1000000);

    if(redis_sock->host[0] == '/' && redis_sock->port < 1) {
        host_len = snprintf(host, sizeof(host), "unix://%s", redis_sock->host);
    } else {
        if(redis_sock->port == 0)
            redis_sock->port = 6379;

#ifdef HAVE_IPV6
        /* If we've got IPv6 and find a colon in our address, convert to proper
         * IPv6 [host]:port format */
        if (strchr(redis_sock->host, ':') != NULL) {
            fmtstr = "[%s]:%d";
        }
#endif
        host_len = snprintf(host, sizeof(host), fmtstr, redis_sock->host, redis_sock->port);
    }

    if (redis_sock->persistent) {
        if (redis_sock->persistent_id) {
            spprintf(&persistent_id, 0, "phpredis:%s:%s", host, 
                redis_sock->persistent_id);
        } else {
            spprintf(&persistent_id, 0, "phpredis:%s:%f", host, 
                redis_sock->timeout);
        }
    }

    redis_sock->stream = php_stream_xport_create(host, host_len, 
        0, STREAM_XPORT_CLIENT | STREAM_XPORT_CONNECT,
        persistent_id, tv_ptr, NULL, NULL, &err);

    if (persistent_id) {
        efree(persistent_id);
    }

    if (!redis_sock->stream) {
        return -1;
    }

    /* set TCP_NODELAY */
    sock = (php_netstream_data_t*)redis_sock->stream->abstract;
    setsockopt(sock->socket, IPPROTO_TCP, TCP_NODELAY, (char *) &tcp_flag, 
        sizeof(int));

    php_stream_auto_cleanup(redis_sock->stream);

    if(tv.tv_sec != 0 || tv.tv_usec != 0) {
        php_stream_set_option(redis_sock->stream,PHP_STREAM_OPTION_READ_TIMEOUT,
            0, &read_tv);
    }
    php_stream_set_option(redis_sock->stream,
        PHP_STREAM_OPTION_WRITE_BUFFER, PHP_STREAM_BUFFER_NONE, NULL);

    redis_sock->status = REDIS_SOCK_STATUS_CONNECTED;

    return 0;
}

PHP_REDIS_API void redis_stream_close(RedisSock *redis_sock TSRMLS_DC) {
    if (!redis_sock->persistent) {
        php_stream_close(redis_sock->stream);
    } else {
        php_stream_pclose(redis_sock->stream);
    }
}
```

`redis.c`

```c
PHP_REDIS_API int redis_sock_get(zval *id, RedisSock **redis_sock TSRMLS_DC, int no_throw);
PHP_REDIS_API int redis_connect(INTERNAL_FUNCTION_PARAMETERS, int persistent);
```