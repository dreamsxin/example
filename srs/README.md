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


## 转码保存为mp4
```shell
# main config for srs.
# @see full.conf for detail config.

listen              1935;
max_connections     1000;
srs_log_tank        file;
srs_log_file        ./objs/srs.log;
http_api {
    enabled         on;
    listen          1985;
}
http_server {
    enabled         on;
    listen          8080;
    dir             ./objs/nginx/html;
}
stats {
    network         0;
    disk            sda sdb xvda xvdb;
}
vhost __defaultVhost__ {
	http_hooks {
	        # whether the http hooks enalbe.
        	# default off.
	        enabled         off;
        	#on_publish      http://api.eotu.com:81/video/publish;
		#on_start_dvr	http://localhost/on_start_dvr;
		#on_dvr		http://localhost/on_dvr;
	}
	dvr {
        enabled         on;
        dvr_path        ./objs/nginx/html/live/[app]/[stream].[timestamp].flv;
        dvr_plan        session;
        dvr_duration    30;
        dvr_wait_keyframe on;
        time_jitter full;
	}
	    transcode {
		enabled     on;
		ffmpeg      /usr/local/bin/ffmpeg;
		engine ff {
		    enabled         on;
		    vfilter {
		    }
		    vcodec          libx264;
		    vthreads        4;
		    vprofile        main;
		    vpreset         medium;
		    vparams {
		    }
		    acodec          libvorbis;
		    aparams {
		    }
		    iformat	    flv;
		    oformat         mp4;
		    output          ./objs/nginx/html/[app]/[stream].mp4;
		}
	    }
}

```

# 压力测试

```shell
git clone https://github.com/dreamsxin/srs-bench.git
cd srs-bench
./configure && make
./objs/sb_rtmp_load -c 1 -r rtmp://127.0.0.1:1935/live/test2
```