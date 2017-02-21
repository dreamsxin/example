# PHP-FPM

## 用信号控制：

master进程可以理解以下信号

- INT, TERM 立刻终止
- QUIT 平滑终止
- USR1 重新打开日志文件
- USR2 平滑重载所有worker进程并重新载入配置和二进制模块

```shell
ps aux|grep php-fpm
kill -USR2 主进程号
```

## 开启慢请求日志

```conf
request_slowlog_timeout = 1
```

## 开启错误日志

`php.ini`设置
```ini
display_errors = On
log_errors = On
error_log = \var\log\php-error.log
```

`php-fpm.conf`最后增加
```conf
[www]
catch_workers_output = yes
error_log = log/error_log
php_flag[display_errors] = on
```

上面其实就是在文件里的配置
`pool.d\www.conf`

`catch_workers_output = yes` 一定要放在对应的`[www]`组下，修改完重启。可以通过`phpinfo()`查看日志文件地址。
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

## 启动脚本

`/etc/init.d/php7-fpm`

```shell
#!/bin/bash

php_command=/usr/local/php7/sbin/php-fpm
php_config=/usr/local/php7/etc/php-fpm.conf
php_pid=/usr/local/php7/var/run/php-fpm.pid
RETVAL=0
prog="php-fpm"

#start function
php_fpm_start() {
    /usr/local/php7/sbin/php-fpm
}

start(){
    if [ -e $php_pid  ]
    then
    echo "php-fpm already start..."
    exit 1
    fi
    php_fpm_start
}

stop(){
    if [ -e $php_pid ]
    then
    parent_pid=`cat $php_pid`
    all_pid=`ps -ef | grep php-fpm | awk '{if('$parent_pid' == $3){print $2}}'`
    for pid in $all_pid
    do
            kill $pid
        done
        kill $parent_pid
    fi
    exit 1
}

restart(){
    stop
    start
}

# See how we were called.
case "$1" in
start)
        start
        ;;
stop)
        stop
        ;;
restart)
        stop
        start
        ;;
status)
        status $prog
        RETVAL=$?
        ;;
*)
        echo $"Usage: $prog {start|stop|restart|status}"
        exit 1
esac
exit $RETVAL
```

```shell
sudo apt-get install chkconfig
sudo ln -s /usr/lib/insserv/insserv /sbin/

##  添加执行权限  
chmod a+x /etc/init.d/php7-fpm

##  加入服务
chkconfig --add php7-fpm

##   开机自启  
chkconfig php7-fpm on
```
