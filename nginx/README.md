# Nginx

```shell
nginx -s reload
```

## location 语法


基本语法：location [=|~|~*|^~] /uri/ { … }

- = 严格匹配。如果这个查询匹配，那么将停止搜索并立即处理此请求。
- ~ 为区分大小写匹配(可用正则表达式)
- !~为区分大小写不匹配
- ~* 为不区分大小写匹配(可用正则表达式)
- !~*为不区分大小写不匹配
- ^~ 如果把这个前缀用于一个常规字符串,那么告诉nginx 如果路径匹配那么不测试正则表达式。

## 动态模块

https://www.nginx.com/resources/wiki/extending/converting/

Nginx 以前增加、修改一个第三方模块，需要重新编译源代码，所有的模块都是用静态链接的形式组织起来的。而 Nginx-1.9.11 开始支持动态模块加载 DSO(Dynamic Shared Objects), 可以实现运行时动态加载模块，而不用每次都要重新编译。

从使用的角度上来说，是增加了一个指令 load_modules 指令，来加载编译为 so 形式的动态模块。

该功能的实现还是挺简单的，主要包括如下几个方面:

- 将模块相关的类型定义和操作方法调整到新的文件 ngx_module.h 和 ngx_module.c 中
- 添加 dlopen 的封装
- 调整编译脚本
- 少量 Nginx 核心代码的调整。主要是为 cycle 新增了一个成员modules, 用来取代之前的全局变量 ngx_modules, 并将之前初始化时对于 ngx_modules 的操作封装成几个函数，放在 init_cycle 里调用。

我认为我们需要重点关注的是如下几个:

- 编译脚本有了较大的变化，在以后的模块编程中，尽量要让自己的模块能够静态链接和动态链接。这就需要适应一下新的编译框架。
- 不光旧的模块需要考虑兼容性，新的模块也要考虑对于旧的 Nginx 版本的兼容性。这包括两个方面，一个是 c 语言层面，要关注动态模块对于 Nginx 框架的调整，例如之前凡是用到全局变量 ngx_modules 的地方，要修改为 cycle->modules，当然正确的做法应该是用宏开关来判断当前 Nginx 版本是否大于 1.9.11 来决定是否调整；另一个是编译脚本，也需要判断版本，从而做出一些开关的选择，重点是对 ngx_module_link 变量的判断。

举个例子，对于 nginx-rtmp 模块，我们要将其修改为既能在 Nginx-1.9.11 及其以后的版本中，同时支持静态链接(–add-module)和动态链接(–add-dynamic-module)，又要让其在老的版本中依然能支持旧的编译框架下的静态链接(–add-module)。

为了达到这个目的，我做了如下测试，首先将 nginx-rtmp 的 congfig 文件修改为：

