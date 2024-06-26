
## 

```shell
wget ftp://ftp.gnu.org/gnu/autoconf/autoconf-2.68.tar.gz
tar zxvf autoconf-2.68.tar.gz
cd autoconf-2.68
./configure --prefix=/usr/
make && make install
```

## CentOS 6 配置第三方源

```shell
wget http://www.atomicorp.com/installers/atomic
sh ./atomic
yum check-update
```

## 安装Tab键自动补全功能：bash-completion

## 防火墙

```shell
yum install -y iptables
yum update -y iptables
yum install -y iptables-services

iptables -I INPUT -p tcp --dport 80 -j ACCEPT
/etc/rc.d/init.d/iptables save
service iptables restart
```

## yum 使用

```shell
yum whatprovides
```
● yum provides/yum whatprovides
这个命令可以查找哪个软件包提供请求的相关内容,可以使用通配符。

● yum list [available|installed|extras|updates|obsoletes|all|recent] [pkgspec]
使用这个命令你可以列出任何在你系统上启用或者安装的仓库里的软件包。

● yum list
默认情况下无参数的‘yum list‘列出在所有软件仓库里的和系统安装的所有的软件包。

● yum list available
列出在所有可用的软件仓库里可以安装的软件包

● yum list installed
这个命令和 rpm -qa 的功能是一样的，可以列出系统上安装的软件列表

● yum list extra
这个命令获得在系统上安装但是没有出现在激活的软件仓库里的软件包列表。用于寻找等待升级或者不是从 fedora 更新镜像上安装的软件包。

● yum list obsoletes
这个命令列出系统上安装但是被软件仓库废除的软件包列表

● yum list updates
这个命令列出已安装的可升级的软件包列表

● yum list recent
这个命令列出最近 7 天加入的软件包列表

● yum list pkgspec

● yum install/remove/update

● yum check-update
很像 yum list updates 但是如果有可用更新的话会返回退出代号 100。

● yum info
显示已安装或者可获得的软件包详细信息，它可以使用和 yum list 一样的参数但最好
为它指定一个软件包名

## rpm 使用

● rpm -qa -－last
这个命令反序列出已安装的软件包和安装时间，因此,最近安装的软件包会被列在前面。

● rpm -qa --qf "%{size} %{name}.%{arch}\n" | sort -n
这个命令显示系统上的所有软件包的大小(按字节)，名字.架构(显示格式)的列表，并按软件包的大小从小到大的顺序排列。

● rpm --showrc
这个命令显示 rpm 资源配置状态

● rpm -Va --nofiles --nomd5
这个命令检查你的系统中 rpm 数据库里未解决的包依赖关系

● rpm --eval <an rpm macro>
这个命令根据你指定的宏或者命令输出 rpm 的返回值

## 查看防火墙

```shell
iptables -L -n
```

## 防火墙开放端口

```shell
/sbin/iptables -I INPUT -p tcp --dport 80 -j ACCEPT
/sbin/iptables -I INPUT -p tcp --dport 22 -j ACCEPT

/etc/rc.d/init.d/iptables save
```

## 安装 Nginx
第一步，在/etc/yum.repos.d/目录下创建一个源配置文件`/etc/yum.repos.d/nginx.repo`：

```text
[nginx]
name=nginx repo
baseurl=http://nginx.org/packages/centos/$releasever/$basearch/
gpgcheck=0
enabled=1
```
保存，则会产生一个/etc/yum.repos.d/nginx.repo文件。

下面直接执行如下指令即可自动安装好Nginx：
```shell
yum install nginx -y
```
安装完成，下面直接就可以启动Nginx了：
```shell
/etc/init.d/nginx start
```
现在Nginx已经启动了，直接访问服务器就能看到Nginx欢迎页面了的。

如果还无法访问，则需配置一下Linux防火墙。
```shell
iptables -I INPUT 5 -i eth0 -p tcp --dport 80 -m state --state NEW,ESTABLISHED -j ACCEPT
service iptables save
service iptables restart
```

Nginx的命令以及配置文件位置：

```shell
/etc/init.d/nginx start # 启动Nginx服务
/etc/init.d/nginx stop # 停止Nginx服务
/etc/nginx/nginx.conf # Nginx配置文件位置
chkconfig nginx on    #设为开机启动
```

## 安装 php7

```shell
yum -y install ntp make openssl openssl-devel pcre pcre-devel libpng libpng-devel libjpeg-6b libjpeg-devel-6b freetype freetype-devel gd gd-devel zlib zlib-devel gcc gcc-c++ libXpm libXpm-devel ncurses ncurses-devel libmcrypt libmcrypt-devel libxml2 libxml2-devel imake autoconf automake screen sysstat compat-libstdc++-33 curl curl-devel
```

CentOS/RHEL 7.x:
```shell
rpm -Uvh https://dl.fedoraproject.org/pub/epel/epel-release-latest-7.noarch.rpm
rpm -Uvh https://mirror.webtatic.com/yum/el7/webtatic-release.rpm
```

CentOS/RHEL 6.x:
```shell
rpm -Uvh https://mirror.webtatic.com/yum/el6/latest.rpm
```

```shell
yum install httpd
yum install php71w php71w-pdo php71w-devel
```

## postgresql 配置

```shell
service postgresql-9.5 initdb
```

`/var/lib/pgsql/9.5/data/pg_hba.conf`


## 关闭 selinux

```shell
sed -i "s#SELINUX=enforcing#SELINUX=disabled#g" /etc/selinux/config  #重启生效
setenforce 0   #临时关闭
```
