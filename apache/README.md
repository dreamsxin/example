# 开启限速模块
文档 `http://httpd.apache.org/docs/2.4/mod/mod_ratelimit.html`
```shell
sudo a2enmod ratelimit
```

# 修改配置
```shell
<Directory /var/www/html/>
    SetOutputFilter RATE_LIMIT
    SetEnv rate-limit 400 
</Directory>
```

## Apache 2.4 使用 PHP FPM
```conf
<FilesMatch \.php$>
         SetHandler "proxy:fcgi://127.0.0.1:9000"
</FilesMatch>
```

```conf
listen = 127.0.0.1:9000
pm = dynamic
pm.max_children = 50
pm.start_servers = 100
pm.min_spare_servers = 30
pm.max_spare_servers = 200
```