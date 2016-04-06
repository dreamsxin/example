# 安装 nginx php5-fpm

```shell
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

## 配置php5监听端口 `/etc/php5/fpm/pool.d/www.conf`

```conf
listen = /var/run/php5-fpm.sock
# 改为
listen = 127.0.0.1:9000
```

## 修改配置 `/etc/nginx/sites-available/default`
```conf
server {
    listen   80;
    server_name localhost.dev;

    index index.php index.html index.htm;
    set $root_path '/var/www/phalcon/public';
    root $root_path;

    location / {
        try_files $uri $uri/ /index.php;
    }

    location ~ \.php$ {
	fastcgi_buffer_size 128k;
	fastcgi_buffers 32 32k;
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
}
```