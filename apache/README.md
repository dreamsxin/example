# 安装

```shell
sudo apt-get install apache2  libapache2-mod-php7.1
sudo a2enmod rewrite
```

# 查看模块
```shell
apache2ctl -l
sudo apache2ctl -M
```
```

# 修改配置

```shell
<VirtualHost *:81>
        # The ServerName directive sets the request scheme, hostname and port that
        # the server uses to identify itself. This is used when creating
        # redirection URLs. In the context of virtual hosts, the ServerName
        # specifies what hostname must appear in the request's Host: header to
        # match this virtual host. For the default virtual host (this file) this
        # value is not decisive as it is used as a last resort host regardless.
        # However, you must set it for any further virtual host explicitly.
        #ServerName www.example.com

        ServerAdmin webmaster@localhost
        DocumentRoot /var/www/html/ads/public

        <Directory /var/www/html/ads/public/>
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

        ErrorLog ${APACHE_LOG_DIR}/error.log
        CustomLog ${APACHE_LOG_DIR}/access.log combined
</VirtualHost>
```


# 开启限速模块
文档 `http://httpd.apache.org/docs/2.4/mod/mod_ratelimit.html`
```shell
sudo a2enmod ratelimit

```text
<Directory /var/www/html/>
    SetOutputFilter RATE_LIMIT
    SetEnv rate-limit 400
</Directory>
```

# 重写规则
`.htaccess`
```text
<IfModule mod_rewrite.c>
    RewriteEngine On
    RewriteCond %{REQUEST_URI} ^/(server-info|server-http-status)
    RewriteRule . - [L]
    RewriteCond %{REQUEST_FILENAME} !-d
    RewriteCond %{REQUEST_FILENAME} !-f
    RewriteRule ^(.*)$ index.php?_url=/$1 [QSA,L]
</IfModule>
```

`mods-available/mpm_event.conf`

```conf
# event MPM
# StartServers: initial number of server processes to start
# MinSpareThreads: minimum number of worker threads which are kept spare
# MaxSpareThreads: maximum number of worker threads which are kept spare
# ThreadsPerChild: constant number of worker threads in each server process
# MaxRequestWorkers: maximum number of worker threads
# MaxConnectionsPerChild: maximum number of requests a server process serves
<IfModule mpm_event_module>
        StartServers             20     # 在启动后建立的子进程数
        MinSpareThreads          25     # 最小空闲进线程，当空闲子进程数小于25时，那么将会立刻生成新的子线程
        MaxSpareThreads          1200   # 最大空闲进线程，当空闲子进程数超过75时，那么父进程会杀死多余的子线程，必须要大于StartServers*ThreadsPerChild=20*50=1000
        ThreadLimit              64	# 每个进程可以生成最大线程数
        ThreadsPerChild          50     # 每个进程可以生成的线程数
        MaxRequestWorkers        1500   # 所能接受的总请求数，当请求超过1500时，多余的请求会进入请求队列
        MaxConnectionsPerChild   0      # 每个子进程所能接受的最大请求数
</IfModule>

<IfModule mpm_event_module>
        StartServers             20
        MinSpareThreads          25
        MaxSpareThreads          1200
        ThreadLimit              64
        ThreadsPerChild          50
        MaxRequestWorkers        1500
        MaxConnectionsPerChild   0
</IfModule>
```

`mods-available/mpm_prefork.conf`
```conf
# prefork MPM
# StartServers: number of server processes to start
# MinSpareServers: minimum number of server processes which are kept spare
# MaxSpareServers: maximum number of server processes which are kept spare
# MaxRequestWorkers: maximum number of server processes allowed to start
# MaxConnectionsPerChild: maximum number of requests a server process serves

<IfModule mpm_prefork_module>
        StartServers              5    # 默认启动的进程数
        MinSpareServers           5    # 最小空闲进程数
        MaxSpareServers           10   # 最大空闲进程数
        MaxRequestWorkers         150  # 最大工作进程数
        MaxConnectionsPerChild    0    # 最大请求数量
</IfModule>

<IfModule mpm_prefork_module>
	ServerLimit               1000
        StartServers              20
        MinSpareServers           10
        MaxSpareServers           60
        MaxRequestWorkers         1000
        MaxConnectionsPerChild    0
