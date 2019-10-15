# CURL


## 查看返回头信息

```shell
curl -I http://localhost
```

## 指定情求头信息

```shell
curl -I -H "Accept-Language: es" http://localhost
```

让 nginx 返回 content-length 头信息
```shell
curl -v -H "Connection:keep-alive" http://localhost
```

## Post 数据

```shell
curl -I -d "name=test&password=test" http://localhost
```

## 传送二进制数据

```shell
curl -v --data-binary @xxx.bin  --header "application/octet-stream" http://localhost/
```

## 指定请求方法

```shell
curl -I -X DELETE http://localhost
```

输出所有发送到服务器的请求信息及返回信息

```shell
curl -v http://localhost
```

## 代理主机和端口

```shell
curl -x proxysever.test.com:3128 http://google.co.in
```

## 网络限速

下载速度最大不会超过1000B/second
```shell
curl --limit-rate 1000B -O http://www.gnu.org/software/gettext/manual/gettext.html
```

## 分段下载 range

```shell
curl -r 0-1024000 -o new_divide_1.mp3 http://localhost/test.mp3
curl -r 1024001-2048000 -o new_divide_2.mp3 http://localhost/test.mp3
curl -r 2048001- -o new_divide_3.mp3 http://localhost/test.mp3

cat new_divide_1.mp3 new_divide_2.mp3 new_divide_3.mp3 > test.mp3

```

当文件在下载完成之前结束该进程，通过添加`-C`选项继续对该文件进行下载，已经下载过的文件不会被重新下载
```shell
curl -C - -O http://www.gnu.org/software/gettext/manual/gettext.html
```

## 上传文件到FTP服务器

将myfile.txt文件上传到服务器
```shell
curl -u ftpuser:ftppass -T myfile.txt ftp://ftp.testserver.com
```
同时上传多个文件
```shell
curl -u ftpuser:ftppass -T "{file1,file2}" ftp://ftp.testserver.com
```
从标准输入获取内容保存到服务器指定的文件中
```shell
curl -u ftpuser:ftppass -T - ftp://ftp.testserver.com/myfile_1.txt
```

## curl命令测试网络请求中DNS解析、响应时间

经查遇到需要测量网络请求时间的问题，包括DNS解析、连接、传输等时间。Linux下的curl命令可以很好的测量网络请求。

如下
```shell
curl -o /dev/null -s -w %{time_connect}:%{time_starttransfer}:%{time_total} http://www.baidu.com
curl -o /dev/null -s -w %{time_connect}-Start:%{time_starttransfer}-Seep:%{speed_download}-Total:%{time_total}-DNS:%{time_namelookup} http://www.baidu.com
```


- -o 表示输出结果到 /dev/null
- -s 表示去除状态信息
- -w 表示列出后面的参数的结果

curl命令支持的参数，有如下
```text
time_connect        建立到服务器的 TCP 连接所用的时间
time_starttransfer  在发出请求之后,Web 服务器返回数据的第一个字节所用的时间
time_total          完成请求所用的时间
time_namelookup     DNS解析时间,从请求开始到DNS解析完毕所用时间(记得关掉 Linux 的 nscd 的服务测试)
speed_download      下载速度，单位-字节每秒。
```

通过该命令，可以计算网络请求中DNS解析、连接、传输及总的时间，进行初步的故障排查。