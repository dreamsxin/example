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
sudo vi /etc/apache2/mods-enabled/mpm_prefork.conf
<IfModule mpm_prefork_module>
        ServerLimit 1000
        StartServers        10
        MinSpareServers     20
        MaxSpareServers     100
        MaxRequestWorkers   1000
        MaxConnectionsPerChild   0
</IfModule>
```

```conf
listen = 127.0.0.1:9000
pm = dynamic
pm.max_children = 200
pm.start_servers = 100
pm.min_spare_servers = 30
pm.max_spare_servers = 200
```