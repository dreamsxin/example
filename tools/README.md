# 代码统计工具

## cloc

```shell
sudo apt-get install cloc
cd cphalcon/ext
cloc .
```

## sloccount

会估计出总成本。

```shell
sudo apt-get install sloccount
sloccount .
```

## wc

统计当前目录下的所有文件行数：

```shell
wc -l *
```

当前目录以及子目录的所有文件行数：

```shell
find  . * | xargs wc -l
```

## 压力测试

Siege

命令常用参数

-c 200 指定并发数200
-r 5 指定测试的次数5
-f urls.txt 制定url的文件
-i internet系统，随机发送url
-b 请求无需等待 delay=0
-t 5 持续测试5分钟

```shell
sudo apt-get install siege
siege -c 200 -r 100 http://www.google.com
```