# 查看模块
```shell
apache2ctl -l
sudo apache2ctl -M
```

# 开启限速模块
文档 `http://httpd.apache.org/docs/2.4/mod/mod_ratelimit.html`
```shell
sudo a2enmod ratelimit
```

# 修改配置
```shell
<Directory /var/www/html/>
    SetOutputFilter RATE_LIMIT
    SetEnv rate-limit 400 
</Directory>
```

## Apache 2.4 使用 PHP FPM
```conf
<FilesMatch \.php$>
         SetHandler "proxy:fcgi://127.0.0.1:9000"
</FilesMatch>
```

```conf
sudo vi /etc/apache2/mods-enabled/mpm_prefork.conf
<IfModule mpm_prefork_module>
        ServerLimit 1000
        StartServers        10
        MinSpareServers     20
        MaxSpareServers     100
        MaxRequestWorkers   1000
        MaxConnectionsPerChild   0
</IfModule>
```

```conf
listen = 127.0.0.1:9000
pm = dynamic
pm.max_children = 200
pm.start_servers = 100
pm.min_spare_servers = 30
pm.max_spare_servers = 200
```

# 记录请求数据到日志
```shell
sudo a2enmod dump_io
```

## 修改配置 `apache2.conf`
```conf
DumpIOInput On

# Enable dumping of all output at a specific LogLevel level such as notice or debug:
DumpIOLogLevel debug

# If you want to enable dumping of all output, enter:
DumpIOOutput On
```

# Apache 使用证书

## 证书生成
```shell
sudo mkdir /etc/apache2/ssl
sudo openssl req -x509 -nodes -days 7650 -newkey rsa:2048 -keyout /etc/apache2/ssl/apache.key -out /etc/apache2/ssl/apache.crt
# or
sudo openssl req -x509 -nodes -days 7650 -newkey rsa:2048 -keyout /etc/ssl/private/ssl-cert-snakeoil.key -out /etc/ssl/certs/ssl-cert-snakeoil.pem
```
```out
Country Name (2 letter code) [AU]:CN
State or Province Name (full name) [Some-State]:JX
Locality Name (eg, city) []:Fu Zhou
Organization Name (eg, company) [Internet Widgits Pty Ltd]:MYLEFT
Organizational Unit Name (eg, section) []:MYLEFT
Common Name (e.g. server FQDN or YOUR name) []:*.myleft.com
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

## 开启扩展或启用站点
```shell
sudo a2enmod ssl
sudo a2ensite default-ssl
```

## HTTPS 反向代理
```shell
<VirtualHost _default_:443>
        ServerAdmin webmaster@localhost
        ServerName your_domain.com

	ProxyPassReverseCookieDomain .eotu.com mobile.eotu.com
	ProxyPass / http://mobile1.eotu.com:81/
	ProxyPassReverse /  http://mobile1.eotu.com:81/
	ErrorLog ${APACHE_LOG_DIR}/eotu-error.log
	CustomLog ${APACHE_LOG_DIR}/eotu-access.log combined
	
	SSLEngine on

        SSLCertificateFile /etc/apache2/ssl/apache.crt
        SSLCertificateKeyFile /etc/apache2/ssl/apache.key

        BrowserMatch "MSIE [2-6]" nokeepalive ssl-unclean-shutdown downgrade-1.0 force-response-1.0
        BrowserMatch "MSIE [17-9]" ssl-unclean-shutdown
</VirtualHost>
```