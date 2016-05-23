# 开启限速模块
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