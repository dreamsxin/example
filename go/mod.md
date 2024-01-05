```shell
# 私有库
go env -w GOPRIVATE=xxx.com
# 配置库不走默认的GOPROXY
go env -w GONOPROXY=xxx.com
# 私有库 https 转 http
go env -w GOINSECURE=xxx.com
# 替换端口
git config --global url."http://xxx.com:9000/".insteadof "https://xxx.com/"
# 取消
git config --global --unset "http://xxx.com:9000/".insteadof
```

```config
# 这是git config --global -e编辑看到的效果
[url "ssh://git@xxx"]
    insteadOf = http://xxx
```
