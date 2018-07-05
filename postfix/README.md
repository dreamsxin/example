# Postfix

Postfix 是一个顶尖的SMTP服务器，也被称为MTA。Postfix负责从其他SMTP服务器接收邮件，将MUA递交的邮件传递给收件人。MUA(mail user agent)就是我们平常使用的Thunderbird，Foxmail，Outlook等邮箱客户端。Postfix程序采用模块化的设计，每一个模块都尽可能以最低的权限运行。这种设计非常有助于安全。Postfix与Unix的结合度非常高。一台Linux服务器的Postfix可以负责多个域名的邮件发送和接收。这篇文章只介绍单个域名的发送和接收。

## 设置正确的主机名

```shell
hostname -f
sudo hostnamectl set-hostname <mail.yourdomain.com>
```

## 设置好系统时间

```shell
date
sudo dpkg-reconfigure tzdata
# or
sudo ln -sf /usr/share/zoneinfo/your-geographic-area/your-city /etc/localtime
```

## MX 记录

MX记录的作用是告诉全世界的SMTP服务器，你的邮箱服务器mail.yourdomain.com负责yourdomain.com的邮件发送和接收。

## A 记录

当其他SMTP服务器知道mail.yourdomain.com负责yourdomain.com的邮件发送和接收后，还必须要知道mail.yourdomain.com这台主机的IP才能建立连接并发送邮件。A记录就是将主机名解析成IP地址的一个记录。

## PTR 记录

PTR 记录也叫做 `pointer` 记录，它将IP地址转换成主机名，与A记录刚好相反。这种解析被称为反向DNS解析（rDNS）。

PTR 记录可以帮助我们过滤垃圾邮件。很多SMTP服务器会查找对方SMTP服务器的PTR记录，得到一个主机名，然后与对方SMTP声称的主机名作比较，如果两者一致，就接收邮件，反之不接收邮件或放进垃圾箱。为了不让你的邮件被拒收或放进垃圾箱，你应该为你的服务器IP设置PTR记录。

查找一个IP地址的PTR记录的命令为：

`dig -x <IP> +short` 或者 `host <IP>`

## 安装

```shell
sudo apt-get install postfix
# 统一验证服务
sudo apt-get install courier-authdaemon
sudo apt-get install sqwebmail
```

安装过程中会让你选择一种Postfix配置类型。一般情况下，我们需要选择第二种类型：`Internet Site`。

- No configuration
表示不要做任何配置；
- Internet Site
表示直接使用本地SMTP服务器发送和接收邮件；
- Internet with smarthost
表示使用本地SMTP服务器接收邮件，但发送邮件时不直接使用本地SMTP服务器，而是使用第三方smart host来转发邮件；
- Satellite system
表示邮件的发送和接收都是由第三方smarthost来完成。
- Local only
表示邮件只能在本机用户之间发送和接收。

在第二个页面`System mail name`中填入你的域名，也就是邮箱地址@符号后面的域名。当发件人的域名地址没有指定时，Postfix会自动将这个域名添加到发件人的地址中。

Postfix在安装过程中会生成`/etc/postfix/main.cf`配置文件。安装完成后`Postfix`会自动运行。通过`pstree`我们查看进程关系。
用下面的命令查看`Postfix`的版本：
```shell
sudo postconf mail_version
```

使用`netstat`来查看监听情况：
```shell
sudo netstat -lnpt
```

在发送测试邮件之前，我们最好是查看25号端口是否被防火墙或主机商屏蔽。nmap可以帮助我们扫描服务器的开放端口：
```shell
sudo nmap <your-server-ip>
```

## 发送测试邮件

如果你的服务器有一个用户名是user1，那么这个用户的邮箱地址就是`user1@yourdomain.com`。不过现在我们只能在服务器上用命令行发送和查看邮件。

Postfix 在安装时，会同时安装一个`sendmail`的程序（/usr/sbin/sendmail）。
发送一封测试邮件（使用的是当前登录用户）：
```shell
echo "test email" | sendmail your-account@gmail.com
```

每个用户的邮件保存在`/var/spool/mail<username>`和`/var/mail/<username>`文件中。如果你不知道收件箱保存在哪里，运行这条命令：
```shell
postconf mail_spool_directory
```
Postfix的收发日志保存在`/var/log/mail.log`文件中。Postfix本身的运行错误日志保存在`/var/log/mail.err`文件中。

## 使用 mail 程序来发送邮件，查看收件箱

`sendmail` 的功能非常有限，现在让我们来安装一个命令行邮箱客户端。
```shell
sudo apt-get install mailutils
```
使用mail发送邮件的命令为
```shell
mail username@gmail.com
```
```text
Cc: 
Subject: 2nd test email
I'm sending this email using the mail program.
```
输入主题和正文后，按Ctrl+D来发送邮件。

