```shell
# 私有库
go env -w GOPRIVATE=xxx.com
# 配置库不走默认的GOPROXY
go env -w GONOPROXY=xxx.com
# 私有库 https 转 http
go env -w GOINSECURE=xxx.com

git config http.sslVerify false
# 替换端口
git config url."http://xxx.com:9000/".insteadof "https://xxx.com/"
git config url."http://xxx.com:9000/".insteadof "http://xxx.com/"
git config --global url."http://xxx.com:9000/".insteadof "git://xxx.com/"
# 取消
git config --unset "http://xxx.com:9000/".insteadof
```

```config
# 这是git config --global -e编辑看到的效果
[url "ssh://git@xxx"]
    insteadOf = http://xxx
```
## 查看拉取详细信息
```shell
go get -v xxx
```

## 清理缓存
```shell
go clean -r github.com/dreamsxin/gota
go clean --modcache
```
