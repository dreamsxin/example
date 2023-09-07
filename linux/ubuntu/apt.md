## E: Sub-process /usr/bin/dpkg returned an error code (1)

解决方法：

1.现将info文件夹更名

```shell
sudo mv /var/lib/dpkg/info /var/lib/dpkg/info.bk
```

2.新建一个新的info文件夹

```shell
sudo mkdir /var/lib/dpkg/info
```

3.安装修复

```shell
sudo apt-get update
sudo apt-get install -f
```

4.执行完上一步操作后，在info文件夹下生成一些文件，现将这些文件全部移到info.bk文件夹下

```shell
sudo mv /var/lib/dpkg/info/* /var/lib/dpkg/info.bk
```

5.把自己新建的info文件夹删掉

```shell
sudo rm -rf /var/lib/dpkg/info
```

6.恢复原有info文件夹，修改名字

```shell
sudo mv /var/lib/dpkg/info.bk /var/lib/dpkg/info
```
