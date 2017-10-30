# Keepalived

Keepalived 是一个基于 VRRP 协议来实现的服务高可用方案，可以利用其来避免 IP 单点故障，类似的工具还有 heartbeat、corosync、pacemaker。但是它一般不会单独出现，而是与其它负载均衡技术（如 lvs、haproxy、nginx）一起工作来达到集群的高可用。

## VRRP 协议

VRRP 全称 Virtual Router Redundancy Protocol，即虚拟路由冗余协议。可以认为它是实现路由器高可用的容错协议，即将 N 台提供相同功能的路由器组成一个路由器组(Router Group)，这个组里面有一个 master 和多个 backup，但在外界看来就像一台一样，构成虚拟路由器，拥有一个虚拟 IP（vip，也就是路由器所在局域网内其他机器的默认路由），占有这个 IP 的 master 实际负责ARP相应和转发 IP 数据包，组中的其它路由器作为备份的角色处于待命状态。master 会发组播消息，当backup在超时时间内收不到vrrp包时就认为master宕掉了，这时就需要根据VRRP的优先级来选举一个backup当master，保证路由器的高可用。

在 VRRP 协议实现里，虚拟路由器使用 `00-00-5E-00-01-XX` 作为虚拟MAC地址，XX 就是唯一的 VRID （Virtual Router IDentifier），这个地址同一时间只有一个物理路由器占用。在虚拟路由器里面的物理路由器组里面通过多播IP地址 `224.0.0.18` 来定时发送通告消息。每个 Router 都有一个 1-255 之间的优先级别，级别最高的（highest priority）将成为主控（master）路由器。通过降低master的优先权可以让处于backup状态的路由器抢占（pro-empt）主路由器的状态，两个 backup 优先级相同的 IP 地址较大者为master，接管虚拟IP。

