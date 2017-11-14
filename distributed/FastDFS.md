# FastDFS

FastDFS是一款类GoogleFS的开源分布式文件系统，它用纯C语言实现，支持Linux、FreeBSD、AIX等UNIX系统。它只能通过专有API对文件进行存取访问，不支持POSIX接口方式，不能mount使用。准确地讲，GoogleFS以及FastDFS、mogileFS、HDFS、TFS等类GoogleFS都不是系统级的分布式文件系统，而是应用级的分布式文件存储服务。

FastDFS是为互联网应用量身定做的分布式文件系统，充分考虑了冗余备份、负载均衡、线性扩容等机制，并注重高可用、高性能等指标。和现有的类Google FS分布式文件系统相比，FastDFS的架构和设计理念有其独到之处，主要体现在轻量级、分组方式和对等结构三个方面。

FastDFS是由国人余庆所开发，其项目地址：https://github.com/happyfish100 。

## 架构图

FastDFS服务端有两个角色：跟踪器（tracker）和存储节点（storage）。跟踪器主要做调度工作，在访问上起负载均衡的作用。

![https://github.com/dreamsxin/example/blob/master/distributed/img/FastDFS.png](https://github.com/dreamsxin/example/blob/master/distributed/img/FastDFS.png?raw=true)

## 模块介绍

- tracker server：跟踪服务器，用来调度来自客户端的请求，且在内存中记录所有存储组和存储服务器的信息状态。
- storage server：存储服务器，用来存储文件(data)和文件属性(metadata)。
- client：客户端，业务请求发起方，通过专用接口基于TCP协议与tracker server和storage server进行交互。
- group：组，也可称为卷，同组内上的文件是完全相同的。
- 文件标识：包括两部分，组名（group）和文件名（含路径）
- 文件相关属性：键值对(Key Value Pair)方式
- 文件名：与原文件名并不相同。由storage server根据特定信息生成，并且可逆，文件名包含：源存储服务器的IP地址、文件创建时间戳、文件大小、随机数和文件扩展名等。

## 安装包准备

执行下面的shell，下载安装包，分别是libfastcommon基础组件、fastdfs、nginx、fastdfs-nginx-module。

```shell
wget https://github.com/happyfish100/libfastcommon/archive/V1.0.7.tar.gz
wget https://github.com/happyfish100/fastdfs/archive/V5.05.tar.gz
wget http://nginx.org/download/nginx-1.11.8.tar.gz
wget http://jaist.dl.sourceforge.NET/project/fastdfs/FastDFS%20Nginx%20Module%20Source%20Code/fastdfs-nginx-module_v1.16.tar.gz
```

## 安装libfastcommon基础组件

```shell
tar -zxvf V1.0.7.tar.gz
cd libfastcommon-1.0.7/
./make.sh
sudo ./make.sh install
ln -s /usr/lib64/libfastcommon.so /usr/local/lib/libfastcommon.so
ln -s /usr/lib64/libfastcommon.so /usr/lib/libfastcommon.so
```

## 安装 fastdfs

```shell
tar -zxvf V5.05.tar.gz
cd fastdfs-5.05/
./make.sh
sudo ./make.sh install
ln -s /usr/lib64/libfdfsclient.so /usr/local/lib/libfdfsclient.so
ln -s /usr/lib64/libfdfsclient.so /usr/lib/libfdfsclient.so
```

目录规划

- /usr/bin
存放有编译出来的文件，包含 fdfs_trackerd、fdfs_storaged、fdfs_monitor、fdfs_upload_file 等

- /etc/fdfs
存放有配置文件

## 配置文件

复制一份 tracker 配置文件：
```shell
cp /etc/fdfs/tracker.conf.sample /etc/fdfs/tracker.conf
```
编辑 `/etc/fdfs/tracker.conf`：
```conf
# the base path to store data and log files
base_path=/home/kevin/fastdfs/tracker/data-and-log
```
复制一份 storage 配置文件：
```shell
cp /etc/fdfs/storage.conf.sample /etc/fdfs/storage.conf
```
编辑 `/etc/fdfs/storage.conf`：
```conf
# the base path to store data and log files
base_path=/home/kevin/fastdfs/storage

# store_path#, based 0, if store_path0 not exists, it's value is base_path
# the paths must be exist
store_path0=/home/kevin/fastdfs/storage/images-data0
store_path1=/home/kevin/fastdfs/storage/images-data1
store_path2=/home/kevin/fastdfs/storage/images-data2

# tracker_server can ocur more than once, and tracker_server format is
#  "host:port", host can be hostname or ip address
tracker_server=192.168.1.32:22122
```
备注：storage （存储节点）服务部署，一般 storage 服务我们会单独装一台机子，但是这里为了方便我们安装在同一台。如果 storage 单独安装的话，那上面安装的所有步骤都要在走一遍，只是到了编辑配置文件的时候，编辑的是 storage.conf 而已。

## 启动 tracker server
```shell
/usr/bin/fdfs_trackerd /etc/fdfs/tracker.conf
/usr/bin/fdfs_trackerd /etc/fdfs/tracker.conf restart
ps aux | grep tracker
```

## 启动 storage server
```shell
/usr/bin/fdfs_storaged /etc/fdfs/storage.conf
/usr/bin/fdfs_storaged /etc/fdfs/storage.conf restart
ps aux | grep storage
```

## client测试

`/etc/fdfs/client.conf`：
```conf
# the base path to store log files
base_path=/home/kevin/fastdfs/client/data-and-log

# tracker_server can ocur more than once, and tracker_server format is
#  "host:port", host can be hostname or ip address
tracker_server=192.168.1.32:22122
```
通过fdfs_test上传的图片，保存了两张图片：
```shell
wget http://img.tuku.cn/file_thumb/201504/m2015042221045424.jpg
/usr/bin/fdfs_test /etc/fdfs/client.conf upload ./m2015042221045424.jpg
find . -name "*.jpg"
./data/00/00/wKgBIFmemtyAGcebAAAdgkSCZh4105.jpg
./data/00/00/wKgBIFmemtyAGcebAAAdgkSCZh4105_big.jpg
```
通过fdfs_upload_file上传，保存一张原始图片：
```shell
/usr/bin/fdfs_upload_file /etc/fdfs/client.conf m2015042221045424.jpg
group1/M00/00/00/wKgBIFmenECAAOdbAAAdgkSCZh4066.jpg
```
```text
$ find . -name "*.jpg" | xargs ls -l
-rw-r--r-- 1 root root 7554  8月 24 17:22 ./data/00/00/wKgBIFmemtyAGcebAAAdgkSCZh4105_big.jpg
-rw-r--r-- 1 root root 7554  8月 24 17:22 ./data/00/00/wKgBIFmemtyAGcebAAAdgkSCZh4105.jpg
-rw-r--r-- 1 root root 7554  8月 24 17:28 ./data/00/00/wKgBIFmenECAAOdbAAAdgkSCZh4066.jpg
```
即使我们现在知道图片的访问地址我们也访问不了，因为我们还没装 FastDFS 的 Nginx 模块。

## 安装 nginx 及其插件

修改 `/opt/setups/FastDFS/fastdfs-nginx-module/src/config` 中
`CORE_INCS="$CORE_INCS /usr/local/include/fastdfs /usr/local/include/fastcommon/"`
修改为
`CORE_INCS="$CORE_INCS /usr/include/fastdfs /usr/include/fastcommon/"`

```shell
sudo apt-get install openssl libssl-dev
sudo apt-get install libpcre3 libpcre3-dev  
sudo apt-get install zlib1g-dev 

cp fastdfs-5.05/conf/http.conf /etc/fdfs
cp fastdfs-5.05/conf/mime.types /etc/fdfs

dpkg -l | grep zlib

mkdir -p /usr/local/nginx /var/log/nginx /var/temp/nginx /var/lock/nginx

./configure \
--prefix=/usr/local/nginx \
--pid-path=/var/local/nginx/nginx.pid \
--lock-path=/var/lock/nginx/nginx.lock \
--error-log-path=/var/log/nginx/error.log \
--http-log-path=/var/log/nginx/access.log \
--with-http_gzip_static_module \
--http-client-body-temp-path=/var/temp/nginx/client \
--http-proxy-temp-path=/var/temp/nginx/proxy \
--http-fastcgi-temp-path=/var/temp/nginx/fastcgi \
--http-uwsgi-temp-path=/var/temp/nginx/uwsgi \
--http-scgi-temp-path=/var/temp/nginx/scgi \
--add-module=/home/kevin/fastdfs/install/soft/fastdfs-nginx-module/src

make
sudo make install
cp fastdfs-nginx-module/src/mod_fastdfs.conf /etc/fdfs
```

确认nginx的fastdfs插件是否安装成功

```text
/usr/local/nginx/sbin/nginx -V

nginx version: nginx/1.11.8
built by gcc 4.8.4 (Ubuntu 4.8.4-2ubuntu1~14.04.3) 
configure arguments: --prefix=/usr/local/nginx --pid-path=/var/local/nginx/nginx.pid --lock-path=/var/lock/nginx/nginx.lock --error-log-path=/var/log/nginx/error.log --http-log-path=/var/log/nginx/access.log --with-http_gzip_static_module --http-client-body-temp-path=/var/temp/nginx/client --http-proxy-temp-path=/var/temp/nginx/proxy --http-fastcgi-temp-path=/var/temp/nginx/fastcgi --http-uwsgi-temp-path=/var/temp/nginx/uwsgi --http-scgi-temp-path=/var/temp/nginx/scgi --add-module=/home/kevin/fastdfs/install/soft/fastdfs-nginx-module/src
```

## 配置文件

编辑`/etc/fdfs/mod_fastdfs.conf`
```conf
# the base path to store log files
base_path=/home/kevin/fastdfs/fastdfs-nginx-module/data-and-log/

# FastDFS tracker_server can ocur more than once, and tracker_server format is
#  "host:port", host can be hostname or ip address
# valid only when load_fdfs_parameters_from_tracker is true
tracker_server=192.168.1.32:22122

# if the url / uri including the group name
# set to false when uri like /M00/00/00/xxx
# set to true when uri like ${group_name}/M00/00/00/xxx, such as group1/M00/xxx
# default value is false
url_have_group_name = true

# store_path#, based 0, if store_path0 not exists, it's value is base_path
# the paths must be exist
# must same as storage.conf
store_path0=/home/kevin/fastdfs/storage/images-data0
store_path1=/home/kevin/fastdfs/storage/images-data1
store_path2=/home/kevin/fastdfs/storage/images-data2
```
编辑 `/usr/local/nginx/conf/nginx.conf`
```conf
# 在http添加对其他配置文件的引用
include /usr/local/nginx/conf/conf.d/*.conf;
```
在 `/usr/local/nginx/conf/` 目录下创建文件夹 `conf.d`，并编辑 `fdfs.conf`，完成 nginx 与 fastdfs 的对接
```conf
server {
	listen       8888;
	server_name  localhost;
	location ~/group[0-9]/ {
		ngx_fastdfs_module;
	}
	error_page   500 502 503 504  /50x.html;
	location = /50x.html {
		root   html;
	}
}
```