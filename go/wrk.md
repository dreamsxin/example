# wrk

```shell
wrk -d 60 -c 10000  -t 32 'http://www.baidu.com'
```

## go-wrk
```shell
go install github.com/tsliwowicz/go-wrk@latest
./go-wrk -c 80 -d 5  http://localhost:8080/ping
```
