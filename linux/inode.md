## 查询系统上的 inode 数量

```shell
sysctl -a | grep inode
```

## 查看文件 inode 号

```shell
ls -i #查看任意一个文件的 inode 编号
```

## 磁盘满

1、磁盘已满。可以用`df -TH`命令进行查看。

2、索引节点已满。可以用`df -i`命令进行查看。

* 索引节点已满的排查解决方案

1、 查看根路径下各个文件夹的文件数
```shell
for i in /*; do echo $i; find $i |wc -l|sort -nr; done
```

如发现var文件夹内容过多，可以cd进入var目录再执行，只需将对应路径进行修改即可。
```shell
cd var

for i in /var/*; do echo $i; find $i |wc -l|sort -nr; done
```

进入到对应目录中，删除对应文件夹下的文件，但执行rm -rf ./* 通常会报错，提示参数列表过长。原因是文件太多了。

换种方式进行删除
```shell
# 一定要先进入要删除的目录下
cd /var/cache/ 

# pwd命令可以显示当前路径
pwd

# 一定要先确认当前路径再执行删除操作
find . -name "*" | xargs rm -rf 
```

```shell
find / -xdev -printf '%h\n' | sort | uniq -c | sort -k 1 -n
```

```shell
lsof -i | grep deleted
```

## 通过 inode 删除

```shell
find ./* -inum 256129
find ./* -inum 256129 -delete
find ./* -inum 256129 -exec rm -i {} \;
```
