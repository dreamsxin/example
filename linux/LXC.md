# LXC

https://github.com/lxc/lxc
https://github.com/lxc/lxd

## 安装

```shell
sudo apt-get install lxc
sudo lxc-checkconfig
```

## 创建

内置模板存放目录 `/usr/share/lxc/templates`

```shell
tree /usr/share/lxc/templates
```
```output
/usr/share/lxc/templates
├── lxc-alpine
├── lxc-altlinux
├── lxc-archlinux
├── lxc-busybox
├── lxc-centos
├── lxc-cirros
├── lxc-debian
├── lxc-download
├── lxc-fedora
├── lxc-gentoo
├── lxc-openmandriva
├── lxc-opensuse
├── lxc-oracle
├── lxc-plamo
├── lxc-slackware
├── lxc-sparclinux
├── lxc-sshd
├── lxc-ubuntu
└── lxc-ubuntu-cloud

0 directories, 19 files
```

缓存目录 `/var/cache/lxc`

```shell
# lxc-create -n 容器名 -t 模板名
sudo lxc-create -n ubuntu-lxc-vm -t ubuntu
```
>> 完成后会给出初始用户名和密码

生成目录 `/var/lib/lxc/ubuntu-lxc-vm`

* 查看创建好的容器状态（包括IP）

```shell
sudo lxc-ls -f
```

## 设定资源

文件 `/var/lib/lxc/ubuntu-lxc-vm/config`
```shell
sudo lxc-cgroup -n ubuntu-lxc-vm memroy.limit_in_bytes 256M
```

* 单核设置 CPU 占用周期最大值 1024

```shell
sudo lxc-cgroup -n ubuntu-lxc-vm cpu.shares 512
```

* 多核指定 CPU

```shell
sudo lxc-cgroup -n ubuntu-lxc-vm cpuset.cpus 0,3
```

示例：
```conf
lxc.cgroup.memory.limit_in_bytes = 512M               # 限定内存
lxc.cgroup.cpuset.cpus = 0                            # 限定可以使用的核
lxc.cgroup.blkio.throttle.read_bps_device = 8:0 100   # 读取速率限定
lxc.cgroup.blkio.throttle.write_bps_device = 8:0 100  # 写入速率限定
lxc.cgroup.blkio.throttle.read_iops_device = 8:0 100  # 读取频率限定
lxc.cgroup.blkio.throttle.write_iops_device = 8:0 100 # 写入频率限定
```
## 设置网络

网络管理
配置Linux虚拟网桥为桥接模式
`ifcfg-eth0`
```text
DEVICE="eth0"
BOOTPROTO="static"
HWADDR=""
NM_CONTROLLED="yes"
ONBOOT="yes"
TYPE="Ethernet"
IPADDR=""
NETMASK=""
GATEWAY=""
DNS1=""
BRIDGE="br0"                #把eth0添加到br0
```
创建配置文件`ifcfg-br0`,并重启网络

```text
DEVICE="br0"                    #设备名字，必须和BRIDGE=“”里的相同
TYPE="Bridge"                   #指定类型为网桥
NOBOOT="yes"
BOOTPROTO="static"
IPADDR=""                       #网桥的管理地址
NETMASK="255.255.255.0"
GATEWAY=""
DNS1=""
DELAY="0"                    #监控流量里的mac地址
```

查看所有网桥

```shell
brctl  show
```
```text
bridge name    bridge id        STP enabled    interfaces
br0        8000.5404a6a7ff6c    no        eth0                          
```

通过命令配置,临时生效。

```shell
brctl addbr br0
brctl addif br0 eth1
ifconfig  br0 172.16.200.33/16 up
ip add del dev eth1 172.16.252.71/16
```

* lxc 的 web 管理

监听端口    ：tcp/5000
默认用户名  ：admin
密码        ：admin
```shell
git clone https://github.com/lxc-webpanel/LXC-Web-Panel.git
cd LXC-Web-Panel
python lwp.py    
```

