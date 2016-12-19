# PHP-FPM 配置

## 开启慢请求日志

```conf
request_slowlog_timeout = 1
```

## 开启错误日志 pool.d/www.conf

`catch_workers_output = yes` 一定要放在对应的`[www]`组下

```conf
catch_workers_output = yes
php_flag[display_errors] = on
php_admin_value[error_log] = /var/log/php-fpm/default/error.log
php_admin_flag[log_errors] = on
```

## Nginx 设置

```conf
http {
    upstream fastcgiserver {
            server unix:/run/php/php7.0-fpm.sock;
            server unix:/run/php/php7.1-fpm.sock;
    }
}

server {
    try_files $uri $uri/ @rewrite;

    location @rewrite {
            rewrite ^/(.*)$ /index.php?_url=/$1;
    }

    location ~ \.php {
            fastcgi_pass fastcgiserver;
            fastcgi_index /index.php;

            include /etc/nginx/fastcgi_params;

            fastcgi_split_path_info    ^(.+\.php)(/.+)$;
            fastcgi_param PATH_INFO    $fastcgi_path_info;
            fastcgi_param PATH_TRANSLATED $document_root$fastcgi_path_info;
            fastcgi_param SCRIPT_FILENAME $document_root$fastcgi_script_name;
    }
}
```
