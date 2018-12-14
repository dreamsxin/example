
```shell
ab -n1000 -c100 http://localhost/
```

- -n	代表请求数
- -c	代表并发数

* 自定义头

```shell
ab -n 1000 -c 100 -H 'content-type: application/json' -H 'Authorization: xxx' -p data.json 'http://localhost/api'
```