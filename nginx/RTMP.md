# nginx_rtmp_module

```conf
rtmp {
	server {
		listen 1935;
		chunk_size 4000;

		application vod {
		    play /var/flvs;
		}

		application vod2 {
		    play /var/mp4s;
		}

		application videochat {
			live on;

			# Make HTTP request & use HTTP retcode
			# to decide whether to allow publishing
			# from this connection or not
			on_publish http://localhost:8080/publish;

			# Same with playing
			on_play http://localhost:8080/play;

			# Publish/play end (repeats on disconnect)
			on_done http://localhost:8080/done;

			record all;
			# 同一个 name 录制文件，是否添加时间戳
			record_unique off;
			# 同一个 name 录制文件，是否追加，还是覆盖
                        record_append on
			record_path /tmp/vc;

			# Async notify about an flv recorded
			on_record_done http://localhost:8080/record_done;

			# track client info
			exec_play bash -c "echo $addr $pageurl >> /tmp/clients";
			exec_publish bash -c "echo $addr $flashver >> /tmp/publishers";

			# convert recorded file to mp4 format
			exec_record_done ffmpeg -y -i $path -acodec libmp3lame -ar 44100 -ac 1 -vcodec libx264 $dirname/$basename.mp4;
		}

		application live {
                        live on;

                        push rtmp://w.gslb.lecloud.com/live;

                        exec /usr/local/bin/ffmpeg -i rtmp://localhost/live/$name
                        -c:a libfdk_aac -b:a 44k -c:v libx264 -b:v 128K -g 30 -f flv rtmp://localhost/hls/$name_low
                        -c:a libfdk_aac -b:a 64k -c:v libx264 -b:v 256k -g 30 -f flv rtmp://localhost/hls/$name_mid
                        -c:a libfdk_aac -b:a 128k -c:v libx264 -b:v 512k -g 30 -f flv rtmp://localhost/hls/$name_high;
                }

		location ~ \.mp4$ {
			mp4;
			mp4_buffer_size     1m;
			mp4_max_buffer_size 5m;
		}

		location ~ \.flv$ {
			flv;
		}

		access_log /var/log/nginx/rtmp_access.log;
	}
}
```

