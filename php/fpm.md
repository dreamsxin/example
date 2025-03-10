# PHP-FPM

```shell
sudo add-apt-repository ppa:ondrej/php
```

## 测试

```shell
sudo apt-get install libfcgi0ldbl
```

* ping

```shell
SCRIPT_NAME=/ping \
SCRIPT_FILENAME=/ping \
REQUEST_METHOD=GET \
cgi-fcgi -bind -connect 127.0.0.1:9000
```

* status

```shell
SCRIPT_NAME=/status \
SCRIPT_FILENAME=/status \
REQUEST_METHOD=GET \
cgi-fcgi -bind -connect 127.0.0.1:9000
```

* script

```shell
SCRIPT_NAME=/custom.php \
SCRIPT_FILENAME=/custom.php \
QUERY_STRING=VAR1 \
DOCUMENT_ROOT=/var/www/example.com/htdocs/ \
REQUEST_METHOD=GET \
cgi-fcgi -bind -connect 127.0.0.1:9000
```


## 优化

连接数监控
```shell
netstat -ant|wc -l
netstat -ant|awk  '{print $6}'|sort|uniq -c|sort -n
netstat -anpo|grep php-cgi|wc -l   # 连接进程数
netstat -ant|grep 11111|wc -l      # 连接端口数
```

CPU 占用率
```shell
top -p $(pgrep php-fpm | head -20 | tr "\\n" "," | sed 's/,$//')

ps -eo pid,ppid,cmd,%mem,%cpu --sort=-%cpu | head
```
查看 TIME_WAIT 和 CLOSE_WAIT 链接状态。

```shell
netstat -n | awk '/^tcp/ {++S[$NF]} END {for(a in S) print a, S[a]}'
```

按顺序列出内存占用率的进程
```shell
ps -A --sort -rss -o comm,pmem,pcpu|uniq -c |head -15
```

查询单个 `php-fpm` 进程占用的内存：

```shell
pmap $(pgrep php-fpm | head -1)
pmap $(pgrep php-cgi | head -1)
```

通过命令查看服务器上一共开了多少的 php-cgi 进程
```shell
ps -fe |grep "php-fpm"|grep "pool"|wc -l
```

查看已经有多少个php-cgi进程用来处理tcp请求
```shell
netstat -anp|grep "php-fpm"|grep "tcp"|grep "pool"|wc -l
```

## php-fpm的配置公式

`pm.start_servers = min_spare_servers + (max_spare_servers - min_spare_servers) / 2`

## `pm.max_children` 的计算方法

本机内存除以单个 `php-fpm` 进程占用的内存得到的数字为最大的限定值。

- pm.max_children	静态方式下开启的php-fpm进程数量
- pm.start_servers	动态方式下的起始php-fpm进程数量
- pm.min_spare_servers	动态方式下的最小php-fpm进程数量
- pm.max_spare_servers	动态方式下的最大php-fpm进程数量

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

`/etc/php/7.1/fpm/php.ini`设置
```ini
display_errors = On
log_errors = On
error_log = /var/log/php-error.log
error_reporting=E_ALL&~E_NOTICE
```

`/etc/php/7.1/fpm/php-fpm.conf`最后增加
```conf
[global]
; Note: the default prefix is /usr/local/php/var
error_log = log/php_error_log
```

`/etc/php/7.1/fpm/pool.d/www.conf`去掉注释：
```conf
[www]
catch_workers_output = yes
```

`catch_workers_output = yes` 一定要放在对应的`[www]`组下，修改完重启。可以通过`phpinfo()`查看日志文件地址。
```conf
catch_workers_output = yes
php_flag[display_errors] = on
php_admin_value[error_log] = /var/log/php-fpm/default/error.log
php_admin_flag[log_errors] = on
```

## 开启状态页

```conf
pm.status_path = /status
```

Nginx 配置
```conf
    # 在 server 中添加以下配置
    location = /status {
	include fastcgi_params;
	fastcgi_pass 127.0.0.1:9000;
	fastcgi_param SCRIPT_FILENAME $fastcgi_script_name;
    }
```

* `pm.status_path` 参数详解

- pool 			– fpm池子名称，大多数为www
- process manager 	– 进程管理方式,值：static, dynamic or ondemand. dynamic
- start time 		– 启动日期,如果reload了php-fpm，时间会更新
- start since 		– 运行时长
- accepted conn 	– 当前池子接受的请求数
- listen queue 		– 请求等待队列，如果这个值不为0，那么要增加FPM的进程数量
- max listen queue 	– 请求等待队列最高的数量
- listen queue len 	– socket等待队列长度
- idle processes 	– 空闲进程数量
- active processes 	– 活跃进程数量
- total processes 	– 总进程数量
- max active processes 	– 最大的活跃进程数量（FPM启动开始算）
- max children reached 	- 进程最大数量限制的次数，如果这个数量不为0，那说明你的最大进程数量太小了，请改大一点。
- slow requests 	– 启用了php-fpm slow-log，缓慢请求的数量

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