```conf
ngx_addon_name="ngx_rtmp_module"  
RTMP_CORE_MODULES="ngx_rtmp_module                             \  
                   ngx_rtmp_core_module                        \  
                   ngx_rtmp_cmd_module                         \  
                   ngx_rtmp_codec_module                       \  
                   ngx_rtmp_access_module                      \  
                   ngx_rtmp_record_module                      \  
                   ngx_rtmp_live_module                        \  
                   ngx_rtmp_play_module                        \  
                   ngx_rtmp_flv_module                         \  
                   ngx_rtmp_mp4_module                         \  
                   ngx_rtmp_netcall_module                     \  
                   ngx_rtmp_relay_module                       \  
                   ngx_rtmp_exec_module                        \  
                   ngx_rtmp_auto_push_module                   \  
                   ngx_rtmp_notify_module                      \  
                   ngx_rtmp_log_module                         \  
                   ngx_rtmp_limit_module                       \  
                   ngx_rtmp_hls_module                         \  
                   ngx_rtmp_dash_module                        \  
                   "  
RTMP_HTTP_MODULES="ngx_rtmp_stat_module                        \  
                   ngx_rtmp_control_module                     \  
                  "  
RTMP_DEPS="$ngx_addon_dir/ngx_rtmp_amf.h               \  
           $ngx_addon_dir/ngx_rtmp_bandwidth.h         \  
           $ngx_addon_dir/ngx_rtmp_cmd_module.h        \  
           $ngx_addon_dir/ngx_rtmp_codec_module.h      \  
           $ngx_addon_dir/ngx_rtmp_eval.h              \  
           $ngx_addon_dir/ngx_rtmp.h                   \  
           $ngx_addon_dir/ngx_rtmp_version.h           \  
           $ngx_addon_dir/ngx_rtmp_live_module.h       \  
           $ngx_addon_dir/ngx_rtmp_netcall_module.h    \  
           $ngx_addon_dir/ngx_rtmp_play_module.h       \  
           $ngx_addon_dir/ngx_rtmp_record_module.h     \  
           $ngx_addon_dir/ngx_rtmp_relay_module.h      \  
           $ngx_addon_dir/ngx_rtmp_streams.h           \  
           $ngx_addon_dir/ngx_rtmp_bitop.h             \  
           $ngx_addon_dir/ngx_rtmp_proxy_protocol.h    \  
           $ngx_addon_dir/hls/ngx_rtmp_mpegts.h        \  
           $ngx_addon_dir/dash/ngx_rtmp_mp4.h          \  
           "  
RTMP_CORE_SRCS="$ngx_addon_dir/ngx_rtmp.c                   \  
                $ngx_addon_dir/ngx_rtmp_init.c              \  
                $ngx_addon_dir/ngx_rtmp_handshake.c         \  
                $ngx_addon_dir/ngx_rtmp_handler.c           \  
                $ngx_addon_dir/ngx_rtmp_amf.c               \  
                $ngx_addon_dir/ngx_rtmp_send.c              \  
                $ngx_addon_dir/ngx_rtmp_shared.c            \  
                $ngx_addon_dir/ngx_rtmp_eval.c              \  
                $ngx_addon_dir/ngx_rtmp_receive.c           \  
                $ngx_addon_dir/ngx_rtmp_core_module.c       \  
                $ngx_addon_dir/ngx_rtmp_cmd_module.c        \  
                $ngx_addon_dir/ngx_rtmp_codec_module.c      \  
                $ngx_addon_dir/ngx_rtmp_access_module.c     \  
                $ngx_addon_dir/ngx_rtmp_record_module.c     \  
                $ngx_addon_dir/ngx_rtmp_live_module.c       \  
                $ngx_addon_dir/ngx_rtmp_play_module.c       \  
                $ngx_addon_dir/ngx_rtmp_flv_module.c        \  
                $ngx_addon_dir/ngx_rtmp_mp4_module.c        \  
                $ngx_addon_dir/ngx_rtmp_netcall_module.c    \  
                $ngx_addon_dir/ngx_rtmp_relay_module.c      \  
                $ngx_addon_dir/ngx_rtmp_bandwidth.c         \  
                $ngx_addon_dir/ngx_rtmp_exec_module.c       \  
                $ngx_addon_dir/ngx_rtmp_auto_push_module.c  \  
                $ngx_addon_dir/ngx_rtmp_notify_module.c     \  
                $ngx_addon_dir/ngx_rtmp_log_module.c        \  
                $ngx_addon_dir/ngx_rtmp_limit_module.c      \  
                $ngx_addon_dir/ngx_rtmp_bitop.c             \  
                $ngx_addon_dir/ngx_rtmp_proxy_protocol.c    \  
                $ngx_addon_dir/hls/ngx_rtmp_hls_module.c    \  
                $ngx_addon_dir/dash/ngx_rtmp_dash_module.c  \  
                $ngx_addon_dir/hls/ngx_rtmp_mpegts.c        \  
                $ngx_addon_dir/dash/ngx_rtmp_mp4.c          \  
                "  
RTMP_HTTP_SRCS="$ngx_addon_dir/ngx_rtmp_stat_module.c       \  
                $ngx_addon_dir/ngx_rtmp_control_module.c    \  
                "  
  
#nginx version >= 1.9.11  
if test -n "$ngx_module_link"; then  
    ngx_module_incs=$ngx_addon_dir  
    ngx_module_deps=$RTMP_DEPS  
    if [ $ngx_module_link = DYNAMIC ] ; then  
        ngx_module_name="$RTMP_CORE_MODULES $RTMP_HTTP_MODULES"  
        ngx_module_srcs="$RTMP_CORE_SRCS $RTMP_HTTP_SRCS"  
        . auto/module  
    elif [ $ngx_module_link = ADDON ] ; then  
        ngx_module_type=CORE  
        ngx_module_name=$RTMP_CORE_MODULES  
        ngx_module_srcs=$RTMP_CORE_SRCS  
        . auto/module  
        ngx_module_type=HTTP  
        ngx_module_name=$RTMP_HTTP_MODULES  
        ngx_module_srcs=$RTMP_HTTP_SRCS  
        . auto/module  
    fi  
  
#nginx version < 1.9.11  
else  
    CORE_MODULES="$CORE_MODULES  
                  $RTMP_CORE_MODULES"  
    HTTP_MODULES="$HTTP_MODULES  
                  $RTMP_HTTP_MODULES"  
    NGX_ADDON_DEPS="$NGX_ADDON_DEPS \  
                    $RTMP_DEPS"  
    NGX_ADDON_SRCS="$NGX_ADDON_SRCS  
                    $RTMP_CORE_SRCS  
                    $RTMP_HTTP_SRCS"
    CFLAGS="$CFLAGS -I$ngx_addon_dir"  
fi  
  
USE_OPENSSL=YES  
```

