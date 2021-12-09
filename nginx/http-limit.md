## 漏桶算法

Nginx按请求速率限速模块使用的是漏桶算法，算法思想：

- 水（请求）从上方倒入水桶，从水桶下方流出（被处理）；
- 来不及流出的水存在水桶中（缓冲），以固定速率流出；
- 水桶满后水溢出（丢弃）。
- 这个算法的核心是：缓存请求、匀速处理、多余的请求直接丢弃。
- 相比漏桶算法，令牌桶算法不同之处在于它不但有一只“桶”，还有个队列，这个桶是用来存放令牌的，队列才是用来存放请求的。

## 限制请求数和连接数

- limit_req_zone 用来限制单位时间内单个标识客户端的请求数，即速率限制,采用的漏桶算法 "leaky bucket"。
- limit_req_conn 用来限制单个标识客户端同一时间连接数，即并发限制。

	*标识客户端可以是针对客户端，也可以针对请求的地址。*

## imit_req_zone 参数配置

* limit_req_zone $binary_remote_addr zone=one:10m rate=1r/s;

- $binary_remote_addr 表示通过remote_addr这个标识来做限制，“binary_”的目的是缩写内存占用量，是限制同一客户端ip地址。
- zone=one:10m 表示生成一个大小为10M，名字为one的内存区域，用来存储访问的频次信息。
- rate=1r/s 表示允许相同标识的客户端的访问频次，这里限制的是每秒1次，还可以有比如30r/m的。

* limit_req zone=one burst=5 nodelay;

- zone=one 设置使用哪个配置区域来做限制，与上面limit_req_zone 里的name对应。
- burst=5 重点说明一下这个配置，burst爆发的意思，这个配置的意思是设置一个大小为5的缓冲区当有大量请求（爆发）过来时，超过了访问频次限制的请求可以先放到这个缓冲区内。
- nodelay 如果设置，超过访问频次而且缓冲区也满了的时候就会直接返回503，如果没有设置，则所有请求会等待排队。

* limit_req_log_level error;

当服务器由于limit被限速或缓存时，配置写入日志。延迟的记录比拒绝的记录低一个级别。例子：limit_req_log_level notice延迟的的基本是info。

```conf
http {
        limit_conn_zone $binary_remote_addr zone=perip:10m;

        limit_req_zone $binary_remote_addr zone=qps1:1m  rate=1r/s;
        limit_req_zone $binary_remote_addr zone=qps2:1m  rate=2r/s;
        limit_req_zone $binary_remote_addr zone=qps3:1m  rate=4r/s;

        limit_req_zone $server_name zone=perserver:10m rate=10r/s;
        limit_req_zone $request_uri zone=perrequesturi:10m rate=500r/s;
        limit_req_zone $uri zone=peruri:10m rate=100r/s;
        limit_req_zone "$binary_remote_addr$request_uri" zone=req_dev:10m rate=2r/s;

        limit_req_zone global zone=apiqps:1k rate=200/s;
        limit_req_zone global zone=apiqps2:1k rate=500/s;
}
```

## 根据请求url限制

- limit_req_zone $binary_remote_addr $uri zone=two:3m rate=1r/s;               # $uri:不带客户端请求参数
- limit_req_zone $binary_remote_addr $request_uri zone=thre:3m rate=1r/s;      # $request_uri:带客户端请求参数

## limit_conn_zone 参数配置

* limit_conn_zone $binary_remote_addr zone=addr:10m;

参数与 imit_req_zone 雷同

```conf
limit_conn_zone $binary_remote_addr zone=perip:10m; ## 每个ip连接数
limit_conn_zone $server_name zone=perserver:10m; ## 每个服务连接数

server {
        limit_conn perip 5;
	limit_conn perserver 100;
        # 限制下载速度
        limit_rate 100k;

        location / {
                limit_req zone=qps1 burst=2;
                try_files $uri $uri/ /index.php?$query_string;
        }
}
```

如何设置能限制某个IP某一时间段的访问次数是一个让人头疼的问题，特别面对恶意的ddos攻击的时候。
其中 CC 攻击（Challenge Collapsar）是 DDOS（分布式拒绝服务）的一种，也是一种常见的网站攻击方法，攻击者通过代理服务器或者肉鸡向向受害主机不停地发大量数据包，造成对方服务器资源耗尽，一直到宕机崩溃。

CC 攻击一般就是使用有限的 ip 数对服务器频繁发送数据来达到攻击的目的，nginx 可以通过 HttpLimitReqModule 和 HttpLimitZoneModule 配置来限制 ip 在同一时间段的访问次数来防 CC 攻击。

HttpLimitReqModule 用来限制连单位时间内连接数的模块，使用 limit_req_zone 和 limit_req 指令配合使用来达到限制。一旦并发连接超过指定数量，就会返回503错误。

HttpLimitConnModule 用来限制单个 ip 的并发连接数，使用 limit_zone 和 limit_conn 指令。

这两个模块的区别前一个是对一段时间内的连接数限制，后者是对同一时刻的连接数限制。

