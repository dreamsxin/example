# 配置

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
