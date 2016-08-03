# Using the JSON-RPC API

官方文档：http://luci.subsignal.org/trac/wiki/Documentation/JsonRpcHowTo

LuCI是一个基于Lua语言开发的Web framework，提供UCI（统一配置接口）的web接口。LuCI也提供了一种基于JSON语法的RPC机制来访问其内部的库。当前LuCI提供了5个库的接口，分别是：auth、uci、fs、sys、ipkg。下面简单介绍一下使用方法，以及如何通过官方文档来构造json请求数据
AUTH库

认证过程是所有其他操作的前提，因为其他操作都需要auth认证成功后产生的token，否则会返回403错误。我们来看一下auth过程是怎么样的。下文所有RPC的操作都是使用curl工具。

```shell
curl -i -X POST -d '{"method":"login","params":["root","1"]}' http://192.168.1.118/luci/rpc/auth
```
返回：
```result
HTTP/1.1 200 OK 
Server: LuCId-HTTPd/1.0 
Expires: 0 
Content-Type: application/json 
Cache-Control: no-cache
Date: Mon, 17 Mar 2014 17:27:30 GMT
Set-Cookie: sysauth=72fe65ae2dd4c8e77623576faba4ac4c; path=/
Connection: close

{"id":null,"result":"72fe65ae2dd4c8e77623576faba4ac4c","error":null}
```

简单说明一下，上面的命令是使用curl工具通过POST方式发送了一个http请求，请求的内容是一个登陆请求
```json
{"method":"login","params":["root","1"]}
```

http返回的内容是json格式的字符串，其中result的值就是上面提到了token。OK，现在我们拿到了下面其他操作需要的关键字段TOKEN


另外，需要注意的是，LuCI 在 openwrt 里的网址是：`http://192.168.1.118/cgi-bin/luci/rpc/xxx`，多了一个cgi-bin。这是因为openwrt默认使用uhttpd作为web server。而在PC里使用`make runhttpd`启动的是lua web server。

## SYS库

SYS库提供了一下系统通用功能，比如说：执行一条命令，查看系统信息，重启等。具体功能请猛戳这个链接：http://luci.subsignal.org/api/luci/modules/luci.sys.html

这里找一个最简单来演示一下如何使用官方文档构造json数据，查看uptime时间。uptime函数返回一个设备从上电到现在的总时间(秒)。那么，看一下使用HTTP请求来调这个函数的方法，还是curl模拟： 

```shell
curl -i -X POST -d '{"method":"uptime"}' http://192.168.1.10:8080/luci/rpc/sys?auth=72fe65ae2dd4c8e77623576faba4ac4c
```

返回：
```result
HTTP/1.1 200 OK
Server: LuCId-HTTPd/1.0
Expires: 0
Content-Type: application/json
Cache-Control: no-cache
Date: Mon, 17 Mar 2014 17:49:42 GMT
Set-Cookie: sysauth=72fe65ae2dd4c8e77623576faba4ac4c; path=/luci
Connection: close

{"id":null,"result":1839536,"error":null}
```

HTTP请求发送的JSON数据为
```json
{"method":"uptime"}
```

返回内容中的result就是uptime函数的结果。

好了，到这里基本就能看出来LuCI的JSON-RPC接口是如何使用的，它的语法是这样的：
```json
{"method":"方法名", "params":["参数1", "参数2", "参数3", ...]}
```

HTTP请求的地址是：`http://[ip|hostname]:[port]/luci/rpc/[库名]?auth=[auth获取的TOKEN]`，本节中sys库的示例中“[库名]”字段即为“sys”

## UCI-Library `/rpc/uci`

支持的方法：

- ​(string) add(config, type)
- ​​(integer) apply(config)
- ​​(object) changes([config)]
- ​​(boolean) commit(config)
- ​​(boolean) delete(config, section[, option)]
- ​​(boolean) delete_all(config[, type)]
- ​​(array) foreach(config[, type)]
- ​​(mixed) get(config, section[, option)]
- ​​(object) get_all(config[, section)]
- ​​(mixed) get_state(config, section[, option)]
- ​​(boolean) revert(config)
- ​​(name) section(config, type, name, values)
- ​​(boolean) set(config, section, option, value)
- ​​(boolean) tset(config, section, values) 

## FS库 `/rpc/fs`

- readfile (filename)		读取文件，返回数据为文件内容的base64编码数据
- writefile (filename, data)	写入文件，输入data为内容的base64编码

## SYS库 `/rpc/sys`

支持以下功能：

- ​​Complete luci.sys library
- ​​Complete luci.sys.group library with prefixgroup.
- ​​Complete luci.sys.net library with prefixnet.
- ​​Complete luci.sys.process library with prefixprocess.
- ​​Complete luci.sys.user library with prefixuser.
- ​​Complete luci.sys.wifi library with prefixwifi.

需要注意的是，sys库中有些子库的用法。比如`luci.sys.wifi`库，它的请求地址和sys是一样的。但是JSON结构中`method`的值有点差别，需要加一个`wifi.`前缀。
