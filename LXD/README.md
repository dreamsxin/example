# LXD

https://linuxcontainers.org/lxd/introduction/
https://github.com/lxc/lxd
https://github.com/lxc/lxc

LXD 属于 LXC 的上层管理工具，使用 Go 语言开发，拆分为 daemon和客户端两部分。

LXD 的定位很清晰：系统容器，直接对标虚拟机 ，甚至可以直接运行虚拟机镜像（但是不启动内核）。
系统容器运行整套操作系统（再说一次，除了内核），应用容器（如 docker）运行应用，两者不冲突。
可以在 LXD 容器里安装和使用 docker，跟在物理机和虚拟机上没什么两样。
LXD 还支持与 OpenStack 集成（nova-lxd 项目，可替代 OpenStack 上的虚拟机？）。

## 安装 LXD

`lxd-client` 是客户端软件包，安装后得到 `lxc` 命令。

```shell
sudo aptitude install lxd lxd-client -y
```

## 去掉默认网桥 lxdbr0

当前版本 `lxd` 默认会拉起 `lxdbr0` 网桥，可修改 `lxd.service` 不要依赖 `lxd-bridge.service`，避免拉起这个默认网桥：
```shell
sudo systemctl stop lxd-bridge.service
sudo rsync -ai /{lib,etc}/systemd/system/lxd.service
sudo sed -r -e '/^(After|Requires)=/ s#lxd-bridge.service\s*##g' /etc/systemd/system/lxd.service -i
sudo systemctl daemon-reload
```

另外 `/etc/default/lxd-bridge` 文件包含 lxd 网桥相关配置，可通过 `debconf-set-selections` 修改此配置文件。
```shell
echo 'lxd lxd/setup-bridge boolean false' | sudo debconf-set-selections
echo 'lxd lxd/use-existing-bridge boolean true' | sudo debconf-set-selections
echo 'lxd lxd/bridge-name string br0' | sudo debconf-set-selections
sudo dpkg-reconfigure -f noninteractive lxd
```

## 从源码编译安装新版本 LXD

为了使用新版 LXD 功能和特性，可从源码编译安装新版本 LXD。

* 安装开发依赖。

```shell
sudo apt-get install acl dnsmasq-base git liblxc1 lxc-dev libacl1-dev make pkg-config rsync squashfs-tools tar xz-utils -y
```

* 使用最新的 Go 编译源码

```shell
mkdir lxd.gopath
cd lxd.gopath/
cat > .envrc <<< $'export GOPATH="${PWD}"\nGOROOT="$(readlink -f /opt/go1.10)"\nPATH="${GOROOT}/bin:${GOPATH}/bin:$PATH"'
direnv allow .
git clone git@github.com:lxc/lxd.git src/github.com/lxc/lxd
( cd src/github.com/lxc/lxd/ && git tag --sort=version:refname | tail )
( cd src/github.com/lxc/lxd/ && git reset --hard lxd-3.0.0 )
make -C src/github.com/lxc/lxd/
```

## 安装到 local 目录：

```shell
sudo install bin/lxd bin/lxc -t /usr/local/bin/ -v
```

转移软件包 lxd 可执行文件，使用本地新版本替代：
```shell
sudo systemctl stop lxd
sudo dpkg-divert --rename --add /usr/bin/lxc
sudo dpkg-divert --rename --add /usr/bin/lxd
sudo ln -sfT /usr/{local/,}bin/lxc
sudo ln -sfT /usr/{local/,}bin/lxd
```

如果 `lxc` 命令被 hash 到系统路径，则需要解除 hash 以使用 local 下的新版 lxc 命令。
```shell
hash -d lxc
```

## 初始化 LXD daemon

LXD daemon, 有时也称作服务器（server）。
LXD 相关操作通常通过客户端 lxc 命令执行，但 lxd 命令也包含一些特殊操作。

```shell
lxd init
```
相当于一个特殊的 lxc 命令。其主要有两个操作，配置网络和存储。

* 配置网络

lxd 与 git 类似，采用分布式的架构，任意两个节点都可以相互通信。配置网络地址将其暴露到网络上，其他节点可使用密码连接。
```shell
lxd init --auto --network-address=0.0.0.0 --trust-password=1234
```
也可以直接使用 lxc 命令修改和查看网络地址（及其他 server 配置）：
```shell
lxc config set core.https_address 0.0.0.0:8443
lxc config show 
```

注意：修改配置后立即生效（类似自动 reload？），而不只是修改配置数据，操作非常方便。

