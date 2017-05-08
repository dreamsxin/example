# 快捷键

## 打开终端

可以使用快捷键 `ctrl + alt + t` 打开一个新终端。
也可以按下 alt 然后输入 terminal

## 打开应用搜索菜单

按下 `supper` 键

## 最小化所有窗口

按下 `supper + ctrl + d` 键

## 打开新窗口

在 Nautilus 打开新的 Tab，浏览器中打开新的页面窗口。

`ctrl + t`

## 关闭当前窗口

在 Nautilus 关闭当前 Tab，浏览器中关闭当前页面窗口。

`ctrl + w`

## 返回 Home 目录

按下 `alt + home` 键

# 安装中文拼音

```shell
sudo apt-get install ibus-sunpinyin
```

## 重启 ibus

```shell
killall ibus-daemon
ibus-daemon -d
# or
ibus-daemon -d -x -r
```

## 设置快捷键 和 添加输入法

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

## 系统编码

```shell
apt-get install -y language-pack-en-base
```

`/etc/default/locale`
```ini
LANG="en_US.UTF-8"
LANGUAGE="en_US:en"
LC_ALL="en_US.UTF-8"
```

```shell
sudo locale-gen
# locale-gen zh_CN.UTF-8
sudo dpkg-reconfigure locales
```

## 遇到 Hash Sum mismatch 错误
```shell
sudo apt-get update --fix-missing
```

# syslog 日志

`/etc/rsyslog.conf`
`/etc/rsyslog.d/50-default.conf`

# 记录 crontab 日志

## 修改 rsyslog

`/etc/rsyslog.d/50-default.conf`

将 cron 前面的注释符去掉
```shell
cron.*                          /var/log/cron.log
```

## 任务

分时日月周
```shell
0 0 * * * /usr/bin/php /usr/share/nginx/www/apps/cli.php --task=report
```

## 重启 rsyslog

```shell
sudo service rsyslog restart
```