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

## 安装 nginx-rtmp-module
```shell
sudo apt-get source nginx
cd nginx-1.8.1/debian/modules/
sudo git clone https://github.com/arut/nginx-rtmp-module.git
```

编辑 `debian/rules` 追加 `--add-module=$(MODULESDIR)/nginx-rtmp-module`

下载nginx的依赖包
`sudo apt-get build-dep nginx`

打包
`sudo dpkg-buildpackage -b`

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