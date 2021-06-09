# OpenSMTPD

https://wiki.archlinux.org/title/OpenSMTPD#Configuration

opensmtpd 是一个轻量的 SMTP 实现，同时提供 sendmail 程序。

## 设置 DNS

通过 sendmail 发邮件，而不需要收邮件，因此配置比较简单。

在 DNS 设置 TXT SFP 记录 `v=spf1 ip4:xxx.xxx.xxx.xxx ip6:xxxx::xxxx -all`

```txt
主机记录	记录类型	记录值
@	TXT	v=spf1 include:spf.xxx.com -all
spf	TXT	v=spf1 ip4:xxx.xxx.xxx.xxx –all
```

## 设置 `FQDN`

最后，有的服务器要求发送方给出一个合法的域名作为主机名 (domain.ltd 形式)，因此需要在 /etc/smtpd/mailname 加入域名:

`echo xxxx.com > /etc/smtpd/mailname`

## 完整主机名
```shell
hostname -f
```
输出 `mail.xxxx.com`

修改 `/etc/hostname` 文件。主机名只能包含数字，字母和连字符：

`mail`
