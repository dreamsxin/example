# sendmail

* 安装

```shell
sudo apt-get install sendmail  
sudo apt-get install sendmail-cf
sudo apt-get install mailutils
```

```shell
ps aux |grep sendmail
```

* 配置

`/etc/mail/sendmail.mc`

```conf
DAEMON_OPTIONS(`Family=inet,  Name=MTA-v4, Port=smtp, Addr=127.0.0.1')dnl
```
修改`Addr=0.0.0.0` ，表明可以连接到任何服务器。

```shell
cd /etc/mail  
mv sendmail.cf sendmail.cf.bak
m4 sendmail.mc > sendmail.cf
```

```php
<?php
$to = 'dreamsxin@qq.com';
$subject = 'the subject';
$message = 'hello';
mail($to, $subject, $message);
```

`/etc/mail/sendmail.cf: line 100: fileclass: cannot open '/etc/mail/local-host-names': No such file or directory`

```shell
sudo touch /etc/mail/local-host-names
sudo chmod a+rwx /etc/mail/local-host-names
```

* 发邮件慢

`My unqualified host name`

修改 `/etc/hosts` 追加本机域名

`/var/spool/mqueue-client/`

```shell
sudo chmod a+rwx /var/spool/mqueue/
sudo chown root:smmsp /var/spool/mqueue-client/
```

安装 `sendmail` 失败卡死问题解决方案


```shell
sudo dpkg --configure -a
Setting up sendmail-bin (8.15.2-3) ...
Updating sendmail environment ...
Reading configuration from /etc/mail/sendmail.conf.
Validating configuration.
Writing configuration to /etc/mail/sendmail.conf.
Writing /etc/cron.d/sendmail.
Disabling HOST statistics file(/var/lib/sendmail/host_status).
Reading configuration from /etc/mail/sendmail.conf.
Validating configuration.
Writing configuration to /etc/mail/sendmail.conf.
Writing /etc/cron.d/sendmail.
Could not open /etc/mail/databases(No such file or directory), creating it.
Reading configuration from /etc/mail/sendmail.conf.
Validating configuration.
Creating /etc/mail/databases...

Checking filesystem, this may take some time - it will not hang!
  ...   Done.

Checking for installed MDAs...
sasl2-bin not installed, not configuring sendmail support.

To enable sendmail SASL2 support at a later date, invoke "/usr/share/sendmail/update_auth"


Creating/Updating SSL(for TLS) information
Creating /etc/mail/tls/starttls.m4...
Creating SSL certificates for sendmail.
```

卡在最后这一行，还有个进度条停在80%不动了。
解决方案：
```shell
cd /etc/mail/tls
sudo openssl dsaparam -out sendmail-common.prm 2048
sudo chown root:smmsp sendmail-common.prm
sudo chmod 0640 sendmail-common.prm
sudo dpkg --configure -a
```