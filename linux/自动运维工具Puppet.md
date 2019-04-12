# Puppet

Puppet 的目录是让管理员只集中于要管理的目标，而忽略实现的细节。Puppet 即可以运行在单机上，也可以以C/S结构使用。
在大规模使用puppet的情况下，通常使用C/S结构，在这种结构中 puppet 客户端只运行 puppeclient，puppet服务端只运行 puppemaster。

## Facter

Facter 是 Puppet 跨平台的系统性能分析库。它能发现并报告每个节点的信息，在Puppet代码中是以变量的形式出现的。它返回的是每个Agent的fact信息，这些信息包括主机名、IP地址、操作系统、内存大小及其他的系统配置选项，这些fact信息在Puppet Agent运行的时候进行收集并传递给Master，同时被自动创建为可以被Puppet使用的变量

## 工作过程中的注意事项

1) 为了保证安全，Client和Master之间是基于SSL和证书的，只有经Master证书认证的Client可以与Master通信
2) Puppet会让系统保持在人们所期望的某种状态并一直维持下去，如检测某个文件并保证其一直存在，保证SSH服务始终开启，如果文件被删除了或者ssh服务关闭了，puppet下次执行时（默认30分钟）会重新创建该文件或者启动SSH服务

## 搭建 puppetmaster

1) 规划服务器主机名

在上千台服务器的环境中，我们要搭建自己的DNS服务器来实现服务通过主机名来进行通信。
在小的规模puppet环境下，一般修改/etc/hosts文件，作用相当如DNS，提供 IP 地址到 `hostname` 的对应。

修改主机名 `/etc/sysconfig/network`
```text
HOSTNAME=xxx.xxx
```
修改对应IP `/etc/hosts`
立即生效
```shell
hostname xxx.xxx
bash
```

2) 时间同步服务器

由于 facter 使用SSL证书，依赖时间同步，所以需要搭建NTP服务器。

```shell
sudo apt-get install ntp
#or
sudo yum -y install ntp
```

`/etc/ntp.conf`
```conf
server 127.0.0.1           # local clock
fudge 127.0.0.1 startum 8  # stratum 设置为其它值也是可以的，其范围为0~15
```

```shell
chkconfig ntpd on
```

## 安装 ruby