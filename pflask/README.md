# pflask

https://github.com/ghedo/pflask

pflask是一个在Linux上创建进程容器的简单工具。 它可用于运行单个命令，甚至可以在隔离环境中启动整个操作系统，其中文件系统层次结构，网络，进程树，IPC子系统和主机/域名可以与主机系统和其他容器隔离。

## 编译和安装

```shell
cd ghedo/pflask/
./bootstrap.py
./waf configure
./waf build
./waf install
```

## 查看版本

```shell
pflask -V
```

## 创建新容器并启动 bash shell
```shell
sudo pflask
# or 启动 shell 时候运行 id
sudo pflask -- id
```

## 指定根文件系统

容器也可以通过使用 `--chroot` 选项运行 `inside private root` 目录：
```shell
sudo pflask --chroot=/path/to/rootfs -- id
```

甚至可以调用init二进制并将整个操作系统 inside 引导到容器中：
```shell
sudo pflask --chroot=/path/to/rootfs --/sbin/init
```
## 创建根文件系统 rootfs

注意pflask不提供任何创建rootfs的支持，但是可以使用现有的工具。
例如 debootstrap 命令可以用于创建如下所示的 Debian rootfs:
```shell
sudo debootstrap sid/path/to/rootfs http://httpredir.debian.org/debian
```

## 网络

将容器的网络与主机系统断开连接，所有网络接口将不可以用于容器：
```shell
sudo pflask --netif -- ip link
```

创建 private 网络接口：
```shell
sudo pflask --netif=macvlan:eth0:net0 -- ip link
```
命令 上面 将从 eth0 主机接口创建一个名为 net0的新 macvlan 接口。 macvlan 接口可以用于向网络适配器提供额外的MAC地址，并使它的看起来像完全不同的设备。

## 文件系统

使用 `--ephemeral` 选项，只要容器终止，pflask 会放弃所有应用到 root 文件系统的任何更改：
```shell
sudo pflask --chroot=/path/to/rootfs --ephemeral --/sbin/init
```

默认情况下，容器将创建新的挂载命名空间。
使用 `--mount` 选项可以创建新的挂载点，实现容器与主机的文件交换。

```shell
sudo pflask --chroot=/path/to/rootfs --mount=bind:/tmp:/tmp
```

命令 上面 会绑定主机目录 /tmp 到容器的/tmp 中。