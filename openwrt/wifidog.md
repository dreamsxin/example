# 

Wifidog将ping协议做为心跳机制向认证服务器发送当前状态信息。这可以实现为认证服务器每个节点的状态生成中央日志。

Wifidog客户端在conf文件中进行设置，目的是通过http定期启动thread(ping_thread.c)向认证服务器发送状态信息。信息格式如下：

http://auth_sever/ping/?gw_id=%s&sys_uptime=%lu&sys_memfree=%u&sys_load=%.2f&wifidog_uptime=%lu

通过系统调用wifidog客户端收集的数据

Headers
HTTP/1.0\r\n" 
"User-Agent: WiFiDog %s\r\n" 
"Host: %s\r\n" 
"\r\n",

一个标准的HTTP需求应该是：

GET /ping/?gw_id=001217DA42D2&sys_uptime=742725&sys_memfree=2604&sys_load=0.03&wifidog_uptime=3861 HTTP/1.0
User-Agent: WiFiDog 1.1.3_beta6
Host: www.wifidog.pro

## 认证服务器认证协议

这个页面描述了当用户已经被认证并允许访问互联网时，为了认证用户和进程，wifidog网关和认证服务器之间的信息传送。

Wifidog客户端将定期的启动一个thread来报告每个用户的连接状况。目前它被用来报告每个用户输入/输出计数器，以显示用户依然在现，并允许认证服务器将不再连接的用户断开。
以下是发给每个在线用户的信息

auth_server:/auth/index.php?stage=&ip=&mac=&token=&incoming=&outgoing=

注意：stage=计数器/登录，取决于是否是新客户端
即使输入输出变量会在所有信息中出现，但他们只对处于counter阶段的信息有效。其它情况下输入输出经常设置为0。
在做回复时，认证服务器会以有效身份或新用户信息，或者认证服务器错误提示形式进行回复。
回复格式如下：

Auth:

新用户状态为：

0 - AUTH_DENIED - User firewall users are deleted and the user removed.
6 - AUTH_VALIDATION_FAILED - User email validation timeout has occured and user/firewall is deleted
1 - AUTH_ALLOWED - User was valid, add firewall rules if not present
5 - AUTH_VALIDATION - Permit user access to email to get validation email under default rules
-1 - AUTH_ERROR - An error occurred during the validation process

注意：认识服务器错误一般不会改变防火墙或用户状态

标准的URL为：

GET /auth/?stage=counters&ip=7.0.0.107&mac=00:40:05:5F:44:43&token=4f473ae3ddc5c1c2165f7a0973c57a98&incoming=6031353&outgoing=827770 HTTP/1.0
User-Agent: WiFiDog 1.1.3_beta6
Host: www.wifidog.pro

## 网关重定向浏览器

客户端浏览器在不同情况下会被重定向到其它页面：

初始化请求：

基于捕捉，客户端会被网关重定向到以下URL：

login/?gw_address=%s&gw_port=%d&gw_id=%s&url=%s 
例如：https://www.wifidog.pro/login/?gw_id=0016B6DA9AE0&gw_address=7.0.0.1&gw_port=2060

初始化请求之后

当请求被处理并且客户端已经被重定向到网关时

如果服务器回复AUTH_DENIED：注意你通常在标准认证服务器上看不到这样的提示。客户端将不会被重定向回网关。

gw_message.php?message=denied

如果服务器回复AUTH_VALIDATION:

gw_message.php?message=activate

如果服务器回复AUTH_ALLOWED:这是门户重定向：

portal/?gw_id=%s

如果服务器回复AUTH_VALIDATION_FAILED:注意你将不会在标准认证服务器看到此回复。客户端将不会重定向回网关。

gw_message.php?message=failed_validation

认证服务器重定向浏览器

基于成功登录，客户端将被重定向到网关。

http://" . $gw_address . ":" . $gw_port . "/wifidog/auth?token=" . $token

URL示例：http://192.168.1.1:2060/wifidog/auth?token=4f473ae3ddc5c1c2165f7a0973c57a98

## 认证接口

实现 wifidog 4 个接口： portal，login，auth，ping （还有一个get_gw_message.php 的接口，可不用实现）

简单来说，就是路由器会以GET方式请求 你的服务器（加入了白名单） 以下四个地址：
- http://server/router/login
- http://server/router/auth
- http://server/router/ping
- http://server/router/portal

以下以php为例子，实现
（ 我是写了一个rewrite ， 把所有请求转发到了 index.php?q=，当然，你也可以每个请求建立一个文件夹下一个 index.php）

具体代码如下：

$q = $_GET['q'] ;
$q = explode('/',$q) ;
$q = $q[0] ;
$a = '' ;
if(!empty($q[1])) $a = $q[1] ;


if($q == 'portal'){
        print 'portal' ;
}

if($q == 'login'){
        print 'login' ;
        $gw_port = $_GET['gw_port']  ;
        $gw_address = $_GET['gw_address']  ;
        //token 自己生成一个
        print '<br /><a href="http://'.$gw_address.':'.$gw_port.'/wifidog/auth?token=789">login</a>' ;
}

if($q == 'auth'){
        $token =  $_GET['token']  ;
        if($token == '789') print "Auth: 1"; 
        else print "Auth: 0"; 
}



if($q == 'ping') print 'Pong' ;


## 把wifidog直接编译进openwrt

openwrt 根目录make menuconfig 搜索 Captive portal/ 看下有没有wifidog选项，如果没有

./scripts/feeds update -a
./scripts/feeds install -a
make menuconfig

还不行的话，在项目下终端：

vim feeds.conf.default

增加这一行：

`src-git wifidog https://github.com/wifidog/wifidog-gateway.git`

然后重新：
```shell
./scripts/feeds update -a
./scripts/feeds install -a
```

终端执行 make menuconfig, 在Network/captive portals/下选择wifidog 就有选择 WiFiDog 这一项了。

## LuCI 加入管理界面

https://github.com/ewinds/luci-app-wifidog

wifidog启动脚本中获取LuCI配置

```shell
wifidog_enable=$(uci get wifidog.settings.wifidog_enable)
offline_enable=$(uci get wifidog.settings.offline_enable)
```