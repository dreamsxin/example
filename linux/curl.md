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