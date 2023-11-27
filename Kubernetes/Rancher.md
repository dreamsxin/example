# Rancher

Rancher 是一个 Kubernetes 管理工具，让你能在任何地方和任何提供商上部署和运行集群。

## 环境

临时禁用或关闭正在运行的swap分区。
```shell
sudo swapoff -a # sudo swapon -a
sudo swapon -s
free -m
```

```shell
sudo yum install ntp
sudo systemctl enable ntpd
sudo systemctl start ntpd

# sudo apt install chrony

timedatectl set-timezone Asia/Shanghai # 切换时区
# tzselect
```

## 确保虚机不会自动 suspend
```shell
sudo systemctl mask sleep.target suspend.target hibernate.target hybrid-sleep.target
```
```shell
sudo modprobe br_netfilter # br_netfilter 模块可以使 iptables 规则可以在 Linux Bridges 上面工作
lsmod | grep br_netfilter 
```

将桥接的IPv4流量传递到iptables的链 `/etc/sysctl.conf`
```conf
cat > /etc/sysctl.conf << EFO
net.ipv4.ip_forward = 1
net.bridge.bridge-nf-call-ip6tables = 1
net.bridge.bridge-nf-call-iptables = 1
EFO
```
`/etc/sysctl.d/10-network-security.conf`
```conf
net.ipv4.conf.default.rp_filter=1 
net.ipv4.conf.all.rp_filter=1
```
```shell
sudo sysctl --system
```
## 启动 Rancher

```shell
#  --privileged 超级权限
sudo docker run -d --restart=unless-stopped --privileged --name rancher -p 20080:80 -p 443:443 rancher/rancher
# 查看密码
sudo docker logs container-id 2>&1 | grep "Bootstrap Password:"
```