* 配置存储

LXD 内置支持多种存储后端，如 dir, btrfs, lvm, zfs, ceph 等，推荐使用 zfs 和 btrfs。

提供一个简便的命令，创建一个名为 default 的（"默认"）存储，并配置（名为 default 的）默认 profile 使用此存储创建容器。
创建一个名为 lxd 的 lvm vg，使用此 vg 创建 "默认" 存储。

```shell
lxd init --auto --storage-backend=lvm --storage-pool=lxd
```
当然也可以直接使用 lxc 命令查看和执行相关操作：

```shell
lxc storage list
lxc profile show default
```

```text
$ lxd init --help
Description:
  Configure the LXD daemon

Usage:
  lxd init [flags]

Examples:
  init --preseed
  init --auto [--network-address=IP] [--network-port=8443] [--storage-backend=dir]
              [--storage-create-device=DEVICE] [--storage-create-loop=SIZE]
              [--storage-pool=POOL] [--trust-password=PASSWORD]


Flags:
      --auto                    Automatic (non-interactive) mode
      --network-address         Address to bind LXD to (default: none)
      --network-port            Port to bind LXD to (default: 8443) (default -1)
      --preseed                 Pre-seed mode, expects YAML config from stdin
      --storage-backend         Storage backend to use (btrfs, dir, lvm or zfs, default: dir)
      --storage-create-device   Setup device based storage using DEVICE
      --storage-create-loop     Setup loop based storage with SIZE in GB (default -1)
      --storage-pool            Storage pool to use or create
      --trust-password          Password required to add new clients

Global Flags:
# ... ...
```

## LXD remote

任意两个节点都可以互相通信，查看 remote 节点。

```shell
lxc remote list
```
lxd 默认添加了 ubuntu 等 remote 节点。本地节点被当作一个名为 local 的特殊节点，同时也是默认节点。
```text
+-----------------+------------------------------------------+---------------+-----------+--------+--------+
|      NAME       |                   URL                    |   PROTOCOL    | AUTH TYPE | PUBLIC | STATIC |
+-----------------+------------------------------------------+---------------+-----------+--------+--------+
| images          | https://images.linuxcontainers.org       | simplestreams |           | YES    | NO     |
+-----------------+------------------------------------------+---------------+-----------+--------+--------+
| local (default) | unix://                                  | lxd           | tls       | NO     | YES    |
+-----------------+------------------------------------------+---------------+-----------+--------+--------+
| ubuntu          | https://cloud-images.ubuntu.com/releases | simplestreams |           | YES    | YES    |
+-----------------+------------------------------------------+---------------+-----------+--------+--------+
| ubuntu-daily    | https://cloud-images.ubuntu.com/daily    | simplestreams |           | YES    | YES    |
+-----------------+------------------------------------------+---------------+-----------+--------+--------+
```

* 添加远程节点
```shell
lxc remote add han2017 https://han2017:8443/ --accept-certificate --password=1234
```

## LXD 镜像管理

```shell
lxc image list
```

```txt
$ lxc image list
+--------------+--------------+--------+---------------------------------------------+--------+----------+-----------------------------+
|    ALIAS     | FINGERPRINT  | PUBLIC |                 DESCRIPTION                 |  ARCH  |   SIZE   |         UPLOAD DATE         |
+--------------+--------------+--------+---------------------------------------------+--------+----------+-----------------------------+
| ubuntu/16.04 | be7cec7c9489 | yes    | ubuntu 16.04 LTS amd64 (release) (20180405) | x86_64 | 156.27MB | Apr 7, 2018 at 8:17am (UTC) |
+--------------+--------------+--------+---------------------------------------------+--------+----------+-----------------------------+

$ sudo tree -L 3 -a /var/lib/lxd/images/
/var/lib/lxd/images/
├── be7cec7c948958adfbb9bc7dbd292762d2388cc883466815fc2b6bc06bf06f5a
└── be7cec7c948958adfbb9bc7dbd292762d2388cc883466815fc2b6bc06bf06f5a.rootfs
```

* 拷贝镜像

LXD 创建容器时，会自动拷贝镜像到本地 cache（私有镜像），并自动管理（自动更新，过期清理）。
也可以显式手动从远程节点拷贝镜像到本地：
```shell
lxc image copy ubuntu:16.04 local: --alias ubuntu/16.04 --public
lxc image copy han2017:ubuntu/16.04 local: --copy-aliases --public
```

