## accept_mutex

当一个新连接到达时，如果激活了accept_mutex，那么多个Worker将以串行方式来处理，其中有一个Worker会被唤醒，其他的Worker继续保持休眠状态；如果没有激活accept_mutex，那么所有的Worker都会被唤醒，不过只有一个Worker能获取新连接，其它的Worker会重新进入休眠状态，这就是「惊群问题」。

Nginx缺省激活了accept_mutex，也就是说不会有惊群问题，但真的有那么严重么？

简单点说：Apache动辄就会启动成百上千的进程，如果发生惊群问题的话，影响相对较大；但是对Nginx而言，一般来说，worker_processes会设置成CPU个数，所以最多也就几十个，即便发生惊群问题的话，影响相对也较小。

另：高版本的Linux中，accept不存在惊群问题，不过epoll_wait等操作还有。

假设你养了一百只小鸡，现在你有一粒粮食，那么有两种喂食方法：

你把这粒粮食直接扔到小鸡中间，一百只小鸡一起上来抢，最终只有一只小鸡能得手，其它九十九只小鸡只能铩羽而归。这就相当于关闭了accept_mutex。
你主动抓一只小鸡过来，把这粒粮食塞到它嘴里，其它九十九只小鸡对此浑然不知，该睡觉睡觉。这就相当于激活了accept_mutex。
可以看到此场景下，激活accept_mutex相对更好一些，让我们修改一下问题的场景，我不再只有一粒粮食，而是一盆粮食，怎么办？

此时如果仍然采用主动抓小鸡过来塞粮食的做法就太低效了，一盆粮食不知何年何月才能喂完，大家可以设想一下几十只小鸡排队等着喂食时那种翘首以盼的情景。此时更好的方法是把这盆粮食直接撒到小鸡中间，让它们自己去抢，虽然这可能会造成一定程度的混乱，但是整体的效率无疑大大增强了。

Nginx缺省激活了accept_mutex（最新版缺省禁用），是一种保守的选择。如果关闭了它，可能会引起一定程度的惊群问题，表现为上下文切换增多（sar -w）或者负载上升，但是如果你的网站访问量比较大，为了系统的吞吐量，我还是建议大家关闭它。

通过工具 ngx-req-distr （https://github.com/openresty/openresty-systemtap-toolkit）来测量 accept_mutex 对性能的影响：
```shell
./ngx-req-distr -m `cat /path/to/nginx.pid`
```
上述命令会输出请求在多个 worker 间的分配情况。


## aio threads;

线程池能够良好应对的一个场景，是数据无法被完全缓存，例如流媒体服务器，我们上面的测试环境，就是模拟的流媒体服务。

```conf
thread_pool default threads=32 max_queue=65536;
aio threads=default;
```

默认情况下包括一个32个线程的线程池，长度为65536的请求队列。如果队列溢出，Nginx会输出如下错误并拒绝请求。

`thread pool "NAME" queue overflow: N tasks waiting`

这个错误表示这个线程池消费小于生产，所以可以增加队列长度，如果调整无效，说明系统达到了瓶颈。

另外，我们可以调整线程相关的参数，例如对不同场景，可以提供独立的线程池。
```conf
http {
    thread_pool one threads=128 max_queue=0;
    thread_pool two threads=32;

    server {
        location /one {
            aio threads=one;
        }

        location /two {
            aio threads=two;
        }
    }
}
```

假如你的缓存代理服务器有3块磁盘，内存不能放下预期需要缓存的文件，所以我们首先需要让磁盘工作最大化。

一个方式是RAID，好坏兼并。另一个方式是Nginx
```conf
# We assume that each of the hard drives is mounted on one of the directories:
# /mnt/disk1, /mnt/disk2, or /mnt/disk3 accordingly
proxy_cache_path /mnt/disk1 levels=1:2 keys_zone=cache_1:256m max_size=1024G 
                 use_temp_path=off;
proxy_cache_path /mnt/disk2 levels=1:2 keys_zone=cache_2:256m max_size=1024G 
                 use_temp_path=off;
proxy_cache_path /mnt/disk3 levels=1:2 keys_zone=cache_3:256m max_size=1024G 
                 use_temp_path=off;

thread_pool pool_1 threads=16;
thread_pool pool_2 threads=16;
thread_pool pool_3 threads=16;

split_clients $request_uri $disk {
    33.3%     1;
    33.3%     2;
    *         3;
}

location / {
    proxy_pass http://backend;
    proxy_cache_key $request_uri;
    proxy_cache cache_$disk;
    aio threads=pool_$disk;
    sendfile on;
}
```
使用了3个独立的缓存，每个缓存指定到一块磁盘，然后有3个独立的线程池。

`split_clients`模块用于缓存间的负载均衡。
`use_temp_path=off`参数让Nginx将缓存文件保存至文件同级目录，可以避免缓存更新时磁盘间的文件数据交换。