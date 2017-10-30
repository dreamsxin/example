# Nginx + Keepalived 实现站点高可用 

避免单点故障，使用 2 台虚拟机通过 Keepalived 工具来实现 nginx 的高可用（High Avaiability），达到一台nginx入口服务器宕机，另一台备机自动接管服务的效果。

本文基于如下的拓扑图：

```text
                   +-------------+
                   |    uplink   |
                   +-------------+
                          |
                          +
    MASTER            keep|alived         BACKUP
172.29.88.224      172.29.88.222      172.29.88.225
+-------------+    +-------------+    +-------------+
|   nginx01   |----|  virtualIP  |----|   nginx02   |
+-------------+    +-------------+    +-------------+
                          |
       +------------------+------------------+
       |                  |                  |
+-------------+    +-------------+    +-------------+
|    web01    |    |    web02    |    |    web03    |
+-------------+    +-------------+    +-------------+
```

环境是 `CentOS 6.2 X86_64`

## 安装 Keepalived

```shell
# yum install -y keepalived
# keepalived -v
Keepalived v1.2.13 (03/19,2015)
```

## 创建 nginx 监控脚本 `/etc/keepalived/check_nginx.sh`

该脚本检测ngnix的运行状态，并在nginx进程不存在时尝试重新启动ngnix，如果启动失败则停止keepalived，准备让其它机器接管。

```shell
#!/bin/bash
counter=$(ps -C nginx --no-heading|wc -l)
if [ "${counter}" = "0" ]; then
    /usr/local/bin/nginx
    sleep 2
    counter=$(ps -C nginx --no-heading|wc -l)
    if [ "${counter}" = "0" ]; then
        /etc/init.d/keepalived stop
    fi
fi
```

你也可以根据自己的业务需求，总结出在什么情形下关闭 keepalived，如 curl 请求主页连续 2 个 3s 没有响应则切换：
```shell
#!/bin/bash
# curl -IL http://localhost/member/login.htm
# curl --data "memberName=fengkan&password=22" http://localhost/member/login.htm
count=0
for (( k=0; k<2; k++ ))
do
    check_code=$( curl --connect-timeout 3 -sL -w "%{http_code}\\n" http://localhost/login.html -o /dev/null )
    if [ "$check_code" != "200" ]; then
        count=$(expr $count + 1)
        sleep 3
        continue
    else
        count=0
        break
    fi
done
if [ "$count" != "0" ]; then
#   /etc/init.d/keepalived stop
    exit 1
else
    exit 0
fi
```

## MASTER 配置 `keepalived.conf`

```conf
! Configuration File for keepalived
global_defs {
    notification_email {
        zhouxiao@example.com
        itsection@example.com
    }
    notification_email_from itsection@example.com
    smtp_server mail.example.com
    smtp_connect_timeout 30
    router_id LVS_DEVEL
}

vrrp_script chk_nginx {
#    script "killall -0 nginx"
    script "/etc/keepalived/check_nginx.sh"
    interval 2
    weight -5
    fall 3  
    rise 2
}

vrrp_instance VI_1 {
    state MASTER
    interface eth0
    mcast_src_ip 172.29.88.224
    virtual_router_id 51
    priority 101
    advert_int 2
    authentication {
        auth_type PASS
        auth_pass 1111
    }
    virtual_ipaddress {
        172.29.88.222
    }
    track_script {
       chk_nginx
    }
}
```

在其它 BACKUP 上，只需要改变：
- `state MASTER` -> `state BACKUP`
- priority 101 -> priority 100
- mcast_src_ip 172.29.88.224 -> mcast_src_ip 172.29.88.225

## 重启

```shell
service keepalived restart
```
