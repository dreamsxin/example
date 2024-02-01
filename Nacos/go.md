Go使用阿里nacos管理服务注册和发现，思路Demo

## 连接
```shell
func InitNacos() *nacosRF {
    //return &nacosRF{}
    d := &nacosRF{}
    clientConfig := constant.ClientConfig{
        TimeoutMs:           5000,
        NotLoadCacheAtStart: true,
        LogDir:              "./log",
        CacheDir:            "./cache",
        LogLevel:            "debug",
    }

    serverConfigs := []constant.ServerConfig{
        {
            IpAddr:      "127.0.0.1",
            ContextPath: "/nacos",
            Port:        8848,
            Scheme:      "http",
        },
    }

    var err error
    d.client, err = clients.NewNamingClient(
        vo.NacosClientParam{
            ClientConfig:  &clientConfig,
            ServerConfigs: serverConfigs,
        },
    )
    if err != nil {
        panic(err)
    }

    NacosInstance = d
    return d
}
```
