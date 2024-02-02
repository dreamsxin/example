首先在各节点启动时将ACL启用，在配置文件夹目录中(这里目录名是config)增加acl.hcl文件(每个节点都需要加)，内容如下：
```conf
acl = {
  enabled = true
  default_policy = "deny"
  enable_token_persistence = true 
}
```
参数说明：

- enabled=true 代表开启ACL；
- default_policy=“deny”默认为allow，如果需要自定义权限，需要将其设置为deny；
- enable_token_persistence =true开启token持久化，将token持久化到磁盘上；
这里需要注意一点，之前说配置目录下的Json文件会被自动加载，其实还有hcl文件也会被自动加载，这里用hcl的形式演示一下。 配置文件准备好之后，重新启动节点即可(集群中的所有节点都需要用上)

如果是在配置文件中配置，输入配置的token即可，如果没有配置，可以在运行时生成一个bootstrap token，在任意一个Server中执行consul acl bootstrap命令获得该bootstrap token；Consul中token都很重要，需要保存好。

bootstrap token权限很大，不可能每个小伙伴都拥有，就像MySQL的root权限一样，只能有个别的人知道。其他用户的权限需单独控制；Consul也是如此，针对不同权限策略，生成对应的token，使用这个token就只能访问或操作对应权限范围内的资源。

## UI方式配置
ACL的配置其他token可以通过命令的形式，也可以通过UI界面的形式(因为现在有bootstrap token超级权限)，这里通过UI的形式很方便的，三步走：

### 创建策略

策略其他信息基本上没啥说的，主要是规则(Rules)的配置，通常主要针对节点(node)、服务(service)、键值对(K/V)进行配置，可以模糊指定，也可以具体指定，如下：

node_prefix ""：节点前缀为空，代表所有的节点都使用策略；

service_prefix ""：服务前缀为空，代表所有的服务都使用策略；

service "Code6688Name"：指定对应的服务使用策略；

key_prefix "redis/"：只对前缀有"redis/"的key使用对应策略；

key "dashboard-app"：指定对应的key使用策略；

以上指定策略的范围是比较常用的方式，具体可以参照官网；

规则中关于策略(policy)通常有以下几种：

read：只能查询；

write：可查可写；

deny：不能读不能写；

