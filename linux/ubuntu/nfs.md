# NFS服务器

```shell
sudo apt-get install nfs-kernel-server
```
## 客户端

```shell
sudo apt-get install nfs-common
sudo mount -t nfs 192.168.1.108:/home/ubuntu/nfs /usr/share/nginx/www/upload
```

## 开机自动挂载

- 第一种方法：`/etc/fstab`里添加  `server_IP:/remote_dir   /local_dir   nfs  defaults  1  1`

- 第二种方法：将手动挂载命令加入到`/etc/rc.local`中.

## HA 集群
- NFS + Keepalived
  实现高可用，防止单点故障。
- Rsync + Inotify
  实现主备间共享数据进行同步
