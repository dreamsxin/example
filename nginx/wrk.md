## 编译

```shell
git clone https://github.com/wg/wrk.git
cd wrk
make
```

## 使用

用12个线程模拟100个连接，持续30秒，Socket超时时间30秒。
```shell
wrk -t12 -c100 -d30s -T30s http://www.myleftstudio.com
```

30 秒钟结束以后可以看到类似如下输出: 

```text
Running 30s test @ http://www.myleftstudio.com
12 threads and 100 connections  
  Thread Stats   Avg      Stdev     Max   +/- Stdev  
    Latency   538.64ms  368.66ms   1.99s    77.33%  
    Req/Sec    15.62     10.28    80.00     75.35%  
5073 requests in 30.09s, 75.28MB read  
  Socket errors: connect 0, read 5, write 0, timeout 64  
Requests/sec:    168.59  
Transfer/sec:      2.50MB
```

- Latency
可以理解为响应时间, 有平均值，标准偏差，最大值，正负一个标准差占比。

- Req/Sec
每个线程每秒钟的完成的请求数，同样有平均值, 标准偏差，最大值，正负一个标准差占比。

一般我们来说我们主要关注平均值和最大值，标准差如果太大说明样本本身离散程度比较高，有可能系统性能波动很大。

## lua 脚本

首先创建一个 `post.lua` 的文件: 
```lua
wrk.method = "POST"  
wrk.body   = "foo=bar&baz=quux"  
wrk.headers["Content-Type"] = "application/x-www-form-urlencoded"
```.
然后执行: 
```shell
wrk -t12 -c100 -d30s -T30s --script=post.lua --latency http://www.myleftstudio.com
```