要查看收件箱，输入`mail`就行了。
以下是用mail管理收件箱的操作方法。

- 要查看第一封邮件，输入数字1。如果邮件只显示了一半，按Enter键来显示剩下的消息。
- 将所有邮件从第一封排序，输入h。
- 要显示最后一屏邮件，输入h$或z。
- 阅读下一封邮件，输入n。
- 删除第一封邮件，输入d 1。
- 删除第一封，第二封和第四封邮件，输入d 1 2 4。
- 删除前10封邮件，输入d 1-10。
- 回复第1封邮件，输入reply 1。
- 退出mail程序，输入q或x。

如果你按`q`来退出mail程序，那么已经阅读过的邮件将会从/var/mail/<username>移动到/home/<username>/mbox文件中。
这意味着其他邮箱客户端将不能阅读这些邮件。如果你不想移动已经阅读的邮件，输入`x`退出mail程序。

如果需要自动转发邮件，那么在用户的home目录下新建一个.forward文件，在这个文件里输入转发邮件地址，然后保存就行了。注意：用户邮箱不会保留原始邮件。

## 配置

```conf
protocols = pop3 imap
listen = *
default_login_user=postfix
default_internal_user=postfix
# 使用postfix 用户
disable_plaintext_auth = no
log_path = /var/log/dovecot.log
info_log_path = /var/log/dovecot.info
log_timestamp = "%Y-%m-%d %H:%M:%S "
ssl = no
mail_location = maildir:/var/vmail/%d/%u
# 邮件存储路径，使用postfix用户的uid
mail_privileged_group = mail
first_valid_uid = 89
protocol pop3 {
	pop3_uidl_format = %08Xu%08Xv
}
auth_mechanisms = plain login

passdb {
	driver=sql
	args = /etc/dovecot/dovecot-mysql.conf
}
userdb {
	driver=sql
	args = /etc/dovecot/dovecot-mysql.conf
}
# 用于SMTP验证
service auth {
	unix_listener /var/spool/postfix/private/auth {
		group = postfix
		user = postfix
		mode = 0660
	}
}
```

`dovecot-mysql.conf`
```conf
driver = mysql
connect = host=localhost dbname=postfix user=postfix password=postfix123456
# 适合明文密码
# default_pass_scheme = PLAIN
# 适合数据库 md5 函数加密的密码
# default_pass_scheme = MD5
# 适合 postfixadmin 默认加密方式 md5-crypt
default_pass_scheme = MD5-CRYPT
password_query = SELECT password FROM mailbox WHERE username = '%u'
# 使用 postfix 这个系统用户的uid gid替换 89
user_query = SELECT maildir, 89 AS uid, 89 AS gid FROM mailbox WHERE username = '%u'
```

## 权限问题

查看目录用户 uid 和 gid 是否一致
```shell
grep "postfix" /etc/passwd
```

不一致，修改目录所属用户和用户组：
```shell
chown -R postfix:postfix /var/vmail
```

```error
2017-01-18 19:17:23 imap(dreamsxin@qq.com): Error: user dreamsxin@qq.com: Initialization failed: Initializing mail storage from mail_location setting failed: mkdir(/var/vmail/qq.com/dreamsxin@qq.com) failed: Permission denied (euid=89(postfix) egid=89(postfix) missing +w perm: /var/vmail/qq.com, euid is not dir owner)
```

如果仍然报同样错误，检查 SELinux 模式：
```shell
# 方法1：设置 SELinux 成为 permissive 模式
setenforce 0
# or
# 查看目录的安全上下文
ls -Z
# 设置安全上下文
chcon -R -t mail_spool_t /var/vmail
# 允许 postfix 写入邮件池
getsebool-a |grep postfix # 查看所有选项 sestatus -b
setsebool -P allow_postfix_local_write_mail_spool on
```

# 测试 IMAP

```shell
telnet 127.0.0.1
Trying 127.0.0.1...
Connected to 127.0.0.1.
Escape character is '^]'.
* OK [CAPABILITY IMAP4rev1 LITERAL+ SASL-IR LOGIN-REFERRALS ID ENABLE IDLE AUTH=PLAIN AUTH=LOGIN] Dovecot ready.
1 login dreamsxin 123
1 OK [CAPABILITY IMAP4rev1 LITERAL+ SASL-IR LOGIN-REFERRALS ID ENABLE IDLE SORT SORT=DISPLAY THREAD=REFERENCES THREAD=REFS MULTIAPPEND UNSELECT CHILDREN NAMESPACE UIDPLUS LIST-EXTENDED I18NLEVEL=1 CONDSTORE QRESYNC ESEARCH ESORT SEARCHRES WITHIN CONTEXT=SEARCH LIST-STATUS] Logged in
```