然后调整所有 c 源文件里使用 ngx_modules 的地方，可以添加类似如下的代码片段:

```c
#if defined(nginx_version) && nginx_version >= 1009011
    modules = cf->cycle->modules;  
#else
    modules = ngx_modules;  
#endif
```

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

## 检测配置文件

```shell
nginx -t
nginx -t -c my-nginx.conf  
```

## 配置

(1) keepalive_timeout
该参数用于设置客户端连接保持会话的超时时间，超过这个时间服务器会关闭该连接

(2) client_header_timeout
该参数用于设置读取客户端请求头数据的超时时间，如果超时客户端还没有发送完整的 header 数据，服务器将返回 "Request time out (408)" 错误

(3) client_body_timeout
该参数用于设置读取客户端请求主体数据的超时时间，如果超时客户端还没有发送完整的主体数据，服务器将返回 "Request time out (408)" 错误

(4) send_timeout
用于指定响应客户端的超时时间，如果超过这个时间，客户端没有任何活动，Nginx 将会关闭连接

(5) tcp_nodelay
默认情况下当数据发送时，内核并不会马上发送，可能会等待更多的字节组成一个数据包，这样可以提高 I/O 性能，但是，在每次只发送很少字节的业务场景中，使用 tcp_nodelay 功能，等待时间会比较长

(6) fastcgi_connect_timeout
链接

(7) fastcgi_read_timeout
读取

(8) fastcgi_send_timeout
发请求

## 平滑重启

```shell
kill -HUP Nginx主进程号  
Killall -s HUP nginx
```

当 nginx 接收到 HUP 信号时，它会尝试先解析配置文件，如果成功，就应用新的配置文件(例如，重新打开日志文件或监听的套接字)。之后，nginx 运行新的工作进程并从容关闭旧的工作进程。通知工作进程关闭监听套接字，但是继续为当前连接的客户提供服务。所有的客户端的服务完成后，旧的工作进程被关闭。如果新的配置文件应用失败，nginx 将继续使用旧的配置文件进行工作。

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
		#or fastcgi_pass unix:/run/php/php7.0-fpm.sock;
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
	# POST 请求静态文件
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

#以用户二进制IP地址，定义三个漏桶，滴落速率1-3req/sec，桶空间1m，1M能保持大约16000个(IP)状态
limit_req_zone $binary_remote_addr zone=qps1:1m  rate=1r/s;
limit_req_zone $binary_remote_addr zone=qps2:1m  rate=2r/s;
limit_req_zone $binary_remote_addr zone=qps3:1m  rate=3r/s;
```
2、在需要限制并发数和下载带宽的网站配置server{}里加上如下代码：
```ini
# 限制并发连接数
limit_conn perip 2;
# 限制同一server并发连接数
limit_conn perserver ;
# 限制下载速度
limit_rate 100k;

 
#速率qps=1，峰值burst=5，延迟请求
#严格按照漏桶速率qps=1处理每秒请求
#在峰值burst=5以内的并发请求，会被挂起，延迟处理
#超出请求数限制则直接返回503
#客户端只要控制并发在峰值[burst]内，就不会触发limit_req_error_log
# 例1：发起一个并发请求=6，拒绝1个，处理1个，进入延迟队列4个：
#time  request  refuse  sucess  delay
#00:01    6    1    1      4
#00:02    0    0    1      3
#00:03    0    0    1      2
#00:04    0    0    1      1
#00:05    0    0    1      0
location /delay {
    limit_req  zone=qps1 burst=5;
}
 
#速率qps=1，峰值burst=5，不延迟请求
#加了nodelay之后，漏桶控制一段时长内的平均qps = 漏桶速率，允许瞬时的峰值qps > 漏桶qps
#所以峰值时的最高qps=(brust+qps-1)=5
#请求不会被delay，要么处理，要么直接返回503
#客户端需要控制qps每秒请求数，才不会触发limit_req_error_log
# 例2：每隔5秒发起一次达到峰值的并发请求，由于时间段内平均qps=1 所以仍然符合漏桶速率：
#time  request   refuse  sucess
#00:01     5     0     5
#00:05     5     0     5
#00:10     5     0     5
# 例3：连续每秒发起并发请求=5，由于时间段内平均qps>>1，超出的请求被拒绝：
#time  request   refuse   sucess
#00:01     5     0      5
#00:02     5     4      1
#00:03     5     4      1
 