</IfModule>
```


* 开启状态服务

`mods-available/status.conf`

* 使用 PHP FPM

```conf
<FilesMatch \.php$>
         SetHandler "proxy:fcgi://127.0.0.1:9000"
</FilesMatch>
```

`/etc/apache2/mods-enabled/mpm_prefork.conf`
```conf
<IfModule mpm_prefork_module>
        ServerLimit 1000
        StartServers        10
        MinSpareServers     20
        MaxSpareServers     100
        MaxRequestWorkers   1000
        MaxConnectionsPerChild   0
</IfModule>
```

或者

```conf
<VirtualHost *:81 *:80>
        ServerAdmin webmaster@localhost
        ServerName app7
        DocumentRoot /var/www/html/app7/public
        <Proxy "unix:/run/php/php7.1-fpm.sock|fcgi://php-fpm">
            # we must declare a parameter in here (doesn't matter which) or it'll not register the proxy ahead of time
            ProxySet disablereuse=off
        </Proxy>

        # Redirect to the proxy
        <FilesMatch \.php$>
            SetHandler proxy:fcgi://php-fpm
        </FilesMatch>
        <Directory /var/www/html/app7/public/>
                Options Indexes FollowSymLinks
                AllowOverride all
                Order deny,allow
                Allow from all
        </Directory>
        ErrorLog ${APACHE_LOG_DIR}/error.log
        CustomLog ${APACHE_LOG_DIR}/access.log combined
</VirtualHost>

<VirtualHost *:81 *:80>
        ServerAdmin webmaster@localhost
        ServerName app
        DocumentRoot /var/www/html/app/public
        <FilesMatch \.php$>
                SetHandler "proxy:fcgi://127.0.0.1:9000"
        </FilesMatch>
        <Directory /var/www/html/app/public/>
                Options Indexes FollowSymLinks
                AllowOverride all
                Order deny,allow
                Allow from all
        </Directory>
        ErrorLog ${APACHE_LOG_DIR}/error.log
        CustomLog ${APACHE_LOG_DIR}/access.log combined
</VirtualHost>
```

# 传送 Authorization 头

```conf
<VirtualHost *:81 *:80>
	SetEnvIf Authorization "(.*)" HTTP_AUTHORIZATION=$1
</VirtualHost>
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

## 使用 alias 与 URL重写

```conf
Alias /test "/var/www/html/test/public"
<Directory /var/www/html/test/public">
          Options FollowSymlinks Indexes MultiViews
          AllowOverride All
          Order allow,deny
          Allow from all
	<IfModule mod_rewrite.c>
	    Options
	    RewriteEngine On
	    RewriteBase /test
	    RewriteCond %{REQUEST_FILENAME} !-d
	    RewriteCond %{REQUEST_FILENAME} !-f
	    RewriteRule ^(.*)$ index.php?_url=/$1 [QSA,L]
	</IfModule>
</Directory>
```
```conf
Alias /test "/var/www/html/test/public"
<Directory /var/www/html/test/public">
          Options Indexes MultiViews
          AllowOverride All
          Order allow,deny
          Allow from all
</Directory>

<IfModule mod_rewrite.c>
    Options +FollowSymlinks
    RewriteEngine On
    RewriteBase /test
    RewriteCond %{REQUEST_FILENAME} !-d
    RewriteCond %{REQUEST_FILENAME} !-f
    RewriteRule ^(.*)$ index.php?_url=/$1 [QSA,L]
</IfModule>
```

## 使用 Nginx 实现负载均衡

Nginx 配置

```conf
upstream myCluster{
	server 127.0.0.1:80;
	server 127.0.0.1:81;
}
server {
        listen 8181;
        server_name localhost;

        root /var/www/html/www/public;

        # Add index.php to the list if you are using PHP
        index index.php index.html;

        server_name _;

        location / {
                # First attempt to serve request as file, then
                # as directory, then fall back to displaying a 404.
                try_files $uri $uri/ /index.php?$query_string;
        }

        location ~ \.php$ {
		proxy_set_header Authorization $http_authorization;
		proxy_set_header HTTP_AUTHORIZATION $http_authorization;
		proxy_pass_header Authorization;
		proxy_pass http://myCluster$request_uri$query_string$is_args$args;
		proxy_redirect off;
		proxy_set_header Host $host;
		proxy_set_header X-Real-IP $remote_addr;
		proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
        }
}
```