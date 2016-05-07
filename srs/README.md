# 编译安装

```shell
git clone https://github.com/dreamsxin/srs.git
cd srs/trunk
./configure --with-ffmpeg && make -j4
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