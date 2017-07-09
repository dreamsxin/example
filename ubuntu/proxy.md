# 设置代理

## 全局代理

在“首选项”->“网络代理”那里，多了个“System-wide”按钮，在这里设置后，`apt-get`确实可以使用代理了。

所设置修改了两个文件，
一个是`/etc/environment`，这个是系统的环境变量，里面定义了`http_proxy`等代理环境变量。
另一个是“/etc/apt/apt.conf”，这个就是apt的配置，内容如下

```conf
Acquire::http::proxy "http://127.0.0.1:8000/";
Acquire::ftp::proxy "ftp://127.0.0.1:8000/";
Acquire::https::proxy "https://127.0.0.1:8000/";
```

## apt-get 代理

apt-get 可以用`-c`选项来指定使用配置文件
```shell
sudo apt-get -c /etc/apt/apt.conf update
```

apt-get 还有有一个`-o`选项，直接设置变量：
```shell
sudo apt-get -o Acquire::http::proxy="http://127.0.0.1:8000/" update
sudo apt-get -o Acquire::socks::proxy="socks5://127.0.0.1:1080" update;
```