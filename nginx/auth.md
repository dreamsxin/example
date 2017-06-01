# 后端验证模块

## auth_request

这个模块， 允许您的nginx通过发送请求到后端服务器（一般是应用服务器，例如tomcat，或者php等）进行请求， 并且根据请求决定是验证通过或者不通过。

Example usage:
```conf
location /private/ {
    auth_request /auth;
    ...
}

location = /auth {
	internal;
    proxy_pass http://localhost/auth/
    proxy_pass_request_body off;
    proxy_set_header Content-Length "";
    proxy_set_header X-Original-URI $request_uri;
	proxy_set_header request_uri $request_uri;
}
```

后端 `http://localhost/auth/` 返回200 验证通过。

- 200 认证成功
- 403 认证失败
- 401 很特殊
- xxx 其它有问题

验证请求例子：
```php
	$url = $this->request->getServer('HTTP_X_ORIGINAL_URI');
	$query_str = parse_url($url, PHP_URL_QUERY);

	parse_str($query_str, $args);
	$uid = \Phalcon\Arr::get($args, 'uid');
	$token = \Phalcon\Arr::get($args, 'token');
```

"auth_request_set" 这个指令是在请求通过认证后作变量的设置。

我们可以根据响应的头信息，进行设置变量：

```php
<?php

$url = "192.168.1.101:81";
header("myurl: $url");
```

```conf
    location / {
        auth_request /auth;
        auth_request_set $url $sent_http_myurl;
        proxy_pass $url;
    }
```

Nginx 内部实现
```c
static ngx_int_t ngx_http_auth_request_handler(ngx_http_request_t *r)
{
    if (ctx != NULL) {
        if (ngx_http_auth_request_set_variables(r, arcf, ctx) != NGX_OK) {  // 看函数名感觉这里有干货
            return NGX_ERROR;
        }

        /* return appropriate status */

        if (ctx->status == NGX_HTTP_FORBIDDEN) { // 403
            return ctx->status;
        }
        
        // 200 and ...
        if (ctx->status >= NGX_HTTP_OK
            && ctx->status < NGX_HTTP_SPECIAL_RESPONSE)
        {
            return NGX_OK;
        }

        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
                      "auth request unexpected status: %d", ctx->status);

        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

    return NGX_AGAIN;
}

static ngx_int_t ngx_http_auth_request_set_variables(ngx_http_request_t *r, ngx_http_auth_request_conf_t *arcf, ngx_http_auth_request_ctx_t *ctx)
{
    cmcf = ngx_http_get_module_main_conf(r, ngx_http_core_module);
    v = cmcf->variables.elts;

    av = arcf->vars->elts;
    last = av + arcf->vars->nelts;

    // 遍历 arcf->vars 这个东东
    while (av < last) {  
        /*
         * explicitly set new value to make sure it will be available after
         * internal redirects
         */

        vv = &r->variables[av->index];

        if (ngx_http_complex_value(ctx->subrequest, &av->value, &val)
            != NGX_OK)
        {
            return NGX_ERROR;
        }

        vv->valid = 1;
        vv->not_found = 0;
        vv->data = val.data;
        vv->len = val.len;

        if (av->set_handler) {
            /*
             * set_handler only available in cmcf->variables_keys, so we store
             * it explicitly
             */

            av->set_handler(r, vv, v[av->index].data);  // 设置变量
        }

        av++;
    }

    return NGX_OK;
}

static ngx_command_t  ngx_http_auth_request_commands[] = {

    { ngx_string("auth_request"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
      ngx_http_auth_request,
      NGX_HTTP_LOC_CONF_OFFSET,
      0,
      NULL },

    { ngx_string("auth_request_set"), // 名称很像设置变量，就是它了
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE2,
      ngx_http_auth_request_set, // 看一下这函数实现，印证了猜测是正确的
      NGX_HTTP_LOC_CONF_OFFSET,
      0,
      NULL },

      ngx_null_command
};

ngx_http_variable_value_t *ngx_http_get_variable(ngx_http_request_t *r, ngx_str_t *name, ngx_uint_t key)
{
    // 通过 http_xxx 获取请求的头部信息
    if (ngx_strncmp(name->data, "http_", 5) == 0) {
        if (ngx_http_variable_unknown_header_in(r, vv, (uintptr_t) name) == NGX_OK)
        {
            return vv;
        }

        return NULL;
    }

    // 通过 sent_http_ 获取响应的头部信息
    if (ngx_strncmp(name->data, "sent_http_", 10) == 0) {
        if (ngx_http_variable_unknown_header_out(r, vv, (uintptr_t) name) == NGX_OK) 
        {
            return vv;
        }

        return NULL;
    }

    vv->not_found = 1;

    return vv;
}
```