`https://docs.hhvm.com/hhvm/installation/building-from-source`

Ubuntu 14.04 Trusty

```shell
# installs add-apt-repository
sudo apt-get install software-properties-common

sudo apt-key adv --recv-keys --keyserver hkp://keyserver.ubuntu.com:80 0x5a16e7281be7a449
sudo add-apt-repository "deb http://dl.hhvm.com/ubuntu $(lsb_release -sc) main"
sudo apt-get update
sudo apt-get install hhvm
```
Ubuntu 15.04 Vivid
```shell
# installs add-apt-repository
sudo apt-get install software-properties-common

sudo apt-key adv --recv-keys --keyserver hkp://keyserver.ubuntu.com:80 0x5a16e7281be7a449
sudo add-apt-repository "deb http://dl.hhvm.com/ubuntu $(lsb_release -sc) main"
sudo apt-get update
sudo apt-get install hhvm
```
Ubuntu 15.10 Wily Werewolf
```shell
# installs add-apt-repository
sudo apt-get install software-properties-common

sudo apt-key adv --recv-keys --keyserver hkp://keyserver.ubuntu.com:80 0x5a16e7281be7a449
sudo add-apt-repository "deb http://dl.hhvm.com/ubuntu $(lsb_release -sc) main"
sudo apt-get update
sudo apt-get install hhvm
```

```shell
sudo locale-gen en_EN
sudo locale-gen en_UK
sudo locale-gen en_US
sudo locale-gen en_GB
sudo locale-gen de_DE
sudo locale-gen fr_FR
sudo locale-gen fa_IR
sudo locale-gen zh_CN.utf8
sudo locale-gen zh_CN

sudo apt-get install autoconf automake binutils-dev build-essential cmake g++ gawk git \
  libboost-dev libboost-filesystem-dev libboost-program-options-dev libboost-regex-dev \
  libboost-system-dev libboost-thread-dev libboost-context-dev libbz2-dev libc-client-dev libldap2-dev \
  libc-client2007e-dev libcap-dev libcurl4-openssl-dev libdwarf-dev libelf-dev \
  libexpat-dev libgd2-xpm-dev libgoogle-glog-dev libgoogle-perftools-dev libicu-dev \
  libjemalloc-dev libmcrypt-dev libmemcached-dev libmysqlclient-dev libncurses-dev \
  libonig-dev libpcre3-dev libreadline-dev libtbb-dev libtool libxml2-dev zlib1g-dev \
  libevent-dev libmagickwand-dev libinotifytools0-dev libiconv-hook-dev libedit-dev \
  libiberty-dev libxslt1-dev ocaml-native-compilers libsqlite3-dev libyaml-dev libgmp3-dev \
  gperf libkrb5-dev libnotify-dev libboost-chrono1.54-dev
```

```shell
mkdir dev
cd dev
git clone git://github.com/facebook/hhvm.git --depth=1
export CMAKE_PREFIX_PATH=`pwd`
cd hhvm
git submodule update --init --recursive
cmake .
make -j 4
sudo make install
```

# FastCGI

```shell
hhvm --mode server -d hhvm.server.type=fastcgi -d hhvm.server.port=9000

hhvm --mode server -d hhvm.server.type=fastcgi -d hhvm.server.file_socket=/var/run/hhvm/sock

hhvm --mode daemon -d hhvm.server.type=fastcgi -d hhvm.server.file_socket=/var/run/hhvm/sock
```

## apache2

开启模块 `mod_proxy` `mod_proxy_fcgi`

```conf
ProxyPass / fcgi://127.0.0.1:9000/path/to/your/www/root/goes/here/
# Or if you used a unix socket
# ProxyPass / unix://var/run/hhvm/sock|fcgi://127.0.0.1:9000/path/to/your/www/root/goes/here/
```

## nginx
```conf
root /path/to/your/www/root/goes/here;
fastcgi_pass   127.0.0.1:9000;
# or if you used a unix socket
# fastcgi_pass   unix:/var/run/hhvm/sock;
fastcgi_index  index.php;
fastcgi_param  SCRIPT_FILENAME $document_root$fastcgi_script_name;
include        fastcgi_params;
```