HttpLimitReqModul 限制某一段时间内同一ip访问数实例：
```conf
http{
    # 定义一个名为 allips 的 limit_req_zone 用来存储session，大小是10M内存，
    # 以 $binary_remote_addr 为 key，限制平均每秒的请求为20个，
    # 1M 能存储 16000 个状态，rete 的值必须为整数，
    # 如果限制两秒钟一个请求，可以设置成 30r/m
    limit_req_zone $binary_remote_addr zone=allips:10m rate=20r/s;

    server{
		location {
			#限制每ip每秒不超过20个请求，漏桶数burst为5
			#brust的意思就是，如果第1秒、2,3,4秒请求为19个，
			#第5秒的请求为25个是被允许的。
			#但是如果你第1秒就25个请求，第2秒超过20的请求返回503错误。
			#nodelay，如果不设置该选项，严格使用平均速率限制请求数，
			#第1秒25个请求时，5个请求放到第2秒执行，
			#设置nodelay，25个请求将在第1秒执行。
			limit_req zone=allips burst=5 nodelay;
		}
    }
}
```

HttpLimitZoneModule 限制并发连接数实例：
```conf
# limit_zone只能定义在http作用域，limit_conn 可以定义在http server location作用域

http{
    #定义一个名为one的limit_zone,大小10M内存来存储session，
    #以$binary_remote_addr 为key
    #nginx 1.18以后用limit_conn_zone替换了limit_conn
    #且只能放在http作用域
    limit_conn_zone one $binary_remote_addr 10m;

    server{
		location {
			limit_conn one 20; #连接数限制
			#带宽限制,对单个连接限数，如果一个ip两个连接，就是500x2k
			limit_rate 500k;
		}
    }
}
```

## 白名单设置

以上配置会对所有的 ip 都进行限制，有些时候我们不希望对搜索引擎的蜘蛛或者自己测试 ip 进行限制，

1.对于特定的白名单 ip 我们可以借助 geo 指令实现。
```conf
http{
    geo $limited{
		default 1;

		#google
		64.233.160.0/19 0;
		65.52.0.0/14 0;
		66.102.0.0/20 0;
		66.249.64.0/19 0;
		72.14.192.0/18 0;
		74.125.0.0/16 0;
		209.85.128.0/17 0;
		216.239.32.0/19 0;

		#M$
		64.4.0.0/18 0;
		157.60.0.0/16 0;
		157.54.0.0/15 0;
		157.56.0.0/14 0;
		207.46.0.0/16 0;
		207.68.192.0/20 0;
		207.68.128.0/18 0;

		#yahoo
		8.12.144.0/24 0;
		66.196.64.0/18 0;
		66.228.160.0/19 0;
		67.195.0.0/16 0;
		74.6.0.0/16 0;
		68.142.192.0/18 0;
		72.30.0.0/16 0;
		209.191.64.0/18 0;

		#My IPs
		127.0.0.1/32 0;
		123.456.0.0/28 0; #example for your server CIDR
	}
}
```

geo指令定义了一个白名单$limited变量，默认值为1，如果客户端ip在上面的范围内，$limited的值为0

2.使用map指令映射搜索引擎客户端的ip为空串，如果不是搜索引擎就显示本身真是的ip，这样搜索引擎 ip 就不能存到 limit_req_zone 内存 session 中，所以不会限制搜索引擎的 ip 访问

```conf
map $limited $limit {
    1 $binary_remote_addr;
    0 "";
}
```

3.设置 limit_req_zone 和 limit_req
```conf
    limit_req_zone $limit zone=foo:1m rate=10r/m;
    limit_req zone=foo burst=5;
```

案例：
限制只允许一分钟内只允许一个 ip 访问60次配置，也就是平均每秒1次，首先我们准备一个test.php脚本放在根目录 $document_root 下。

```conf
http{
    limit_req_zone $binary_remote_addr zone=allips:10m rate=60r/m;

    server{
		location {
			limit_req zone=allips;
		}
    }
}
```
如果我们未设置 brust 和 nodelay ，则可以看到该配置只允许每秒访问 1 次，超出的请求会返回503错误：
```text
ab -n 5 -c 1 http://localhost/test.php
118.144.94.193 - - [22/Dec/2012:06:27:06 +0000] "GET /test.php HTTP/1.0" 200 11000 "-" "ApacheBench/2.3"
118.144.94.193 - - [22/Dec/2012:06:27:06 +0000] "GET /test.php HTTP/1.0" 503 537 "-" "ApacheBench/2.3"
118.144.94.193 - - [22/Dec/2012:06:27:07 +0000] "GET /test.php HTTP/1.0" 503 537 "-" "ApacheBench/2.3"
118.144.94.193 - - [22/Dec/2012:06:27:07 +0000] "GET /test.php HTTP/1.0" 503 537 "-" "ApacheBench/2.3"
118.144.94.193 - - [22/Dec/2012:06:27:07 +0000] "GET /test.php HTTP/1.0" 503 537 "-" "ApacheBench/2.3"
```
现在我们设置一下brust和nodelay，结果如下：

```conf
http{
    ...
    limit_req_zone $binary_remote_addr zone=allips:10m rate=60r/m;
    server{
    location {
		limit_req zone=allips burst=1 nodelay;
    }
    }
}
```

可以看出，设置了 brust=1 和 nodelay 后，第1秒可以处理两个请求。

备注：经过多次测试发现，rate 与 burst 设置在执行层面上并不会很精确。
