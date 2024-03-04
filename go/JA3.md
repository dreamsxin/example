- https://github.com/open-ch/ja3
- https://github.com/dreadl0ck/ja3
- https://github.com/gospider007/gospider/blob/master/ja3/ja3.go
```go
server := &http.Server{
    ConnContext: ja3.ConnContext, // 上下文传递
    TLSConfig: &tls.Config{
        GetConfigForClient: ja3.GetConfigForClient, // 关键
    },
}

handle.GET("/", func(ctx *gin.Context) {
    ja3ContextData := ja3.GetRequestJa3Data(ctx.Request)
    ja3Name, ja3Ok := ja3ContextData.Verify()//返回设备名称,是否是安全流量
    ja3Md5:=ja3ContextData.Md5()//返回ja3的md5 值
    var msg string
    if ja3Ok {
        msg = "安全的访问"
    } else {
        msg = "不安全的访问,请使用浏览器访问,这次请求将被拦截"
    }
    ctx.JSON(200, map[string]any{
        "msg": msg,
        "id":  ja3Md5,
        "设备":  ja3Name,
    })
})
```
