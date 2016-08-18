# use gcc 5.x on ubuntu 14.04 

```shell
sudo add-apt-repository ppa:ubuntu-toolchain-r/test
sudo apt-get update
sudo apt-get install gcc-5 g++-5

sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-5 60 --slave /usr/bin/g++ g++ /usr/bin/g++-5
```

# denyhosts 使用

重要配置信息
```ini
SECURE_LOG = /var/log/auth.log
HOSTS_DENY = /etc/hosts.deny
```

## 启动与停止
```shell
sudo service denyhosts start
sudo service denyhosts stop
```

## 清除日志
```shell
sudo rm /var/log/auth.log
```

## 警告: 磁盘描述表明物理块大小为 2048 字节，但 Linux 认为它是 512 字节。  (udisks-error-quark, 0)

```shell
sudo dd if=/dev/zero of=/dev/sdb bs=2048 count=32
```