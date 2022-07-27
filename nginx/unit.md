# Unit

Unit 是一个现代的动态应用服务器。支持多语言应用、反向代理和静态文件服务。
https://github.com/nginx/unit

## 特性

1. 动态：基于HTTP的RESTful JSON API的配置热加载
2. 语言：支持主流语言：Java, Python, PHP, Perl, Ruby, Go, NodeJS
3. 性能：高性能，高并发，低内存
4. 安全：支持命名空间，进程隔离和TLS

### 核心功能
1. 路由：好用又强大的路由，很适合做微服务网关
2. 即时响应：任意状态码的响应，比如重定向301
3. 文件服务：提供静态文件资源服务
4. 应用服务：同时跑多个语言和各版本
5. 负载均衡：支持代理和负载均衡

### 其它功能
1. 进程管理：支持静态和动态扩展，并且相互隔离
2. 多线程：多线程方式处理请求
3. 证书管理：支持HTTPS/TLS处理
4. NJS支持：用JS扩展配置处理

## 上手体验

简单讲就是启动后根据需要更改配置。

1. 安装：
```shell
curl --output /usr/share/keyrings/nginx-keyring.gpg  \
      https://unit.nginx.org/keys/nginx-keyring.gpg
```
创建文件 `/etc/apt/sources.list.d/unit.list`:
```text
deb [signed-by=/usr/share/keyrings/nginx-keyring.gpg] https://packages.nginx.org/unit/ubuntu/ jammy unit
deb-src [signed-by=/usr/share/keyrings/nginx-keyring.gpg] https://packages.nginx.org/unit/ubuntu/ jammy unit
```

```shell
sudo apt update
sudo apt install unit
sudo apt install unit-dev unit-go unit-php
sudo systemctl restart unit
```

默认 `/var/run/control.unit.sock`
也可以使用监听端口得方式：
```shell
sudo unitd --control 127.0.0.1:8400
```

2.显示配置
```shell
curl http://127.0.0.1:8400
curl --unix-socket /var/run/control.unit.sock
```
* 显示部分配置
```shell
curl 'http://127.0.0.1:8400/config/applications/blogs'
```

3.更改配置

```shell
cat << EOF > config.json
{
    "type": "php",
    "root": "/var/www/html/blogs/scripts"
}
EOF

curl -X PUT --data-binary @config.json --unix-socket /var/run/control.unit.sock \
       http://localhost/config/applications/blogs
```

核心选项：
- listeners
- routes
- applications
- upstreams
- match
- action
- return
- share
- proxy
- pass

a. 即时响应示例：
```json
{
    "listeners": {
        "*:7080": {
            "pass": "routes"
        }
    },
    "routes": [
        {
            "action": {
                "return": 206
            }
        }
    ]
}
```
b. 文件服务示例
```json
{
    "listeners": {
        "*:7080": {
            "pass": "routes"
        }
    },
    "routes": [
        {
            "action": {
                "share": "/www/html"
            }
        }
    ]
}
```

c. 应用服务示例
```json
{
    "listeners": {
        "*:7080": {
            "pass": "applications/python"
        }
    },
    "applications": {
        "python": {
            "type": "python",
            "path": "/www/",
            "module": "wsgi"
        }
    }
}
```
`/www/wsgi.py`
```python
def application(environ, start_response):
    start_response("200 OK", [("Content-Type", "text/plain")])
    return (b"Hello Unit Python!")
```

d. 负载均衡

第一种. 简单代理
```json
{
    "listeners": {
        "*:7080": {
            "pass": "routes"
        }
    },
    "routes": [
        {
            "action": {
                "proxy": "http://127.0.0.1:8080"
            }
        }
    ]
}
```

第二种. 负载均衡
```json
{
    "listeners": {
        "*:7080": {
            "pass": "upstreams/rr-lb"
        }
    },
    "upstreams": {
        "rr-lb": {
            "servers": {
                "127.0.0.1:8080": {},
                "127.0.0.1:8081": {
                    "weight": 5
                },
                "127.0.0.1:8082": {}
            }
        }
    }
}
```

