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