`/etc/network/interfaces` 增加：
```text
auto br0
iface br0 inet dhcp
bridge_ports eth0
```
启用 br0
``
sudo ifup br0
```
修改 `/var/lib/lxc/ubuntu-lxc-vm/config` 
`lxc.network.link = lxcbr0` 改为 `lxc.network.link = br0`

* 重启网络
```shell
sudo service lxc-net restart
```

配置示例：
```conf
lxc.network.type=phys
lxc.network.link=eth0
lxc.network.flags=up
lxc.utsname = cjhlxc1
#lxc.network.hwaddr = 00:16:3e:f9:ad:be #此行要注释掉#
lxc.devttydir = lxc
lxc.tty = 4
lxc.pts = 1024
lxc.rootfs = /var/lib/lxc/myt2/rootfs
lxc.mount  = /var/lib/lxc/myt2/fstab
lxc.arch = amd64
lxc.cap.drop = sys_module mac_admin
lxc.pivotdir = lxc_putold
```

- lxc.network.type 指定用于容器的网络类型，包括四种类型：

	- empty：建立一个没有使用的网络接口
	- veth：容器将链接到 lxc.network.link 定义的网桥上，从而与外部通信。之前网桥必须在预先建立。
	- macvlan：一个 macvlan 的接口会链接到 lxc.network.link 上。
	- phys： lxc.network.link 指定的物理接口分配给容器。

- lxc.network.flags 用于指定网络的状态，up 表示网络处于可用状态。

- lxc.network.link 用于指定用于和容器接口通信的真实接口，比如一个网桥 br0 。

- lxc.network.hwaddr 用于指定容器的网络接口的 mac 地址。

## 端口映射

```shell
sudo iptables -t nat -A PREROUTING -d 192.168.1.117 -p tcp --dport 10001 -j DNAT --to-destination 10.0.3.155:22
```

## 启动

```shell
sudo lxc-start -n ubuntu-lxc-vm
```

## 打开对应容器的控制台：

```shell
sudo lxc-console -n ubuntu-lxc-vm
```
也可以通过 SSH 登录
```shell
sudo ssh ubuntu@xxx.xxx.xxx.xx
```
获取使用 lxc-attach

```shell
sudo lxc-attach -n ubuntu-lxc-vm
```

然后就可以在容器里安装你喜欢的软件了。

## 重置密码

```shell
chroot /var/lib/lxc/ubuntu-lxc-vm/rootfs
passwd root #修改root初始密码
```

## 停止

```shell
sudo lxc-stop -n ubuntu-lxc-vm
sudo lxc-shutdown -n ubuntu-lxc-vm
```

## 复制

```shell
sudo lxc-clone -o ubuntu-lxc-vm -n ubuntu-lxc-vm2
```

## lxc-kill

发送信号给容器中的第一个用户进程（容器内部进程号为2的进程）。
```shell
## SIGNUM 参数可选，默认SIGKILL
lxc-kil -n ubuntu-lxc-vm SIGNUM
```

## lxc-info

获取一个容器的状态

```shell
lxc-info -n ubuntu-lxc-vm
```

## lxc-ls
列出当前系统所有的容器

## ~/.bashrc

```shell
echo "The Status Of the Machine:"
INFO=$(sudo lxc-ls -f)
echo "$INFO"
echo "The Table For Choosing Sys-Container:"
echo "1 --------- Ubuntu16.04+TensorFlow1.0"
echo "2 --------- Debian10.42+Caffe1.2"
echo "3 --------- Centos17.22+TensorFlow1.2"
echo "Please Choose The Lable Of The System:"
read input
if [ "$input" -eq 1 ]; then
        echo "Ubuntu is Starting,Please waitting..."
        sudo lxc-start -n Ubuntu
        echo "Have a Good Day!"
        sudo ssh ubuntu@10.0.3.155
elif [ "$input" -eq 2 ]; then
        echo "Debian is Starting,Please waitting..."
        sudo lxc-start -n Debian
        echo "Have a Good Day!"
        sudo ssh Debian@10.0.3.239
elif [ "$input" -eq 3 ]; then
        echo "Centos is Starting,Please waitting..."
        sudo lxc-start -n Centos
        echo "Have a Good Day!"
        sudo ssh Centos@10.2.0.134
else
        echo "Thanks For Choosing the Host Machine,Enjoying~"
fi
```

## 使用非 root 用户启动容器

调整 `/etc/lxc/lxc-usernet` 设置普通用户
```text
# USERNAME TYPE BRIDGE COUNT
ubuntu veth lxcbr0 2
```

获取 ubuntu 这个用户的 subuid 和 subgid（在 /etc/subuid 和 /etc/subgid）。

* 切换到 ubuntu 用户

在 ubuntu 的用户目录下建立 `.config/lxc` 目录，然後，复制 `/etc/lxc/default.conf` 
```shell
mkdir -p ~/.config/lxc
cp /etc/lxc/default.conf ~/.config/lxc
 ```
 追加
 ```text
lxc.id_map = u 0 100000 65536
lxc.id_map = g 0 100000 65536
```

然后就可以使用 ubuntu 用户创建启动容器，数据会存在用户目录下 `.local/share/lxc`。

## 冻结/解冻LXC容器

可以通过以下命令冻结LXC容器
```shell
sudo lxc-freeze --name=NAME
```
这条命令会冻结LXC容器的所有进程。这些进程会阻塞直到你使用lxc-unfreeze解冻
```shell
sudo lxc-unfreeze --name=NAME
```

## 删除 LXC容器

```text
$ lxc-destroy --help
Usage: lxc-destroy --name=NAME [-f] [-P lxcpath]

选项 :
  -n, --name=NAME   容器的名称
  -f, --force       等待容器关闭
```

## lxc-snapshot 
创建，列出和还原容器快照。