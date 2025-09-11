#

Linux系统中的strings命令是一个专门用于从二进制文件中提取可打印字符串的工具，常用于逆向分析、安全审计或调试场景‌。

- -t
显示偏移量
- -e
指定编码，指定字符编码（如s = 7位ASCII，S = 8位Latin1）
‌
## 提取二进制文件中的函数名、变量名或注释‌
```shell
strings -t x program | grep "key_"
```

```shell
strings libstdc++.so.6 |grep GLIBCXX
```

```shell
sudo add-apt-repository ppa:ubuntu-toolchain-r/test
sudo apt update
sudo apt install gcc-11 g++-11
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-11 110
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-11 110
```
