# 流量监控

## 安装
```shell
sudo apt-get install vnstat vnstati
```

## 初始化
```shell
sudo vnstat -u -i eth1
```

## 启动
```shell
sudo /etc/init.d/vnstat start
```

## 自动开机
```shell
sudo update-rc.d vnstat enable
```

## 按小时统计流量
```shell
vnstat -i eth1 -h
```
## 按天统计流量
```shell
vnstat -i eth1 -d
```

## 改默认监控网卡
```shell
sudo vi /etc/vnstat.conf
```