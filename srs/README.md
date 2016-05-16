# 编译安装

```shell
git clone https://github.com/dreamsxin/srs.git
cd srs/trunk
./configure --with-ffmpeg && make -j4

sudo make install
sudo ln -sf /usr/local/srs/etc/init.d/srs /etc/init.d/srs
sudo update-rc.d srs defaults
```

# 配置拉取第三方流 hls
`conf/hls.conf`
```conf
listen              1935;
max_connections     1000;
vhost __defaultVhost__ {
    ingest demo {
        enabled      on;
        input {
            type    stream;
            url     rtmp://192.168.1.239:1945/live/demo;
        }
        ffmpeg      ./objs/ffmpeg/bin/ffmpeg;
        engine {
            enabled         off;
            output          rtmp://192.168.1.239/live/demo;
        }
    }

    hls {
        enabled         on;
        hls_fragment    3;
        hls_window      60;
        hls_on_error    continue; # hls 切片失败后继续
        hls_path        ./objs/nginx/html;
        hls_m3u8_file   [app]/[stream].m3u8;
        hls_ts_file     [app]/[stream]-[seq].ts;
    }
}
```

# 修改 `./etc/init.d/srs`
```shell
CONFIG="./conf/hls.conf"
```

# 启动
```shell
sudo ./objs/nginx/sbin/nginx

./objs/srs -c conf/hls.conf
# or
./etc/init.d/srs start
```

# 查看日志

```shell
tail -f ./objs/srs.log
```

# HTTP 回调

当客户端连接服务器时触发on_connect，会给指定的 url 发送如下信息
```json
{
	"action": "on_connect",
	"client_id": 1985,
	"ip": "192.168.1.10", 
	"vhost": "video.test.com", 
	"app": "live",
	"tcUrl": "rtmp://x/x?key=xxx",
	"pageUrl": "http://x/x.html"
}
```

当客户端发布流时触发on_publish，会给指定的 url 发送如下信息
```json
{
	"action": "on_publish",
	"client_id": 1985,
	"ip": "192.168.1.10", 
	"vhost": "video.test.com", 
	"app": "live",
	"tcUrl": "rtmp://x/x?key=xxx",
	"stream": "livestream"
}
```

当DVR录制关闭一个flv文件时
```json
{
	"action": "on_dvr",
	"client_id": 1985,
	"ip": "192.168.1.10", 
	"vhost": "video.test.com", 
	"app": "live",
	"stream": "livestream",
	"cwd": "/opt",
	"file": "./l.xxx.flv"
}
```

HTTP服务器返回HTTP 200，内容为0时，说明允许，返回其他数字时，将会禁止

比如使用token认证，客户端请求带token的地址：`rtmp://vhost/app?token=xxxx/stream`，在on_connect回调接口中，根据tcUrl值认证。