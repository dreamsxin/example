# MooseFS

## MooseFS 架构

MooseFS 主要分为4种服务器，分别提供不同的服务。对应的关系如下：

- Managing server (master server)
是一个单独的服务器，用来管理整个文件系统。它存储了文件系统中每个文件的元数据（包括文件的大小，属性以及文件存储的位置等）。

- Data servers (chunk server)
由若干台机器组成的数据服务器，用来实际存储文件的数据。它们之间还可以互相同步数据（这是因为每个文件在文件系统中可能拥有多个 copy，copy 的数量可以自行配置，见下文）。

- Metadata backup server(s) (metalogger server)
由一台及以上机器组成，用来存储元数据的 `change logs`，并且周期性地从 Managing server 中下载最新的 `main metadata file`。当 Managing server 挂掉的时候，系统会从 `Metadata backup Servers` 中选择一台作为 `Managing server` 使用。

- Clients
使用 `MooseFS` 的客户机。通过 `mfsmount` 进程和 `Managing server` 进行通信执行各种文件操作。

## MooseFS 内部操作流程

`Metadata` 被存储在 `Managing server` 中的内存中，周期性地写回磁盘（并且更新日志）。同时这些内容会被同步到 metaloggers 中。每个文件被分割为若干个 `fragments(chunks)`，每个 `chunk` 是一个单独的文件，存储在 `data server` 上，单个大小最大为64MB。 通过为每个文件设置 “goal” 值来保证该文件的 copy 数量，以此保证文件系统的可靠性。

`Clients` 上的 mfsmount 进程在以下情况会直接和 managing server 通信： 
1. 新建/删除文件 
2. 读取目录内容 
3. 读取/修改属性（个人理解为修改文件系统属性）
4. 改变文件大小 
5. 读取/写入文件开始时 
6. 修改任何一个特殊文件的 METADATA 时 mfsmount 会直接 data server 通信。

当写入一个文件结束时，mfsmount 会通知 managing server 来修改该文件的元数据。同时，多个 `data server` 之间也会相互通信，来保证相同文件在不同机器上的同步。

这里使用 MooseFS 官网的两张图片说明执行文件操作时的内部执行流程：

![https://github.com/dreamsxin/example/blob/master/MooseFS/img/mfs01.png](https://github.com/dreamsxin/example/blob/master/MooseFS/img/mfs01.png?raw=true)

![https://github.com/dreamsxin/example/blob/master/MooseFS/img/mfs02.png](https://github.com/dreamsxin/example/blob/master/MooseFS/img/mfs02.png?raw=true)

## 安装与配置

Ubuntu:
```shell
# wget -O - http://ppa.moosefs.com/moosefs.key | apt-key add -
wget http://ppa.moosefs.com/stable/apt/moosefs.key
sudo apt-key add moosefs.key
# echo "deb http://ppa.moosefs.com/moosefs-3/apt/ubuntu/trusty trusty main" > /etc/apt/sources.list.d/moosefs.list
echo "deb http://ppa.moosefs.com/stable/apt/ubuntu/trusty trusty main" > /etc/apt/sources.list.d/moosefs.list
apt-get update
```

For Master Servers:
```shell
# apt-get install moosefs-master
# apt-get install moosefs-cli
sudo apt-get install moosefs-ce-master
```
配置：
```shell
cd /etc/mfs
cp mfsmaster.cfg.dist mfsmaster.cfg
cp mfsexports.cfg.dist mfsexports.cfg
```

修改`mfsexports.cfg`中第一行，该行表示访问权限，在最后添加`,password=pwdtext`，为文件系统添加密码。
修改`/etc/default/moosefs-ce-master`中的`MFSMASTER_ENABLE`为`true`，使得该服务开机启动。
通过service命令控制该服务的开启等操作：
```shell
sudo service moosefs-ce-master start
```

For Chunkservers:
```shell
# apt-get install moosefs-chunkserver
sudo apt-get install moosefs-ce-chunkserver
```
配置如同上一步：
```shell
cd /etc/mfs
cp mfschunkserver.cfg.dist mfschunkserver.cfg
cp mfshdd.cfg.dist mfshdd.cfg
```
在`mfshdd.cfg`中添加：
```text
/mnt/mfschunk1
/mnt/mfschunk2
```
然后手动创建上面的两个目录，然后修改用户 mfs 的权限，使其能够写入该分区：
```shell
chown -R mfs:mfs /mnt/mfschunks1
chown -R mfs:mfs /mnt/mfschunks2
```
最后修改

`/etc/default/moosefs-ce-chunkserver`中的`MFSCHUNKSERVER_ENBLE`为`true`保证开机启动，并通过service来控制当前运行情况：
```shell
sudo service moosefs-ce-chunkserver start
```

For Metaloggers:
```shell
apt-get install moosefs-metalogger
```

For Clients:
```shell
apt-get install moosefs-client
```
挂载：
```shell
mkdir -p /mnt/mfs
mfsmount /mnt/mfs -p -H mfsmaster-server
```

安装 MooseFS Client 之后，可以在 `/etc/fstab` 增加条目实现自动加载`MooseFS`：
```text
mfsmount /mnt/mfs fuse defaults 0 0
```

## 错误恢复

启动 `master server` 的时候提示找不到 `metadata.mfs` 文件，启动失败。这是因为非法关机导致主元数据文件（main metadata file）不完整，需要将元数据变更日志整合进该文件中。只需执行以下命令：

```shell
mfsmaster -a
```
然后重启服务即可。