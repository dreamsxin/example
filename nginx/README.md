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

	error_page 405 =200 $uri;
}
```

```conf
pm = static
pm.max_children = 30
request_terminate_timeout = 900
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