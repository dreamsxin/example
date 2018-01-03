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
State or Province Name (full name) [Some-State]:CHONGQING　← 输入省名  
Locality Name (eg, city) []:CHONGQING　←输入城市名  
Organization Name (eg, company) [Internet Widgits Pty Ltd]:MIKE　← 输入公司名  
Organizational Unit Name (eg, section) []:MIKE　← 输入组织单位名  
Common Name (eg, YOUR name) []:www.mike.me　← 输入主机名  
Email Address []:easylife206@gmail.com　←输入电子邮箱地址  
← 回车
← 回车
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