镜像默认以文件形式保持在 /var/lib/lxd/images/ 目录下。
镜像使用 id 标识，可以添加简单可读的别名 ALIAS。拷贝时可以指定目标别名，或者拷贝复用源节点上的别名。

## LXD 容器配置

可划分为两部分：

- key/value 配置，使用 lxc config 或 lxc profile 管理。
- 对标虚拟机概念，设备管理，使用 lxc config device 或 lxc profile device 管理。如磁盘设备，网络设备等。

注意，容器内磁盘设备通常是使用文件系统，而不是块设备。

`lxc config` 管理已创建容器实例配置，`lxc profile` 管理容器 `profile` 配置。
为了方便管理容器配置，LXD 支持使用 `profile` 预设管理容器配置模板。
创建容器时可指定多个 `profile`，多个 `profile` 与容器自身配置叠加覆盖得到最终有效配置。

## 使用客户端命令创建 LXD 容器

* 查看已有容器

```shell
lxc list
```
* 显示存储卷
```shell
lxc storage volume list default
```
* 显示镜像卷、容器卷和存储卷
```shell
lxc storage volume list default
```

lxc init 命令：
```output
$ lxc init --help
Description:
  Create containers from images

Usage:
  lxc init [<remote>:]<image> [<remote>:][<name>] [flags]

Examples:
  lxc init ubuntu:16.04 u1

Flags:
  -c, --config      Config key/value to apply to the new container
  -e, --ephemeral   Ephemeral container
  -n, --network     Network name
  -p, --profile     Profile to apply to the new container
  -s, --storage     Storage pool name
      --target      Node name
  -t, --type        Instance type

$ lxc network list
+--------+----------+---------+-------------+---------+
|  NAME  |   TYPE   | MANAGED | DESCRIPTION | USED BY |
+--------+----------+---------+-------------+---------+
| br0    | bridge   | NO      |             | 0       |
+--------+----------+---------+-------------+---------+
| enp5s0 | physical | NO      |             | 0       |
+--------+----------+---------+-------------+---------+
```

- -p 指定 profile, 可重复多次以指定多个 profile 叠加覆盖。不指定时默认使用 default。
- -c 指定容器 key/value 配置。
- -s 使用指定存储池创建容器卷。容器卷未指定大小时默认与镜像大小相同。
- -n 创建网卡设备连接到指定网络。可指定外部（手动管理的）网桥（或网卡等网络设备？）名。

* 创建特权容器。

LXD 非特权容器默认开启了用户 idmap，虽然 LXD 对 idmap 做了很好的支持，但 idmap 解决的问题比带来的问题更多，如与宿主机共享文件系统问题等。

修改默认 profile：
```shell
lxc profile set default security.privileged true
```

* 创建容器 test
```shell
lxc init ubuntu/16.04 test -s default -n br0
```

* 启动容器
```shell
lxc start test
```

* 查看网桥

```shell
brctl show
```
```output
bridge name    bridge id            STP enabled    interfaces    tap0
br0            8000.0227625084a8    yes            enp5s0        vethKFYJSY
```
```shell
ip link show dev vethKFYJSY
```

* 登录容器

```shell
lxc exec -t test /bin/bash
```
在宿主机上查看 bash 进程

```shell
ps -C bash -fww f
```
* 在容器内重新分配伪终端
输入 `tty`，输出 `not a tty`
```shell
script -c /bin/bash /dev/null
```
可以在 lxc exec 时直接执行 script 命令：
```shell
lxc exec -t test -- script -c /bin/bash /dev/null
```

## 访问宿主机文件系统，LXD 支持动态添加路径绑定，操作立即生效，非常方便。

新建 profile 方便管理相关配置：
```shell
lxc profile create share-host
lxc profile set share-host security.privileged true
lxc profile device add share-host /etc/apt/ disk {source,path}=/etc/apt/
```
查看
```shell
lxc profile show share-host
```
```output
config:
  security.privileged: "true"
description: ""
devices:
  /etc/apt/:
    path: /etc/apt/
    source: /etc/apt/
    type: disk
name: share-host
used_by: []
```

```shell
lxc profile add test share-host
# 查看文件
lxc exec -t test findmnt /etc/apt/

lxc profile device add share-host /var/cache/apt/ disk {source,path}=/var/cache/apt/
Device /var/cache/apt/ added to share-host
lxc exec -t test findmnt /var/cache/apt/
```

## 将容器发布为镜像

```shell
lxc publish test --alias test --public
lxc image list
```

* 使用新镜像创建容器：

```shell
lxc init test test2 -p share-host -p default -s default -n br0
```
