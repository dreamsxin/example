
```shell
sudo groupmod -g <新GID> <组名>  # 修改组ID
sudo groupadd <组名>             # 添加新组
sudo groupdel <组名>             # 删除组
# 切换到新组
newgrp <新组名>

sudo systemctl restart gdm

sudo chown -R <用户名>:<新组名> /path/to/directory  # 更改文件/目录的所有者和组
sudo chmod -R <权限> /path/to/directory            # 更改文件/目录的权限
```
