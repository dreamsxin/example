## 安装nfs
```shell
yum -y install nfs-utils
service nfs restart

#创建nfs目录
mkdir -p /nfs/data/

#修改权限
chmod -R 777 /nfs/data
```
## 配置目录
`/etc/exports`
```
/nfs/data *(rw,no_root_squash,sync)  # * 代表所有人都能连接，建议换成具体ip或ip段，如192.168.20.0/24）
/var/www/html  192.168.2.0/24(ro,all_squash,sync)
# nfs4 挂载目录下所有子目录
/nfsdata 192.168.1.101(rw,fsid=0,sync,no_wdelay,insecure_locks,no_root_squash)
```
刷新
```shell
exportfs -rv
```

```shell
mount  x.x.x.x:/var/www/html /usr/local/ydhnginx/html/
```
nfs 自己并没有去对外监听某个端口号，而是外包给了rpc服务，rpc帮助nfs去监听端口，然后告诉客户机和本机的那个进程对应的端口连续
```shell
# -a或--all 显示所有连线中的Socket。
# -n或--numeric 直接使用IP地址，而不通过域名服务器。
# -l或--listening 显示监控中的服务器的Socket。
# -u或--udp 显示UDP传输协议的连线状况。
# -t或--tcp 显示TCP传输协议的连线状况。
# -p或--programs 显示正在使用Socket的程序识别码和程序名称。
#netstat -anp
netstat -anplut|grep rpc
```
