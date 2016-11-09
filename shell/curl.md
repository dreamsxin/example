查看返回头信息

```shell
curl -I http://localhost
```

指定情求头信息

```shell
curl -I -H "Accept-Language: es" http://localhost
```

Post 数据

```shell
curl -I -d "name=test&password=test" http://localhost
```

传送二进制数据

```shell
curl -v --data-binary @xxx.bin  --header "application/octet-stream" http://localhost/
```

指定请求方法

```shell
curl -I -X DELETE http://localhost
```

输出所有发送到服务器的请求信息及返回信息

```shell
curl -v http://localhost
```