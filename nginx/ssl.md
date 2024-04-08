## 生成自签名证书:

```shell
cd /var/www
mkdir ssl
cd ssl
sudo openssl genrsa -des3 -out server.key 1024
sudo openssl req -new -key server.key -out server.csr
sudo openssl rsa -in server.key -out server_nopwd.key  
sudo openssl x509 -req -days 365 -in server.csr -signkey server_nopwd.key -out server.crt  
```
其中证书的生成过程大致如下：

```text
~$ sudo openssl req -new -key server.key -out server.csr  
Enter pass phrase for server.key:   ←输入第一步中生成server.key时设置的密码，比如 1234
You are about to be asked to enter information that will be incorporated  
into your certificate request.  
What you are about to enter is what is called a Distinguished Name or a DN.  
There are quite a few fields but you can leave some blank  
For some fields there will be a default value,  
If you enter '.', the field will be left blank.  
-----  
Country Name (2 letter code) [AU]:CN　←输入国家代码  
State or Province Name (full name) [Some-State]:　← 输入省名  
Locality Name (eg, city) []:　←输入城市名  
Organization Name (eg, company) [Internet Widgits Pty Ltd]:　← 输入公司名  
Organizational Unit Name (eg, section) []:　← 输入组织单位名  
Common Name (eg, YOUR name) []:　← 输入主机名（域名） 
Email Address []:　←输入电子邮箱地址  
```

```shell
openssl genrsa -des3 -out ssl.key 4096
# 消除密码
openssl rsa -in ssl.key -out ssl2.key
openssl req -new -key ssl2.key -out ssl2.csr
openssl x509 -req -days 365 -in ssl2.csr -signkey ssl2.key -out ssl2.crt
```

## 配置

```shell
sudo vim /etc/nginx/sites-available/default  
```

```conf
server {  
	listen 443 ssl default_server；
	listen [::]:443 ssl default_server;

	ssl on;  
	ssl_certificate /var/www/ssl/server.crt;  
	ssl_certificate_key /var/www/ssl/server_nopwd.key; 
}  
```

## 优化

例子：
```conf
ssl_protocols TLSv1 TLSv1.1 TLSv1.2; # Dropping SSLv3, ref: POODLE
ssl_prefer_server_ciphers off;
ssl_ciphers ALL:!kEDH!ADH:RC4+RSA:+HIGH:+MEDIUM:+LOW:+SSLv2:+EXP;
ssl_session_cache shared:SSL:20m;
ssl_session_timeout 10m;
```

参数解析：
```conf
listen 443 ssl http2;
#使用HTTP/2，需要Nginx1.9.7以上版本

add_header Strict-Transport-Security "max-age=6307200; includeSubdomains; preload";
#开启HSTS，并设置有效期为“6307200秒”（6个月），包括子域名(根据情况可删掉)，预加载到浏览器缓存(根据情况可删掉)

add_header X-Frame-Options DENY;
#禁止被嵌入框架

add_header X-Content-Type-Options nosniff;
#防止在IE9、Chrome和Safari中的MIME类型混淆攻击

ssl_certificate /usr/local/nginx/conf/vhost/sslkey/www.linpx.com.crt;
ssl_certificate_key /usr/local/nginx/conf/vhost/sslkey/www.linpx.com.key;
#SSL证书文件位置

ssl_trusted_certificate /usr/local/nginx/conf/vhost/sslkey/chaine.pem;
#OCSP Stapling的证书位置

ssl_dhparam /usr/local/nginx/conf/vhost/sslkey/dhparam.pem;
#DH-Key交换密钥文件位置

#SSL优化配置

ssl_protocols TLSv1 TLSv1.1 TLSv1.2;
#只允许TLS协议

ssl_ciphers EECDH+CHACHA20:EECDH+CHACHA20-draft:EECDH+AES128:RSA+AES128:EECDH+AES256:RSA+AES256:EECDH+3DES:RSA+3DES:!MD5;
#加密套件,这里用了CloudFlare's Internet facing SSL cipher configuration

ssl_prefer_server_ciphers on;
#由服务器协商最佳的加密算法

ssl_session_cache builtin:1000 shared:SSL:10m;
#Session Cache，将Session缓存到服务器，这可能会占用更多的服务器资源

ssl_session_tickets on;
#开启浏览器的Session Ticket缓存

ssl_session_timeout 10m; 
#SSL session过期时间

ssl_stapling on; 
#OCSP Stapling开启,OCSP是用于在线查询证书吊销情况的服务，使用OCSP Stapling能将证书有效状态的信息缓存到服务器，提高TLS握手速度

ssl_stapling_verify on;
#OCSP Stapling验证开启

resolver 8.8.8.8 8.8.4.4 valid=300s;
#用于查询OCSP服务器的DNS

resolver_timeout 5s;
#查询域名超时时间
```