## 实例
```conf
rtmp {
	server {
		listen 1935;
		chunk_size 4000;

		application flv {
		    play /var/www/html/video/flv;
		}

		application mp4 {
		    play /var/www/html/video/mp4;
		}

		application live {
			live on;
			// ffmpeg 要开启 --enable-libfdk_aac

			exec /usr/local/bin/ffmpeg -i rtmp://localhost/live/$name
			-c:a libfdk_aac -b:a 32k  -c:v libx264 -b:v 128K -f flv rtmp://localhost/hls/$name_low 
			-c:a libfdk_aac -b:a 64k  -c:v libx264 -b:v 256k -f flv rtmp://localhost/hls/$name_mid 
			-c:a libfdk_aac -b:a 128k -c:v libx264 -b:v 512k -f flv rtmp://localhost/hls/$name_norm 
			-c:a libfdk_aac -b:a 128k -c:v libx264 -b:v 1000k -f flv rtmp://localhost/hls/$name_high;

			exec /usr/local/bin/ffmpeg -i rtmp://localhost/live/$name
			-c:a copy -c:v libx264 -b:v 128K -g 30 -f flv rtmp://localhost/hls/$name_low 
			-c:a copy -c:v libx264 -b:v 256k -g 30 -f flv rtmp://localhost/hls/$name_mid 
			-c:a copy -c:v libx264 -b:v 512k -g 30 -f flv rtmp://localhost/hls/$name_high;
		}

		application hls {
			live on;

			hls on;
			hls_path /var/www/html/video/hls;
			hls_nested on;

			hls_variant _low BANDWIDTH=160000,CODECS=H264;
			hls_variant _mid BANDWIDTH=320000,CODECS=H264;
			hls_variant _high BANDWIDTH=640000,CODECS=H264;
			#hls_variant _norm BANDWIDTH=640000;
			#hls_variant _high BANDWIDTH=1128000;
		}

		application encoder {
			live on;

			wait_video on;

			exec ffmpeg -i rtmp://localhost/encoder/$name
			-c:a libfdk_aac -b:a 128k -c:v libx264 -b:v 2500k -f flv -g 30 -r 30 -s 1280x720 -preset superfast -profile:v baseline rtmp://localhost/hls2/$name_720p2628kbs
			-c:a libfdk_aac -b:a 128k -c:v libx264 -b:v 1000k -f flv -g 30 -r 30 -s 854x480 -preset superfast -profile:v baseline rtmp://localhost/hls2/$name_480p1128kbs
			-c:a libfdk_aac -b:a 128k -c:v libx264 -b:v 750k -f flv -g 30 -r 30 -s 640x360 -preset superfast -profile:v baseline rtmp://localhost/hls2/$name_360p878kbs
			-c:a libfdk_aac -b:a 128k -c:v libx264 -b:v 400k -f flv -g 30 -r 30 -s 426x240 -preset superfast -profile:v baseline rtmp://localhost/hls2/$name_240p528kbs
			-c:a libfdk_aac -b:a 64k -c:v libx264 -b:v 200k -f flv -g 15 -r 15 -s 426x240 -preset superfast -profile:v baseline rtmp://localhost/hls2/$name_240p264kbs;
		}

		application hls2 {
			live on;
			hls on;
			hls_fragment_naming system;
			hls_fragment 5s;
			hls_path HTTP_DOCUMENTROOT/data/hls;
			hls_nested on;

			hls_variant _720p2628kbs BANDWIDTH=2628000,RESOLUTION=1280x720;
			hls_variant _480p1128kbs BANDWIDTH=1128000,RESOLUTION=854x480;
			hls_variant _360p878kbs BANDWIDTH=878000,RESOLUTION=640x360;
			hls_variant _240p528kbs BANDWIDTH=528000,RESOLUTION=426x240;
			hls_variant _240p264kbs BANDWIDTH=264000,RESOLUTION=426x240;
		}

		application live2 {

			live on;

			hls on;
                        #hls_continuous on;
                        hls_fragment 5s;
                        hls_playlist_length 30s;
                        hls_path /var/www/html/video/hls;


			# Make HTTP request & use HTTP retcode
			# to decide whether to allow publishing
			# from this connection or not
			#on_publish http://localhost:8080/publish;

			# Same with playing
			#on_play http://localhost:8080/play;

			# Publish/play end (repeats on disconnect)
			#on_done http://localhost:8080/done;

			record all;
			record_unique on;
			record_path /var/www/html/video/flv;

			# Async notify about an flv recorded
			#on_record_done http://localhost:8080/record_done;
		}
		
		application myapp {
			live on;
			recorder rec1 {
				record all manual;
				record_suffix all.flv;
				record_path /tmp/rec;
				record_unique on;
			}
		}

	}
}
# or 
rtmp {
        server {
                listen 1935;

                application flv {
                    play /var/www/html/video/flv;
                }

                application mp4 {
                    play /var/www/html/video/mp4;
                }

                application stream {
                        live on;
                }

                application live {
                        live on;

                        # Make HTTP request & use HTTP retcode
                        # to decide whether to allow publishing
                        # from this connection or not
                        #on_publish http://localhost:8080/publish;

                        # Same with playing
                        #on_play http://localhost:8080/play;

                        # Publish/play end (repeats on disconnect)
                        #on_done http://localhost:8080/done;

                        #record all;
                        #record_unique on;
                        #record_path /var/www/html/video/flv;

                        # Async notify about an flv recorded
                        #on_record_done http://localhost:8080/record_done;

                        #exec /usr/local/bin/ffmpeg -i rtmp://localhost/live/$name -c:a libfdk_aac -b:a 32k  -c:v libx264 -b:v 128K -f flv rtmp://localhost/hls/$name_low -c:a libfdk_aac -b:a 64k -c:v libx264 -b:v 256k -f flv rtmp://localhost/hls/$name_mid -c:a libfdk_aac -b:a 128k -c:v libx264 -b:v 512k -f flv rtmp://localhost/hls/$name_norm -c:a libfdk_aac -b:a 128k -c:v libx264 -b:v 1000k -f flv rtmp://localhost/hls/$name_high;
                        exec /usr/local/bin/ffmpeg -i rtmp://localhost/live/$name
                        -c:a libfdk_aac -b:a 44k -c:v libx264 -b:v 128K -g 30 -f flv rtmp://localhost/hls/$name_low
                        -c:a libfdk_aac -b:a 64k -c:v libx264 -b:v 256k -g 30 -f flv rtmp://localhost/hls/$name_mid
                        -c:a libfdk_aac -b:a 128k -c:v libx264 -b:v 512k -g 30 -f flv rtmp://localhost/hls/$name_high;
                }

		application show {
                        live on;
                        pull rtmp://192.168.1.108/live;
                }


                application hls {
                    live on;

                    hls on;
                    hls_path /var/www/html/video/hls;
                    hls_nested on;
                        hls_variant _low BANDWIDTH=160000;
                        hls_variant _mid BANDWIDTH=320000;
                        hls_variant _high  BANDWIDTH=640000;
                }

        }
}
```

