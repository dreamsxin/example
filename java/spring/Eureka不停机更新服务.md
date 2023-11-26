# 不停机更新服务

假设一个应用部署两个实例，在更新应用时应该按照以下步骤进行：

## 第一步：修改服务实例状态为 DOWN
方案1.直接调用 Eureka Server API 修改：
```shell
PUT /eureka/apps/{appID}/{instanceID}/status?value=DOWN
```
方案2.调用服务实例对应的 actuator endpoint
```shell
curl -H "Content-Type:application/json" -X POST -u {username}:{password} http://{host:port}/actuator/service-registry?status=DOWN
```

## 第二步：等待其他服务缓存刷新

具体要等多久，看客户端配置
- `eureka.client.registryFetchIntervalSeconds `
- `ribbon.ServerListRefreshInterval`
注册中心配置
- `eureka.server.responseCacheUpdateIntervalMs`
