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