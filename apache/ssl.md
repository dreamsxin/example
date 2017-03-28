# CentOS 配置双向证书

```shell
/sbin/iptables -I INPUT -p tcp --dport 443 -j ACCEPT
yum install httpd
yum install mod_ssl openssl
```

修改配置文件 `/etc/httpd/conf.d/ssl.conf`
```conf
<VirtualHost _default_:443>
DocumentRoot "/var/www/https"
ServerName *:443
DirectoryIndex index.html

SSLEngine on
SSLCertificateFile /etc/pki/tls/server.crt    //证书
SSLCertificateKeyFile /etc/pki/tls/server.key  //私钥

</VirtualHost>
```

```shell
service httpd restart
```
