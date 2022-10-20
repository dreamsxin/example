```shell
##SSH代理命令依然是 ProxyCommand ，使用方法有所不同:
ssh <target_server> -o "ProxyCommand ssh -W %h:%p <jump_server>"
##举例:
ssh 192.168.6.253 -o "ProxyCommand ssh -W %h:%p 10.10.1.200"
ssh 192.168.6.253 -o "ProxyCommand ssh -i xxx.key 10.10.1.200"
```

* ProxyJump
```shell
scp -o "ProxyJump <User>@<Proxy-Server>" <File-Name> <User>@<Destination-Server>:<Destination-Path>
##ProxyJump 指令还支持多跳(神奇):
ssh -J jumpuser1@jumphost1,jumpuser2@jumphost2,...,jumpuserN@jumphostN user@host
```
