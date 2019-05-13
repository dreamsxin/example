# Simple Service Discovery Protocol (简单服务发现协议)

实现无配置，自发现局域网内部服务的协议。

SSDP 全称是 Simple Service Discover Protocol 简单服务发现协议，这个协议是 UPnP 的核心，在 UPnP 中定义了一组协议框架，其中有控制点，根设备等概念，UPnP 设备通过 SSDP 协议与根设备（用户设备）进行交互。SSDP 是应用层协议，使用 HTTPU 和 HTTPMU 规范，基于 UDP 端口进行通信。

SSDP 使用一个固定的组播地址 `239.255.255.250` 和 UDP 端口号 `1900` 来监听其他设备的请求，负责多播数据。

## UPnP

Universal Plug and Play，中文是 “通用即插即用”。在理解 UPnP 之前，我们先了解一下传统的 PnP 技术，因为 UPnP 是对于传统 PnP（即插即用）概念的扩展。

## 请求消息

SSDP 协议主要有有两种请求消息类型，查询请求和服务通知。

- 查询请求，协议客户端用此请求查询某种类型的设备和服务。
- 服务通知，设备和服务使用此类通知消息声明自己存在。

### 设备查询 discovery

当一个客户端接入网络的时候，它可以使用 `M-SEARCH` 方法发送 `ssdp:discover` 消息。当设备监听到这个保留的多播地址上由控制点发送的消息的时候，设备将通过单播的方式直接响应控制点的请求。

典型的设备查询请求消息格式：
```text
M-SEARCH * HTTP/1.1
Host:239.255.255.250:1900
ST:urn:schemas-upnp-org:device:InternetGatewayDevice:1
Man:"ssdp:discover"
MX:3
```

- M-SEARCH 说明这是强制的搜索方法(由Mandatory Extensions in HTTP中的Mandatory HTTP Requests确定)
- ST （search term)搜索条件，指明需要搜索的设备，可以是类型，服务，甚至是UUID，至于怎么回应嘛……那是服务端的事了
- Man M-SEARCH请求必须带的数据项，值必须为”ssdp:discover”
- MX 优先级，数字越高，优先级越低

对应的响应消息，应该包含服务的位置信息（Location 或AL头），ST和USN头。响应消息应该包含cache控制信息（max-age 或者 Expires头）：
```text
HTTP/1.1 200 OK
Cache-Control: max-age= seconds until advertisement expires
S: uuid:ijklmnop-7dec-11d0-a765-00a0c91e6bf6
Location: URL for UPnP description for root device
Cache-Control: no-cache="Ext",max-age=5000ST:ge:fridge // search targetUSN: uuid:abcdefgh-7dec-11d0-a765-00a0c91e6bf6 // advertisement UUIDAL: <blender:ixl><http://foo/bar>
```

* 服务发现流程

```text
            +---------+ +---------+               +-----------+
            | Client  | | Server  |               | Multicast |
            +---------+ +---------+               +-----------+
---------------\ |           |                          |
| Initialized  |-|           |                          |
---------------- |           |                          |
                 |           |                          |
                 | discovery |                          |
                 |------------------------------------->|
                 |           |                          |
                 |           |          Client wants ST |
                 |           |< ------------------------|
                 |           | -------------------\     |
                 |           |-| In discovery ST? |     |
                 |           | --------------------     |
                 |           |                          |
                 |           | (In ST) alive            |
                 |           |------------------------->|
                 |           |                          |
                 |           |           Here is Server |
                 |< ------------------------------------|
                 |           |                          |
```

### 设备通知消息

在设备加入网络时，它应当向一个特定的多播地址的 SSDP 端口使用 NOTIFY 方法发送 “ssdp:alive” 消息，以便宣布自己的存在，更新期限信息，更新位置信息。
当一个设备计划从网络上卸载的时候，它也应当向一个特定的多播地址的 SSDP 端口使用 NOTIFY 方法发送 “ssdp:byebye” 消息。

* `ssdp:alive` 服务上线/广播存活/心跳包

由于 UDP 协议是不可信的，设备应该定期发送它的公告消息。在设备加入网络时，它必须用 NOTIFY 方法发送一个多播传送请求。NOTIFY 方法发送的请求没有回应消息。

典型的设备通知消息格式如下：
```text
NOTIFY * HTTP/1.1
HOST: 239.255.255.250:1900
CACHE-CONTROL: max-age=100
LOCATION: http://10.5.4.81:49155/TxMediaRenderer_desc.xml
NT: upnp:rootdevice
NTS: ssdp:alive
USN: uuid:001e4fd3fa0e0000_MR::upnp:rootdevice
```

- CACHE-CONTROL说明这个设备状态至少在100秒内不会过期，过期时，所有设备就必须要刷新这信息，如果得不到新的数据，则认为此设备不可用。如果不提供CACHE-CONTROL或者EXPIRES，此设备的信息将不允许缓存，超时机制由接受端决定
- LOCATION此设备的控制点或描述文件所在地

* `ssdp:byebye` 消息

当一个设备计划从网络上卸载的时候，它也应当向一个特定的多播地址的 SSDP 端口使用 NOTIFY 方法发送 “ssdp:byebye” 消息。但是，即使没有发送 “ssdp:byebye” 消息，控制点也会根据 “ssdp:alive” 消息指定的超时值，将超时并且没有再次收到的 “ssdp:alive” 消息对应的设备认为是失效的设备。

典型的设备卸载消息格式如下：
```text
NOTIFY * HTTP/1.1
Host: 239.255.255.250:1900
NT: someunique:idscheme3
NTS: ssdp:byebye
USN: someunique:idscheme3
```


- NOTIFY 通知所有广播域的机器
- HOST 值是固定的（IPv4），算是协议的一部分
- NT （Notification Type）这个是GENA的定义，即通知类型,值一般是当前设备的类型
- NTS (Notification Sub-Type)通知子类型，如果要遵守SSDP，这个值就代表了请求的类型，但是为什么NTS和NT搞混了呢……协议中写得非常明白

    5.3.5. Shouldn’t the NT and NTS values be switched? Yes, they should. Commands such as ssdp:alive and ssdp:byebye should be NT values and the service type, where necessary, should be the NTS. The current mix-up is a consequence of a previous design where the NT header was used in a manner much like we use the USN today. This really needs to change.

- USN 这个设备的UUID，防止设备的IP或者网络环境改变后，连接至错误的设备。
