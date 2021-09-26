# Nginx 负载均衡

Nginx 可以根据客户端IP进行负载均衡，在 upstream 里设置ip_hash，就可以针对同一个C类地址段中的客户端选择同一个后端服务器，除非那个后端服务器宕了才会换一个。

目前支持的 5 种方式的分配：

* 轮询（默认）
每个请求按时间顺序逐一分配到不同的后端服务器，如果后端服务器down掉，能自动剔除。
```conf
upstream backserver {
	server 192.168.0.14;
	server 192.168.0.15;
}
```

* 指定权重
指定轮询几率，weight和访问比率成正比，用于后端服务器性能不均的情况。
```conf
upstream backserver {
	server 192.168.0.14 weight=10;
	server 192.168.0.15 weight=10;
}
```

* IP绑定 ip_hash
每个请求按访问ip的hash结果分配，这样每个访客固定访问一个后端服务器，可以解决session的问题。
```conf
upstream backserver {
	ip_hash;
	server 192.168.0.14:88;
	server 192.168.0.15:80;
}
```

* fair（第三方）
按后端服务器的响应时间来分配请求，响应时间短的优先分配。
```conf
upstream backserver {
	server server1;
	server server2;
	fair;
}
```

* url_hash（第三方）
按访问url的hash结果来分配请求，使每个url定向到同一个后端服务器，后端服务器为缓存时比较有效。
```conf
upstream backserver {
	server squid1:3128;
	server squid2:3128;
	hash $request_uri;
	hash_method crc32;
}
```

实例：
```conf
upstream backserver{
	ip_hash;
	server 127.0.0.1:9090 down;		# down 表示单前的server暂时不参与负载
	server 127.0.0.1:8080 weight=2;	# weight 默认为1.weight越大，负载的权重就越大
	server 127.0.0.1:6060;
	server 127.0.0.1:7070 backup;	# 其它所有的非backup机器down或者忙的时候，请求backup机器
}
```
- max_fails
允许请求失败的次数默认为 1，当超过最大次数时，返回proxy_next_upstream 模块定义的错误。

- fail_timeout
max_fails 次失败后，暂停的时间。

## 自动注册

方案一：etcd + confd + nginx

方案二：etcd/consul + nginx-upsync-module

方案三：ZooKeeper + dyups  + nginx-upstream-reloader

方案四：使用Consul + Consul-template实现
https://github.com/maxwellyue/gitbook-way-to-architect

方案五：nacos-nginx-template 

### confd

https://github.com/kelseyhightower/confd

它可以将配置信息存储在etcd、consul、dynamodb、redis以及zookeeper等。
confd定期会从这些存储节点pull最新的配置，然后重新加载服务，完成配置文件的更新。


```shell
mkdir -p /root/go/src/github.com/kelseyhightower
git clone https://github.com/kelseyhightower/confd.git /root/go/src/github.com/kelseyhightower/confd
cd /root/go/src/github.com/kelseyhightower/confd
make
mv bin/confd /usr/local/bin
```

* 配置文件
```conf
# cat conf.d/www.imlcs.com.toml

[template]
src = "www.imlcs.com.tmpl"
dest = "/etc/nginx/conf.d/www.imlcs.com.conf"
keys = [
    "/nginx/www",
    "/domain/www",
]
check_cmd = "nginx -t"
reload_cmd = "nginx -s reload"
```
* 参数说明：
- src：模板文件名
- dest：生成目标文件的文件名及位置
- keys：监控的keys，是一个数组，用到哪个key加入里面就行
- check_cmd：检查命令，可以不用加
- relaod_cmd：重新加载进程的命令

* 模板文件

```tmpl
# cat templates/www.imlcs.com.tmpl
upstream {{"/domain/www"}} { # 获取单个 key 的值
    {{range getvs "/nginx/www/*"}} # 循环获取当前目录下的所有 key 的值
    server {{.}}; 
    {{end}}
}
server {
    listen 80;
    server_name {{getv "/domain/www"}};
    proxy_pass http://www;
}
```

测试
```shell
#confd 启动命令
confd --watch -backend etcd -node http://127.0.0.1:2379
#向 etcd 中写入数据
etcdctl  set /nginx/www/node1 192.168.1.11:80
etcdctl  set /nginx/www/node2 192.168.1.12:80
etcdctl  set /nginx/www/node3 192.168.1.13:80
etcdctl  set /domain/www www.imlcs.com

# 生成文件的内容

# cat /etc/nginx/conf.d/www.imlcs.com.conf
upstream /domain/www {

    server 192.168.1.11:80;

    server 192.168.1.12:80;

    server 192.168.1.13:80;

}
server {
    listen 80;
    server_name www.imlcs.com;
    proxy_pass http://www;
}
supvervisor 管理 confd 进程

# cat /etc/supervisord.d/confd.conf
[program:confd]
command=/usr/local/bin/confd --watch -backend etcd -node http://127.0.0.1:2379
autostart=true
startsecs=5
autorestart=true
startretries=3
user=root
redirect_stderr = true
stdout_logfile_maxbytes=20MB
stdout_logfile_backups=20
stopasgroup=true
killasgroup=true
exitcodes=0,2
stopsignal=KILL
```

## 通过 Nacos 让 Nginx 拥有服务发现能力
