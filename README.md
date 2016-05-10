example
=======

各种代码实例


双网卡NAT
```shell
sudo iptables -F
sudo iptables -t nat -A POSTROUTING -o eth0 -j MASQUERADE #打开NAT
sudo iptables -t nat -A POSTROUTING -s 192.168.1.0/24 -o eth1 -j SNAT --to-source xxx.xxx.xxx.xxx
sudo iptables -t nat -A POSTROUTING -s 192.168.1.0/24 -d ! 192.168.1.0/24 -j SNAT --to-source xxx.xxx.xxx.xxx
sudo iptables -A FORWARD -s 192.168.1.0/24 -p tcp -m tcp --tcp-flags SYN,RST SYN -j TCPMSS --set-mss 1200
#sudo ifconfig eth1 mtu 1500
#sudo ifconfig eth0 mtu 1500
```

```shell
./configure --prefix=/usr --with-apxs2=/usr/bin/apxs2 --with-config-file-path=/etc/php5/apache2 \
	--with-config-file-scan-dir=/etc/php5/apache2/conf.d --build=x86_64-linux-gnu \
	--host=x86_64-linux-gnu --sysconfdir=/etc --localstatedir=/var --mandir=/usr/share/man \
	--disable-debug --with-regex=php --disable-rpath --disable-static --with-pic --with-layout=GNU \
	--with-pear=/usr/share/php --without-gdbm --with-iconv --enable-exif --enable-ftp --with-gettext \
	--enable-mbstring --with-pcre-regex=/usr --enable-shmop --enable-sockets --enable-wddx \
	--with-libxml-dir=/usr --with-zlib --enable-zip --without-mm --with-curl=shared,/usr \
	--with-zlib-dir=/usr --with-gd=shared,/usr --enable-gd-native-ttf --with-jpeg-dir=shared,/usr \
	--with-png-dir=shared,/usr --enable-intl=shared --without-t1lib --with-libdir=/lib/x86_64-linux-gnu \
	--enable-maintainer-zts --enable-pdo --with-pdo-mysql --with-pdo-pgsql --with-pdo-sqlite --with-pgsql \
	--disable-dom --disable-simplexml --disable-xml --without-pear
```
```shell
./configure --prefix=/usr --with-apxs2=/usr/bin/apxs2 --with-config-file-path=/etc/php5/apache2 --with-config-file-scan-dir=/etc/php5/apache2/conf.d --build=x86_64-linux-gnu --host=x86_64-linux-gnu --sysconfdir=/etc --localstatedir=/var --mandir=/usr/share/man --disable-debug --with-regex=php --disable-rpath --disable-static --with-pic --with-layout=GNU --with-pear=/usr/share/php --without-gdbm --with-iconv --enable-exif --enable-ftp --with-gettext --enable-mbstring --with-pcre-regex=/usr --enable-shmop --enable-sockets --with-libxml-dir=/usr --with-zlib --enable-zip --without-mm --with-curl=shared,/usr --with-zlib-dir=/usr --enable-gd-native-ttf --with-jpeg-dir=shared,/usr --with-png-dir=shared,/usr --enable-intl=shared --without-t1lib --with-libdir=/lib/x86_64-linux-gnu --enable-maintainer-zts --enable-pdo --with-pdo-mysql --with-pdo-pgsql --with-pdo-sqlite --with-pgsql --disable-dom --disable-xml --without-pear --disable-simplexml
```
# Apache 使用证书
证书生成
```shell
sudo openssl req -x509 -nodes -days 7650 -newkey rsa:2048 -keyout /etc/apache2/ssl/apache.key -out /etc/apache2/ssl/apache.crt
```
```out
Country Name (2 letter code) [AU]:US
State or Province Name (full name) [Some-State]:New York
Locality Name (eg, city) []:New York City
Organization Name (eg, company) [Internet Widgits Pty Ltd]:Your Company
Organizational Unit Name (eg, section) []:Department of Kittens
Common Name (e.g. server FQDN or YOUR name) []:your_domain.com
Email Address []:your_email@domain.com
```
修改配置文件
```conf
<VirtualHost _default_:443>
        ServerAdmin webmaster@localhost
        ServerName your_domain.com
        DocumentRoot /var/www/html/eotu/new/public/api
        <Directory /var/www/html/eotu/new/public/api/>
                Options Indexes FollowSymLinks
                AllowOverride all
                Order deny,allow
                Allow from all
        </Directory>
        # Available loglevels: trace8, ..., trace1, debug, info, notice, warn,
        # error, crit, alert, emerg.
        # It is also possible to configure the loglevel for particular
        # modules, e.g.
        #LogLevel info ssl:warn

        ErrorLog ${APACHE_LOG_DIR}/eotu-error.log
        CustomLog ${APACHE_LOG_DIR}/eotu-access.log combined

        SSLEngine on

        SSLCertificateFile /etc/apache2/ssl/apache.crt
        SSLCertificateKeyFile /etc/apache2/ssl/apache.key
        <FilesMatch "\.(cgi|shtml|phtml|php)$">
                        SSLOptions +StdEnvVars
        </FilesMatch>
        <Directory /usr/lib/cgi-bin>
                        SSLOptions +StdEnvVars
        </Directory>

        BrowserMatch "MSIE [2-6]" \
                        nokeepalive ssl-unclean-shutdown \
                        downgrade-1.0 force-response-1.0
        # MSIE 7 and newer should be able to use keepalive
        BrowserMatch "MSIE [17-9]" ssl-unclean-shutdown

</VirtualHost>
```

开启扩展或启用站点
```shell
sudo a2enmod ssl
sudo a2ensite default-ssl
```

Ubuntu

```shell
apt-get install -y language-pack-en-base
vi /etc/default/locale
sudo locale-gen
sudo dpkg-reconfigure locales
apt-get install software-properties-common
add-apt-repository ppa:ondrej/php5
```