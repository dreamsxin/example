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
    proxy_pass http://localhost/auth/
    proxy_pass_request_body off;
    proxy_set_header Content-Length "";
    proxy_set_header X-Original-URI $request_uri;
}

```

后端 `http://localhost/auth/` 返回200 验证通过。

- 200 认证成功
- 403 认证失败
- 401 很特殊
- xxx 其它有问题

"auth_request_set" 这个指令是在请求通过认证后作变量的设置。