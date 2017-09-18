## 安装

```shell
sudo apt-get install unison
sudo apt-get install inotify-tools
```

## 配置双机信任(ssh key)

在从服务器上执行：

```shell
ssh-keygen -t rsa
```

使用OpenSSH的ssh-copy-id工具将公钥写入主服务器上的`~/.ssh/authorized_keys`：

```shell
ssh-copy-id -i ~/.ssh/id_rsa.pub root@master
```

在主服务器上也执行上诉操作。

```shell
ssh-keygen -t rsa
ssh-copy-id -i ~/.ssh/id_rsa.pub root@slave
```

然后可以测试登录，是否需要输入密码。

## 创建同步脚本

 
从服务器脚本`unison-slave.sh`：

```shell
#!/bin/bash

export HOME=/root

echo $HOME

ip="root@master"

src="/var/www/html/"
dst="/var/www/html/"

/usr/bin/inotifywait -mrq -e create,delete,modify,move $src | while read line; do
	unison -batch -sshargs "-i /root/.ssh/id_rsa" $src ssh://$ip/$dst
	echo -n "$line " >> /var/log/inotify.log
	echo `date | cut -d " " -f1-4` >> /var/log/inotify.log
done
```

在`/etc/rc.local`文件末尾（exit 0）之前添加如下代码：

`/home/root/unisonsync.sh &`

执行脚本测试，如果出现错误`/bin/bash^M: bad interpreter: No such file or directory`

可能的原因是脚本文件是DOS格式的, 即每一行的行尾以\r\n来标识, 使用vim编辑器打开脚本, 运行:
```shell
:set ff?
```
可以看到DOS或UNIX的字样. 使用`set ff=unix`把它强制为unix格式的, 然后存盘退出, 即可。

# 手动单向同步

```shell
unison -force="/var/www/html" -batch -sshargs "-i /root/.ssh/id_rsa" "/var/www/html/\" "ssh://slave//var/www/html"
```