## 点播配置
```conf
server {
        listen 80 default_server;
        listen [::]:80 default_server;

        root /var/www/html/video;

        # Add index.php to the list if you are using PHP
        index index.html index.htm index.nginx-debian.html;
        charset utf-8;
        server_name _;

        autoindex on;
        autoindex_exact_size on;
        autoindex_localtime on;

	location /hls {
		types {
			application/vnd.apple.mpegusr m3u8;
			video/mp2t ts;
		}
		add_header Cache-Control no-cache;
	}

	location /mp4 {
		mp4;
		mp4_buffer_size     1m;
		mp4_max_buffer_size 5m;
	}

	location /flv {
		flv;
	}

        location ~ \.mp4$ {
                mp4;
                mp4_buffer_size     1m;
                mp4_max_buffer_size 5m;
        }

        location ~ \.flv$ {
                flv;
        }

        location ~ \.ts {
                types {
                        video/mp2t ts;
                }
                #add_header Cache-Control no-cache;
        }

        location ~ \.m3u8 {
                types {
                        application/vnd.apple.mpegusr m3u8;
                }
                #add_header Cache-Control no-cache;
        }

	location ~ \.html {
	}   

	// Record（控制录制开始、结束）、DROP（断开指定推送、订阅）、Redirect（重定向推送或播放流地址）
	// http://server.com/control/record/start|stop?srv=SRV&app=APP&name=NAME&rec=REC
	// curl http://localhost:8080/control/record/start?app=myapp&name=mystream&rec=rec1
	// curl http://localhost:8080/control/record/stop?app=myapp&name=mystream&rec=rec1
	// http://server.com/control/drop/publisher|subscriber|client?srv=SRV&app=APP&name=NAME&addr=ADDR&clientid=CLIENTID
	// curl http://localhost:8080/control/drop/publisher?app=myapp&name=mystream
	// curl http://localhost:8080/control/drop/client?app=myapp&name=mystream
	// curl http://localhost:8080/control/drop/client?app=myapp&name=mystream&addr=192.168.0.1
	// curl http://localhost:8080/control/drop/client?app=myapp&name=mystream&clientid=1
	// http://server.com/control/redirect/publisher|subscriber|client?srv=SRV&app=APP&name=NAME&addr=ADDR&clientid=CLIENTID&newname=NEWNAME
	location /control {
		rtmp_control all;
	}

        # This URL provides RTMP statistics in XML
        location /stat {
            rtmp_stat all;

            # Use this stylesheet to view XML as web page
            # in browser
            rtmp_stat_stylesheet stat.xsl;
        }

        location /stat.xsl {
            # XML stylesheet to view RTMP stats.
            # Copy stat.xsl wherever you want
            # and put the full directory path here
            root /path/to/stat.xsl/;
        }

	// 自定义样式
	location / {
		add_before_body /.theme/header.html;
		add_after_body /.theme/footer.html;
	}
}
```