location /nodelay {
    limit_req  zone=qps1 burst=5 nodelay;
}
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

不过这个源将SPDY作为额外模块，因此如果你需要启用SPDY或者执行`service nginx start`后没有反应则应该执行：

```shell
apt-get install nginx-extras
# 查看模块
nginx -V
# grep 过滤模块
2>&1 nginx -V | tr ' '  '\n'|grep rtmp
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
log_format main ' $remote_user [$time_local] $http_x_Forwarded_for $remote_addr $request $http_x_forwarded_for $upstream_addr ups_resp_time: $upstream_response_time request_time: $request_time';
log_format combined '$remote_addr-$remote_user [$time_local] "$request" $status $body_bytes_sent "$http_referer" "$http_user_agent"';
log_format main '$remote_addr - $remote_user [$time_local] "$request" $status $body_bytes_sent "$http_referer" "$http_user_agent"';
log_format main '$remote_addr - $remote_user [$time_local] "$request" $status $body_bytes_sent "$http_referer" "$http_user_agent" $http_x_forwarded_for "$request_time" $request_body';
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

# Too many open files

```shell
ulimit -a
lsof -n|awk '{print$2}'|sort|uniq -c |sort -nr|more 
```

其中第一列是打开的文件句柄数量，第二列是进程号。

```shell
ps -aef|grep 进程号
```

查看当前系统打开的文件数量

```shell
lsof | wc -l  
watch "lsof | wc -l"
```

查看某一进程的打开文件数量

```shell
lsof -p pid | wc -l
lsof -p 1234 | wc -l
```

```shell
ulimit -SHn 40960
```

修改 `/etc/security/limits.conf`

```conf
* hard nofile 40960
* soft nofile 40960
```

`nginx.conf` 添加
```conf
worker_rlimit_nofile 30000;
```

# Resource temporarily unavailable

` /etc/php/7.1/fpm/pool.d/www.conf` 修改：
```conf
listen.backlog = 1024
```

## 负载均衡

```conf
upstream apis {
	least_conn;
        server localhost:80 weight=1;
        server localhost:81 weight=2;
}
server {
        listen 8282;
        server_name localhost;


        location / {
                # First attempt to serve request as file, then
                # as directory, then fall back to displaying a 404.
                try_files $uri $uri/ /index.php?$query_string;
        }

        location ^~ /api/ads {
                proxy_pass http://apis$request_uri$query_string$is_args$args;
                proxy_set_header Host $host;
                proxy_set_header X-Real-IP $remote_addr;
                proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;

                #proxy_buffering off;
        }

        # pass PHP scripts to FastCGI server
        location ~ \.php$ {
                limit_req zone=allips burst=5 nodelay;
                include snippets/fastcgi-php.conf;
                if ($request_uri ~ /api/) {
                        fastcgi_pass phpfpm;
                }
                if ($request_uri !~ /api/) {
                        fastcgi_pass 127.0.0.1:9001;
                }
                # With php-fpm (or other unix sockets):
                #fastcgi_pass unix:/var/run/php/php7.1-fpm.sock;
                ## With php-cgi (or other tcp sockets):
                #fastcgi_pass phpfpm;
        }
}

