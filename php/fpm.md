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
