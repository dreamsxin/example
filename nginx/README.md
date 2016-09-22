# 安装 nginx php5-fpm

```shell
sudo add-apt-repository ppa:nginx/stable
sudo apt-get update
sudo apt-get install nginx php5-fpm
```

# 查看php5运行进程
ps -waux | grep php5

打开关闭php5进程
```shell
sudo service php5-fpm stop
sudo service php5-fpm start
sudo service php5-fpm restart
sudo service php5-fpm status
```

## 修改nginx配置
```conf
server {
	listen   80;
	server_name localhost.dev;

	index index.php index.html index.htm;
	set $root_path '/var/www/phalcon/public';
	root $root_path;
	client_max_body_size 50M;
	client_body_temp_path /tmp;

	try_files $uri $uri/ @rewrite;

	location @rewrite {
		rewrite ^/(.*)$ /index.php?_url=/$1;
	}

	location ~ \.php {
		fastcgi_pass unix:/run/php-fpm/php-fpm.sock;
		fastcgi_index /index.php;

		include /etc/nginx/fastcgi_params;

		fastcgi_split_path_info	   ^(.+\.php)(/.+)$;
		fastcgi_param PATH_INFO	   $fastcgi_path_info;
		fastcgi_param PATH_TRANSLATED $document_root$fastcgi_path_info;
		fastcgi_param SCRIPT_FILENAME $document_root$fastcgi_script_name;
	}

	location ~* ^/(css|img|js|flv|swf|download)/(.+)$ {
		root $root_path;
	}

	location ~ /\.ht {
		deny all;
	}
}
upstream myleft
{
    server 213.215.217.210:9090;
    server 213.215.217.211:9090;
}

server {
        listen 80 default_server;
        listen [::]:80 default_server ipv6only=on;
        server_name *.myleftstudio.com;

        location / {
                proxy_pass http://myleft;
                proxy_set_header Host $host;
                proxy_set_header X-Real-IP $remote_addr;
                proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;

                proxy_buffering off;
        }

}
```

## 配置php5监听端口 `/etc/php5/fpm/pool.d/www.conf`

```conf
listen = /var/run/php5-fpm.sock
# 改为
listen = 127.0.0.1:9000
```

## 修改nginx配置
`/etc/nginx/nginx.conf`
```conf
user www-data;
worker_processes auto;
pid /run/nginx.pid;
worker_rlimit_nofile 100000;

events {
        worker_connections 2048;
        # multi_accept on;
        use epoll;
}

```

`/etc/nginx/sites-available/default`
```conf
server {
	listen   80;
	server_name localhost.dev;

	index index.php index.html index.htm;
	set $root_path '/var/www/phalcon/public';
	root $root_path;

	location / {
		try_files $uri $uri/ /index.php?$query_string;
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

	location ~* ^/(css|img|js|flv|swf|download)/(.+)$ {
		root $root_path;
	}

	location ~ /\.ht {
		deny all;
	}

	location /nginx-status {
		stub_status on;
		access_log off;
		#加入访问限制
		allow all;
		# deny all;
	}
	error_page 405 =200 $uri;
}
```

```conf
pm = static
pm.max_children = 30
request_terminate_timeout = 900
```

## 限速

1、在nginx.conf里的http{}里加上如下代码：
```ini
#ip limit
limit_conn_zone $binary_remote_addr zone=perip:10m;
limit_conn_zone $server_name zone=perserver:10m;
```
2、在需要限制并发数和下载带宽的网站配置server{}里加上如下代码：
```ini
limit_conn perip 2; //限制并发连接数
limit_conn perserver 20; // 限制同一server并发连接数
limit_rate 100k; // 限制下载速度
```

## 限请求频率

```conf
#定义一个名为allips的limit_req_zone用来存储session，大小是10M内存，以$binary_remote_addr 为key,限制平均每秒的请求为20个，
limit_req_zone $binary_remote_addr zone=allips:10m rate=20r/s;

#限制每ip每秒不超过20个请求，漏桶数burst为5，brust的意思就是，如果第1秒、2,3,4秒请求为19个，第5秒的请求为25个是被允许的。
#但是如果你第1秒就25个请求，第2秒超过20的请求返回503错误。
#nodelay，如果不设置该选项，严格使用平均速率限制请求数，
#第1秒25个请求时，5个请求放到第2秒执行，
#设置nodelay，25个请求将在第1秒执行。
limit_req zone=allips burst=5 nodelay;
```

