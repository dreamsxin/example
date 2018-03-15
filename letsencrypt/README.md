# Let's Encrypt

```shell
git clone https://github.com/certbot/certbot
cd certbot
./certbot-auto --help all
./certbot-auto certonly --standalone --email dreamsxin@qq.com -d myleft.org -d *.myleft.org
```

在完成Let's Encrypt证书的生成之后，我们会在"/etc/letsencrypt/live/laozuo.org/"域名目录下有4个文件就是生成的密钥证书文件。

- cert.pem  - Apache服务器端证书
- chain.pem  - Apache根证书和中继证书
- fullchain.pem  - Nginx所需要ssl_certificate文件
- privkey.pem - 安全证书KEY文件

我们从生成的文件中可以看到，Let's Encrypt证书是有效期90天的，可以制作成定时执行任务，比如每个月执行一次。