## 统计

```conf
        location /stat {
            rtmp_stat all;
        }
	//  http://127.0.0.1/statclient?app=app应用名&name=频道名
	location /statclient {
		proxy_pass http://127.0.0.1/stat;
		xslt_stylesheet statclient.xsl app='$arg_app' name='$arg_name';
		add_header Refresh "3; $request_uri";
	}
```

`statclient.xsl`
```xml
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="html"/>

<xsl:param name="app"/>
<xsl:param name="name"/>

<xsl:template match="/">
    <xsl:value-of select="count(//application[name=$app]/live/stream[name=$name]/client[not(publishing) and flashver])"/>
</xsl:template>

</xsl:stylesheet>
```

## 控制 control

http 请求返回结果，成功`1`、失败`0`，在设置了push功能的情况下，无法切换流，在使用了 `exec` 情况下无法使用控制。

```conf
http {
    server {
        listen       8080;
        server_name  localhost;
        location /control {
            rtmp_control all;
        }
     }
}
```

### 录制 Record

This sub-module starts and stops recordings created with manual flag. Syntax:

http://server.com/control/record/start|stop?srv=SRV&app=APP&name=NAME&rec=REC

- srv=SRV - optional server{} block number within rtmp{} block, default to first server{} block
- app=APP - required application name
- name=NAME - required stream name
- rec=REC - optional recorder name, defaults to root (unnamed) recorder

Example：
```conf
rtmp {
    server {
        listen 1935;
        application myapp {
            live on;
            recorder rec1 {
               record all manual;
               record_suffix all.flv;
               record_path /tmp/rec;
               record_unique on;
           }
       }
    }
}
```

```shell
curl "http://localhost:8080/control/record/start?app=myapp&name=mystream&rec=rec1"
curl "http://localhost:8080/control/record/stop?app=myapp&name=mystream&rec=rec1"
```

### 断开客户端 Drop

This sub-module provides a simple way to drop client connection. Syntax:

http://server.com/control/drop/publisher|subscriber|client?srv=SRV&app=APP&name=NAME&addr=ADDR&clientid=CLIENTID

- srv, app, name - the same as above
- addr - optional client address (the same as returned by rtmp_stat)
- clientid - optional nginx client id (displayed in log and stat)

The first method drop/publisher drops publisher connection. The second drop/client drops every connection matching addr argument or all clients (including publisher) if addr is not specified.

Examples：
```shell
curl http://localhost:8080/control/drop/publisher?app=myapp&name=mystream
curl http://localhost:8080/control/drop/client?app=myapp&name=mystream
curl http://localhost:8080/control/drop/client?app=myapp&name=mystream&addr=192.168.0.1
curl http://localhost:8080/control/drop/client?app=myapp&name=mystream&clientid=1
```

### Redirect

Redirect play/publish client to a new stream. Syntax:

http://server.com/control/redirect/publisher|subscriber|client?srv=SRV&app=APP&name=NAME&addr=ADDR&clientid=CLIENTID&newname=NEWNAME

- srv, app, name, addr, clients - the same as above
- newname - new stream name to redirect to

Examples：
```shell
ffmpeg -f alsa -ac 1 -i pulse -acodec aac -f video4linux2  -s 640x360 -i /dev/video0 -acodec libfdk_aac -vcodec libx264 -preset ultrafast -vprofile baseline -vlevel 1.0  -s 640x480 -b:v 800k -r 25  -pix_fmt yuv420p -f flv rtmp://localhost/live/mystream

ffmpeg -f alsa -ac 1 -i pulse -acodec aac -f video4linux2  -s 640x360 -i /dev/video0 -acodec libfdk_aac -vcodec libx264 -preset ultrafast -vprofile baseline -vlevel 1.0  -s 640x480 -b:v 800k -r 25  -pix_fmt yuv420p -f flv rtmp://localhost/live/mystream2

curl http://localhost:8080/control/redirect/publisher?app=myapp&name=mystream&newname=mystream2
```