upstream apis {
        least_conn;
        server localhost:81;
}
server {
        listen 8282;
        server_name localhost;
        limit_conn perserver 200;
        keepalive_timeout 3;
	reset_timedout_connection on;
	client_header_timeout 1;
	client_body_timeout 2;
	send_timeout 2;
        location / {
                proxy_pass http://apis;
                proxy_set_header Host $host;
                proxy_set_header X-Real-IP $remote_addr;
                proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
                proxy_connect_timeout 1s;
                proxy_read_timeout 1s;
                proxy_send_timeout 1s;
                proxy_next_upstream_tries 1;
                proxy_next_upstream error off;
                proxy_next_upstream_timeout 1s;
                #proxy_set_header Authorization $http_authorization;
                #proxy_pass_header Authorization;
                #proxy_buffering off;
        }
}
```

## 日志


- log_not_found
默认为on：启用或禁用404错误日志

location = /favicon.ico {
	log_not_found off;
	access_log off;
}

`error_log off` 并不能关闭日志记录功能，它将日志文件写入`/usr/share/nginx/off`文件中，应使用以下配置：
`error_log /dev/null crit;`


默认日志格式：
```conf
log_format combined '$remote_addr - $remote_user [$time_local]  "$request" $status $body_bytes_sent "$http_referer" "$http_user_agent" $request_time';
```

查看当前TCP连接数
```shell
netstat -tan | grep "ESTABLISHED" | grep ":80" | wc -l
```

用 tcpdump 嗅探 80 端口的访问看看哪个IP最高
```shell
tcpdump -i eth0 -tnn dst port 80 -c 1000 | awk -F"." '{print $1"."$2"."$3"."$4}' | sort | uniq -c | sort -nr
```

统计每秒钟的请求数
```shell
awk '{print $4}' access.log | sort | uniq -c | sort -nr -k1 | head -n 10
```

统计每分钟的请求数
```shell
awk '{print $4}' access.log |cut -c 14-18|sort|uniq -c|sort -nr|head -n 10
```

每小时的请求数
```shell
awk '{print $4}' access.log |cut -c 14-15|sort|uniq -c|sort -nr|head -n 10
```

根据访问IP统计UV
```shell
awk '{print $1}'  access.log|sort | uniq -c |wc -l
```

统计访问URL统计PV
```shell
awk '{print $7}' access.log|wc -l
```

查询访问最频繁的URL
```shell
awk '{print $7}' access.log|sort | uniq -c |sort -n -k 1 -r|more
```

查询访问最频繁的IP
```shell
awk '{cnt[$1]++;}END{for(i in cnt){printf("%s\t%s\n", cnt[i], i);}}' access.log|sort -n
```

```shell
awk '{print $1}' access.log|sort | uniq -c |sort -n -k 1 -r|more
```

查看某一时间段的IP访问量(4-5点)
```shell
grep "07/Apr/2017:0[4-5]" access.log | awk '{print $1}' | sort | uniq -c| sort -nr | wc -l
```

查看访问100次以上的IP
```shell
awk '{print $1}' access.log | sort -n |uniq -c |awk '{if($1 >100) print $0}'|sort -rn
```

查询某个IP的详细访问情况,按访问频率排序
```shell
grep '127.0.0.1' access.log |awk '{print $7}'|sort |uniq -c |sort -rn |head -n 100
```

根据时间段统计查看日志
```shell
cat  access.log| sed -n '/14\/Mar\/2015:21/,/14\/Mar\/2015:22/p'|more
```

导出特定 `ip` 日志
```shell
grep "192.168.1.1" access.log > 192.168.1.1.log
```

列出传输时间超过 3 秒的页面（$request_time 在最后一列）
```shell
cat access.log|awk '($NF > 3){print $12}'|sort -n|uniq -c|sort -nr|head -10
```

统计流量
```shell
cat access.log|awk '{sum+=$10} END {print sum/1024/1024/1024}'
```

- `$10` 是 nginx 字段 `bytes_sent` 字段，根据自己的日志格式修改
- `body_bytes_sent` 发送给客户端的字节数,不包括响应头的大小
- `bytes_sent`  发送给客户端的字节数

计算最高带宽
```shell
cat access.log|awk -F'[: ]' '{a[$5":"$6]+=$14}END{for(i in a){print i,a[i]}}'|sort|awk '{a+=$2;if(NR%5==0){if(a>b){b=a;c=$1};a=0}}END{print c,b*8/300/1024/1024}'
```

## 统计请求时间

```conf
log_format _main '$remote_addr "$time_iso8601" $request_method "$request_uri" $status $body_bytes_sent $request_time "$http_user_agent" "$args" "$request_body"';
```

```shell
#! /bin/bash

function cal_work(){
        ls | grep 'access.log' | xargs cat | awk   'BEGIN{FPAT="[^ ]+|\"[^\"]+\"";max=0;min=1}
                {if ($4 ~ /^"'"$1"'/  && $(NF-3)==200){
                        sum+=$7; count+=1;
                        if($7 > max) max=$7 fi;if($7 < min) min=$7 fi;
                    }
                }
                END {print "Average = ", sum/count;print "Max = ", max;print "Min", min;print "Sum", sum;print "Count", count}'
}

function get_stat(){
        target=$1
        target=${target//\//\\/}
        cal_work $target
}

get_stat '/api/ads'
```

```shell
#! /bin/bash

function cal_work(){
        ls | grep 'access.log' | xargs cat | awk   'BEGIN{max=0;min=1}
                {if ($4 ~ /^"'"$1"'$/){
                        sum+=$7; count+=1;
                        if($7 > max) max=$7 fi;if($7 < min) min=$7 fi;
                    }
                }
                END {print "Average = ", sum/count;print "Max = ", max;print "Min", min;print "Sum", sum;print "Count", count}'
}

function get_stat(){
        target=$1
        target=${target//\//\\/}
        cal_work $target
}

get_stat '/api/ads'
```