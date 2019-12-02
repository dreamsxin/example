

```shell
apt-get install ejabberd
# or
wget  http://www.process-one.net/downloads/ejabberd/2.1.13/ejabberd-2.1.13-linux-x86_64-installer.run
chmod +x ejabberd-2.1.13-linux-x86_64-installer.run
./ejabberd-2.1.13-linux-x86_64-installer.run
```

管理后台 
http://localhost:5280/admin

可以通过命令 ejabberdctl 注册用户，这个时候默认的域名是`localhost`，可以通过修改配置文件`/etc/ejabberd/ejabberd.cfg`来修改域名

```shell
ejabberdctl register admin localhost password
```

显示指定域名下所有注册用户

```shell
./ejabberdctl registered_users
```