## 安装 nginx-rtmp-module
```shell
sudo apt-get source nginx
cd nginx-1.8.1/debian/modules/
sudo git clone https://github.com/arut/nginx-rtmp-module.git
```

编辑 `debian/rules` 追加 `--add-module=$(MODULESDIR)/nginx-rtmp-module`

## 下载nginx的依赖包
`sudo apt-get build-dep nginx`

## 打包，会在上层目录生成 deb 文件
`sudo dpkg-buildpackage -b`

## 安装
`sudo dpkg --install nginx-full_1.8.1-0+precise0_amd64.deb`

或者

```shell
./configure --add-module=debian/modules/nginx-rtmp-module
```

Push和pull模式，用在多个server协助的情况下，输出流可以push，输入流可以pull。

```conf 
application mypush {
    live on;

    # Every stream published here
    # is automatically pushed to 
    # these two machines
    push rtmp1.example.com;
    push rtmp2.example.com:1934;
}

application mypull {
    live on;

    # Pull all streams from remote machine
    # and play locally
    pull rtmp://rtmp3.example.com pageUrl=www.example.com/index.html;
}
```

nginx-rtmp拉流是的live选项的含义：

pull rtmp://rtmp3.example.com/test  live=1 name=test static;

rtmp的play包，有几个参数：
-2：猜这个流类型，先试live，然后vod。
-1：live类型，若这个流是点播，就失败。
0：vod类型，若这个流是直播，就失败。

因为rtmp播放时，例如：rtmp://xx/app/stream ，无法判断该rtmp流是直播还是点播，所以需要加这个选项进行区分。

static：
默认情况下，nginx-rtmp切hls的时候是要求用户访问才会切片，加static后，只有有流接入就会切片

## 统计
```shell
cat  access.log.1| sed -n '/21\/May\/2016:21/,/21\/May\/2016:21/p'|more
# 统计指定时间段连接数
cat  access.log.1| sed -n '/21\/May\/2016:21/,/21\/May\/2016:21/p'|wc -l
# 排除相同IP
cat access.log.1| sed -n '/21\/May\/2016:21/,/21\/May\/2016:21/p'|awk '{print $1}'|uniq -c|wc -l
```

# 伪流媒体服务

```shell
apt-add-repository ppa:nginx/stable
apt-get update
apt-get install nginx
```

不过这个源将SPDY作为额外模块，因此如果你需要启用SPDY或者执行service nginx start后没有反应则应该执行：

```shell
apt-get install nginx-extras
# 查看模块
nginx -V
```

或者使用官方源

```shell
deb http://nginx.org/packages/ubuntu/ precise nginx
deb-src http://nginx.org/packages/ubuntu/ precise nginx
```

## 配置模块 ngx_http_mp4_module

为H.264/AAC文件，主要是以 .mp4、.m4v、和.m4a为扩展名的文件， 提供伪流媒体服务端支持。

伪流媒体是与Flash播放器一起配合使用的。 播放器向服务端发送HTTP请求，请求中的查询串是以开始时间为参数的(以 start简单命名)，而服务端以流响应，这样流的开始 位置就能于请求中的时间相对应。 例如：

`http://example.com/elephants_dream.mp4?start=238.88`
这样就允许随意拖放，或者从节目的中途开始回放。

```conf
location /live/ {
    mp4;
    mp4_buffer_size     1m;
    mp4_max_buffer_size 5m;
}
```

## 配置模块 ngx_http_flv_module

为Flash Video(FLV)文件 提供服务端伪流媒体支持

通过返回以请求偏移位置开始的文件内容，该模块专门处理 在查询串中有start参数的请求, 和有预先设置到FLV头部的请求。

```conf
location ~ \.flv$ {
    flv;
}
```

## 配置模块 ngx_http_hls_module

```conf
location / {
    hls;
    hls_fragment            5s;
    hls_buffers             10 10m;
    hls_mp4_buffer_size     1m;
    hls_mp4_max_buffer_size 5m;
    root /var/video/;
}
```

