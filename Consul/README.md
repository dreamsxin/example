#

## 安装
https://www.hashicorp.com/official-packaging-guide
```shell
# Download the signing key to a new keyring
sudo apt update && sudo apt install gpg
# Verify the key's fingerprint
wget -O- https://apt.releases.hashicorp.com/gpg | sudo gpg --dearmor -o /usr/share/keyrings/hashicorp-archive-keyring.gpg
# Add the HashiCorp repo
echo "deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/hashicorp-archive-keyring.gpg] https://apt.releases.hashicorp.com $(lsb_release -cs) main" | sudo tee /etc/apt/sources.list.d/hashicorp.list
# see all available packages
grep ^Package: /var/lib/apt/lists/apt.releases.hashicorp.com*Packages | sort -u

sudo apt install consul
```

## consul 常用命令
### consul agent
- -bind=0.0.0.0 指定 consul所在机器的 IP地址。 默认值：0.0.0.0
- -http-port=8500 consul 自带一个web访问的默认端口：8500
- -client=127.0.0.1 表明哪些机器可以访问consul 。 默认本机。0.0.0.0 所有机器均可访问。
- -config-dir=foo 所有主动注册服务的 描述信息
- -data-dir=path 储存所有注册过来的srv机器的详细信息。
- -dev 开发者模式，直接以默认配置启动 consul
- -node=hostname 节点在集群中的名称，在一个集群中必须是唯一的，默认是该节点的主机名。
- -rejoin 使consul忽略先前的离开，在再次启动后仍旧尝试加入集群中。
- -server 以服务方式开启consul， 允许其他的consul 连接到开启的 consul上 （形成集群）。如果不加 -server， 表示以 “客户端” 的方式开启。不能被连接。
- -ui 可以使用 web 页面 来查看服务发现的详情

### consul members
查看集群中有多少个成员。
### consul info
查看当前 consul 的 IP 信息。
### consul leave
优雅的关闭 consul 。—— 不优雅 -> Ctrl -c

```shell
consul agent -dev -ui -client 0.0.0.0
```

管理界面 http://localhost:8500/
