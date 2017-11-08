# OpenResty

```shell
sudo apt-get install build-essential
sudo apt-get install libreadline-dev libncurses5-dev libpcre3-dev libssl-dev perl

wget https://openresty.org/download/openresty-1.11.2.2.tar.gz
wget https://openresty.org/download/openresty-1.11.2.2.tar.gz.asc
gpg --keyserver pgpkeys.mit.edu --recv-key A0E98066
gpg openresty-1.11.2.2.tar.gz.asc

tar -xvf openresty-1.11.2.2.tar.gz
cd openresty-1.11.2.2
./configure -j2 --with-pcre-jit --with-ipv6
make -j2
sudo make install

sudo ufw allow http
sudo ufw status

sudo /usr/local/openresty/bin/openresty -s quit
```