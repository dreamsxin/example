# Vagrant 使用

```shell
sudo apt-get install vagrant 
```

## 常用命令

- vagrant init		# 初始化
- vagrant up		# 启动虚拟机
- vagrant halt		# 关闭虚拟机
- vagrant reload	# 重启虚拟机
- vagrant ssh		# SSH 至虚拟机
- vagrant status	# 查看虚拟机运行状态
- vagrant destroy	# 销毁当前虚拟机

## 安装

```shell
vagrant box add ub14 https://cloud-images.ubuntu.com/vagrant/trusty/current/trusty-server-cloudimg-amd64-vagrant-disk1.box
vagrant init ub14
vagrant up
```