server {
        listen 9090;
        server_name localhost;

        # SSL configuration
        #
        listen 443 ssl default_server;
        listen [::]:443 ssl default_server;
        #
        # Note: You should disable gzip for SSL traffic.
        # See: https://bugs.debian.org/773332
        #
        # Read up on ssl_ciphers to ensure a secure configuration.
        # See: https://bugs.debian.org/765782
        #
        # Self signed certs generated by the ssl-cert package
        # Don't use them in a production server!
        #
        include snippets/snakeoil.conf;

        index index.php index.html index.htm;
        set $root_path '/var/www/html/www/public/';
        root $root_path;

        location / {
                try_files $uri $uri/ /index.php?$query_string;
        }

        location /ngx_status {
                stub_status on;
                access_log off;
                #allow 127.0.0.1;
                #deny all;
        }

        location ~ \.php$ {
                fastcgi_buffer_size 128k;
                fastcgi_buffers 4 256k;
                fastcgi_busy_buffers_size 256k;
                fastcgi_temp_file_write_size 256k;
                fastcgi_send_timeout 900;
                fastcgi_read_timeout 900;
                try_files $uri =404;
                fastcgi_split_path_info ^(.+\.php)(/.+)$;
                fastcgi_pass 127.0.0.1:9000;
                fastcgi_index index.php;
                fastcgi_param  SCRIPT_FILENAME  $document_root$fastcgi_script_name;
                include fastcgi_params;
        }

        location ~ \.php$ {
                include snippets/fastcgi-php.conf;

        #       # With php7.0-cgi alone:
        #       fastcgi_pass 127.0.0.1:9000;
                # With php7.0-fpm:
                fastcgi_pass unix:/run/php/php7.1-fpm.sock;
        }

        location ~ \.php$ {
                include snippets/fastcgi-php.conf;
                if ($request_uri ~ /api/) {
                        # With php7.0-cgi alone:
                        fastcgi_pass 127.0.0.1:9000;
                        # With php7.0-fpm:
                        #fastcgi_pass unix:/run/php/php7.1-fpm.sock;
                }
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

## 开启 php-fpm 生成 core dumps

切换到 root 用户修改：

```shell
su -
echo '/tmp/core-%e.%p' > /proc/sys/kernel/core_pattern
echo 0 > /proc/sys/kernel/core_uses_pid
ulimit -c unlimited
```

设置 `/etc/php-fpm.d/www.conf`

```conf
rlimit_core = unlimited
```

```shell
gdb /path/to/php-fpm core.dump
```

## 日志格式

```text
; The access log file

; Default: not set

;access.log = log/$pool.access.log

;接收日志，默认没有设置


; The access log format.

; The following syntax is allowed

;   %%: the '%' character

;   %C: %CPU used by the request

;       it can accept the following format:

;       - %{user}C for user CPU only

;       - %{system}C for system CPU only

;       - %{total}C  for user + system  CPU (default)

;   %d: time taken to serve the request

;       it can accept the following format:

;       - %{seconds}d (default)

;       - %{miliseconds}d

;       - %{mili}d

;       - %{microseconds}d

;       - %{micro}d

;   %e: an environment variable (same as $_ENV or $_SERVER)

;       it must be associated with embraces to specify the name of the env

;       variable. Some exemples:

;       - server specifics like: %{REQUEST_METHOD}e or %{SERVER_PROTOCOL}e

;       - HTTP headers like: %{HTTP_HOST}e or %{HTTP_USER_AGENT}e

;   %f: script filename

;   %l: content-length of the request (for POST request only)

;   %m: request method

;   %M: peak of memory allocated by PHP

;       it can accept the following format:

;       - %{bytes}M (default)

;       - %{kilobytes}M

;       - %{kilo}M

;       - %{megabytes}M

;       - %{mega}M

;   %n: pool name

;   %o: ouput header

;       it must be associated with embraces to specify the name of the header:

;       - %{Content-Type}o

;       - %{X-Powered-By}o

;       - %{Transfert-Encoding}o

;       - ....

;   %p: PID of the child that serviced the request

;   %P: PID of the parent of the child that serviced the request

;   %q: the query string

;   %Q: the '?' character if query string exists

;   %r: the request URI (without the query string, see %q and %Q)

;   %R: remote IP address

;   %s: status (response code)

;   %t: server time the request was received

;       it can accept a strftime(3) format:

;       %d/%b/%Y:%H:%M:%S %z (default)

;   %T: time the log has been written (the request has finished)

;       it can accept a strftime(3) format:

;       %d/%b/%Y:%H:%M:%S %z (default)

;   %u: remote user

;

; Default: "%R - %u %t \"%m  %r\" %s"
```