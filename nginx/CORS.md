# CORS 跨域（Cross-Origin Resource Sharing）

- https://developer.mozilla.org/zh-CN/docs/Web/HTTP/Guides/CORS

备注： 当响应的是附带身份凭证的请求时，服务端必须明确 Access-Control-Allow-Origin 的值，而不能使用通配符“*”。

如果后端返回非200的状态时没有添加头，加个 `always`
`add_header 'Access-Control-Allow-Origin' '*' always;`

## https

通过客户端传过来的头信息（origin）设置。

```conf
        if ($http_origin = ''){
            set $http_origin "*";
        }   

        add_header 'Access-Control-Allow-Origin' $http_origin;
```

## 注意事项

* Nginx 只处理最后一个 `add_header`。

因此，如果在 `server` 有一个`add_header`，在 `location` 中也有一个，将只处理 `location` 上下文中的 `add_header` 指令。

* 一个 `location` 实际上已经把请求发送到另一个 `location`，那么第一个 `location` 的 `add_header` 指令是无用的。

例如，我们的配置文件如下：
```conf
server {
	listen 80;
	server_name localhost;

	root_path /var/www/html;

	location / {
		if ($request_method = 'OPTIONS') {
			add_header 'Access-Control-Allow-Origin' '*';
			add_header 'Access-Control-Allow-Methods' 'GET, POST, OPTIONS';
			
			add_header 'Access-Control-Allow-Headers' 'DNT,X-CustomHeader,Keep-Alive,User-Agent,X-Requested-With,If-Modified-Since,Cache-Control,Content-Type';
			
			add_header 'Access-Control-Max-Age' 1728000;
			add_header 'Content-Type' 'text/plain charset=UTF-8';
			add_header 'Content-Length' 0;
			return 204;
		}

		add_header 'Access-Control-Allow-Origin' '*';
		add_header 'Access-Control-Allow-Methods' 'GET, POST, OPTIONS';
		add_header 'Access-Control-Allow-Headers' 'DNT,X-CustomHeader,Keep-Alive,User-Agent,X-Requested-With,If-Modified-Since,Cache-Control,Content-Type';
		try_files $uri $uri/ /index.php?$query_string;
	}

	location ~ \.php$ {
		include snippets/fastcgi-php.conf;
		fastcgi_pass unix:/run/php/php7.1-fpm.sock;

		add_header 'Access-Control-Allow-Origin' '*';
		add_header 'Access-Control-Allow-Methods' 'GET, POST, OPTIONS';
		add_header 'Access-Control-Allow-Headers' 'DNT,X-CustomHeader,Keep-Alive,User-Agent,X-Requested-With,If-Modified-Since,Cache-Control,Content-Type';
	}
}
```
这种情况下我把指令加到所有的 `location ~ \.php$` 才能生效。

```conf
```