# 日志格式
```conf
log_format combined '$remote_addr-$remote_user [$time_local] "$request" $status $body_bytes_sent "$http_referer" "$http_user_agent"';
log_format main '$remote_addr - $remote_user [$time_local] "$request" $status $body_bytes_sent "$http_referer" "$http_user_agent"';
log_format main '$remote_addr - $remote_user [$time_local] "$request" $status $body_bytes_sent "$http_referer" "$http_user_agent" $http_x_forwarded_for "$request_time"';
```

# 限制IP

```shell
allow 127.0.0.0/24;
allow 192.168.0.0/16;
allow 192.168.1.0/24;
deny all;
```

# Nginx 执行步骤

nginx在处理每一个用户请求时，都是按照若干个不同的阶段依次处理的，与配置文件上的顺序没有关系，详细内容可以阅读《深入理解nginx:模块开发与架构解析》这本书，这里只做简单介绍；

（1）post-read

读取请求内容阶段，nginx读取并解析完请求头之后就立即开始运行；

（2）server-rewrite

server请求地址重写阶段；

（3）find-config

配置查找阶段，用来完成当前请求与location配重块之间的配对工作；

（4）rewrite

location请求地址重写阶段，当ngx_rewrite指令用于location中，就是再这个阶段运行的；

（5）post-rewrite

请求地址重写提交阶段，当nginx完成rewrite阶段所要求的内部跳转动作，如果rewrite阶段有这个要求的话；

（6）preaccess

访问权限检查准备阶段，ngx_limit_req和ngx_limit_zone在这个阶段运行，ngx_limit_req可以控制请求的访问频率，ngx_limit_zone可以控制访问的并发度；

（7）access

权限检查阶段，ngx_access在这个阶段运行，配置指令多是执行访问控制相关的任务，如检查用户的访问权限，检查用户的来源IP是否合法；

（8）post-access

访问权限检查提交阶段；

（9）try-files

配置项try_files处理阶段；

（10）content

内容产生阶段，是所有请求处理阶段中最为重要的阶段，因为这个阶段的指令通常是用来生成HTTP响应内容的；

（11）log

日志模块处理阶段；

ngx_lua属于nginx的一部分，它的执行指令都包含在nginx的11个步骤之中了，相应的处理阶段可以做插入式处理，即可插拔式架构，不过ngx_lua并不是所有阶段都会运行的；另外指令可以在http、server、server if、location、location if几个范围进行配置：


init_by_lua
init_by_lua_file

nginx Master进程加载配置时执行；通常用于初始化全局配置/预加载Lua模块

init_worker_by_lua
init_worker_by_lua_file

每个Nginx Worker进程启动时调用的计时器，如果Master进程不允许则只会在init_by_lua之后调用；通常用于定时拉取配置/数据，或者后端服务的健康检查

set_by_lua
set_by_lua_file

设置nginx变量，可以实现复杂的赋值逻辑；此处是阻塞的，Lua代码要做到非常快；

rewrite_by_lua
rewrite_by_lua_file

rewrite阶段处理，可以实现复杂的转发/重定向逻辑；

access_by_lua
access_by_lua_file

请求访问阶段处理，用于访问控制

content_by_lua
content_by_lua_file

内容处理器，接收请求处理并输出响应

header_filter_by_lua
header_filter_by_lua_file

设置header和cookie

body_filter_by_lua
body_filter_by_lua_file

对响应数据进行过滤，比如截断、替换。

log_by_lua
log_by_lua_file

log阶段处理，比如记录访问量/统计平均响应时间

# 启用php-fpm状态功能

`php-fpm.conf`

```conf
pm.status_path = /status
```

## 配置 
```conf
server {
    listen  *:80 default_server;
    server_name _;
    location ~ ^/(status|ping)$
    {
        include fastcgi_params;
        fastcgi_pass 127.0.0.1:9000;
        fastcgi_param SCRIPT_FILENAME $fastcgi_script_name;
    }
}
```

## 反向代理

```shell
server {
	location / {
		proxy_set_header Host $host;
		proxy_set_header X-Real-IP $remote_addr;
		proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;

		#禁用缓存
		proxy_buffering off;

		#反向代理的地址
		proxy_pass http://live1.eotu.com:81;     
	}
}
```

# 静态文件 POST 方式获取

```conf
error_page 405 =200 $uri;
```