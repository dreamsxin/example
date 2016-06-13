# 安装httpry

在基于debian系统如Ubuntu，httpry没有包含在基础仓库中。

```shell
sudo apt-get install gcc make git libpcap0.8-dev
git clone https://github.com/jbittel/httpry.git
cd httpry
make
sudo make install
```

# httpry基本用法

```shell
sudo httpry -i eth0
```
httpry监听在指定的网卡下，实时捕获并显示HTTP请求与响应的包

在大多数情况下，输出滚动非常快的，需要保存捕获的HTTP数据包进行离线分析。可以使用-b或-o选项。“-b”选项将原始的HTTP数据包保存到一个二进制文件，然后可以用httpry进行重播。 “-o”选项保存可读的输出到文本文件。

保存到二进制文件中：

```shell
sudo httpry -i eth0 -b output.dump
```

重放：
```shell
httpry -r output.dump
```

保存到文本文件：
```shell
sudo httpry -i eth0 -o output.txt
```

# httpry高级用法

如果你要捕获特定的HTTP方法，如GET、POST、PUT、HEAD、CONNECT等等，可以使用‘-m'选项：
```shell
sudo httpry -i eth0 -m get,head
```

```shel;
sudo tcpdump  -XvvennSs 0 -i eth0 tcp[20:2]=0x4745 or tcp[20:2]=0x4854  or tcp[20:2]=0x504F and port 81
```