![https://github.com/dreamsxin/example/blob/master/keepalived/img/vrrp.jpg](https://github.com/dreamsxin/example/blob/master/keepalived/img/vrrp.jpg?raw=true)

## 与 heartbeat、corosync 比较

Heartbeat、Corosync 属于同一类型。Keepalived 使用的 vrrp 协议方式，虚拟路由冗余协议 (Virtual Router Redundancy Protocol，简称VRRP)，Heartbeat 和 Corosync 是基于主机或网络服务的高可用方式。简单的说，Keepalived 的目的是模拟路由器的高可用，Heartbeat 或 Corosync 的目的是实现 Service 的高可用。

所以一般 Keepalived 是实现前端高可用，常用的前端高可用的组合有 LVS + Keepalived、Nginx + Keepalived、HAproxy + Keepalived。
而Heartbeat 或 Corosync 是实现服务的高可用，常见的组合有 Heartbeat v3(Corosync) + Pacemaker + NFS + Httpd、Heartbeat v3(Corosync)+ Pacemaker + NFS + MySQL。
总结一下，Keepalived 中实现轻量级的高可用，一般用于前端高可用，且不需要共享存储，一般常用于两个节点的高可用。而 Heartbeat(Corosync) 一般用于服务的高可用，且需要共享存储，一般用于多节点的高可用。

那 heartbaet 与 corosync 我们又应该选择哪个好啊，我想说我们一般用 corosync，因为 corosync 的运行机制更优于 heartbeat，就连从 heartbeat 分离出来的 pacemaker 都说在以后的开发当中更倾向于 corosync，所以现在 corosync + pacemaker 是最佳组合。

Keepalived 可以认为是 VRRP 协议在 Linux 上的实现，主要有三个模块，分别是 core、check 和 vrrp。core 模块为 keepalived 的核心，负责主进程的启动、维护以及全局配置文件的加载和解析。check负责健康检查，包括常见的各种检查方式。

## 配置选项说明

* global_defs

- notification_email ： keepalived在发生诸如切换操作时需要发送email通知地址，后面的 smtp_server 相比也都知道是邮件服务器地址。也可以通过其它方式报警，毕竟邮件不是实时通知的。
- router_id ： 机器标识，通常可设为hostname。故障发生时，邮件通知会用到

* vrrp_instance

- state ： 指定instance(Initial)的初始状态，就是说在配置好后，这台服务器的初始状态就是这里指定的，但这里指定的不算，还是得要通过竞选通过优先级来确定。如果这里设置为MASTER，但如若他的优先级不及另外一台，那么这台在发送通告时，会发送自己的优先级，另外一台发现优先级不如自己的高，那么他会就回抢占为MASTER
- interface ： 实例绑定的网卡，因为在配置虚拟IP的时候必须是在已有的网卡上添加的
- mcast_src_ip ： 发送多播数据包时的源IP地址，这里注意了，这里实际上就是在那个地址上发送VRRP通告，这个非常重要，一定要选择稳定的网卡端口来发送，这里相当于heartbeat的心跳端口，如果没有设置那么就用默认的绑定的网卡的IP，也就是interface指定的IP地址
- virtual_router_id ： 这里设置VRID，这里非常重要，相同的VRID为一个组，他将决定多播的MAC地址
- priority ： 设置本节点的优先级，优先级高的为master
- advert_int ： 检查间隔，默认为1秒。这就是VRRP的定时器，MASTER每隔这样一个时间间隔，就会发送一个advertisement报文以通知组内其他路由器自己工作正常
- authentication ： 定义认证方式和密码，主从必须一样
- virtual_ipaddress ： 这里设置的就是VIP，也就是虚拟IP地址，他随着state的变化而增加删除，当state为master的时候就添加，当state为backup的时候删除，这里主要是有优先级来决定的，和state设置的值没有多大关系，这里可以设置多个IP地址
- track_script ： 引用VRRP脚本，即在 vrrp_script 部分指定的名字。定期运行它们来改变优先级，并最终引发主备切换。

* vrrp_script

告诉 keepalived 在什么情况下切换，可以有多个 `vrrp_script`。

- script ： 自己写的检测脚本。也可以是一行命令如killall -0 nginx
- interval 2 ： 每2s检测一次
- weight -5 ： 检测失败（脚本返回非0）则优先级 -5
- fall 2 ： 检测连续 2 次失败才算确定是真失败。会用weight减少优先级（1-255之间）
- rise 1 ： 检测 1 次成功就算成功。但不修改优先级

这里要提示一下script一般有2种写法：

- 通过脚本执行的返回结果，改变优先级，keepalived 继续发送通告消息，backup 比较优先级再决定
- 脚本里面检测到异常，直接关闭 keepalived 进程，backup 机器接收不到 advertisement 会抢占IP

个人更倾向于通过 shell 脚本判断，但有异常时 `exit 1`，正常退出 `exit 0`，然后 keepalived 根据动态调整的 `vrrp_instance` 优先级选举决定是否抢占VIP：

- 如果脚本执行结果为0，并且 weight 配置的值大于0，则优先级相应的增加
- 如果脚本执行结果非0，并且 weight 配置的值小于0，则优先级相应的减少

其他情况，原本配置的优先级不变，即配置文件中 priority 对应的值。

* 提示

>> 优先级不会不断的提高或者降低
>> 可以编写多个检测脚本并为每个检测脚本设置不同的 weight（在配置中列出就行）
>> 不管提高优先级还是降低优先级，最终优先级的范围是在[1,254]，不会出现优先级小于等于0或者优先级大于等于255的情况
>> 在 MASTER 节点的 vrrp_instance 中 配置 nopreempt ，当它异常恢复后，即使它 prio 更高也不会抢占，这样可以避免正常情况下做无谓的切换

以上可以做到利用脚本检测业务进程的状态，并动态调整优先级从而实现主备切换。

在默认的 `keepalive.conf` 里面还有 virtual_server,real_server 这样的配置，我们这用不到，它是为lvs准备的。 notify 可以定义在切换成 MASTER 或 BACKUP 时执行的脚本。