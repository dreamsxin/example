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
			record_unique on;
			record_path /tmp/vc;

			# Async notify about an flv recorded
			on_record_done http://localhost:8080/record_done;

		}

		location ~ \.mp4$ {
			mp4;
			mp4_buffer_size     1m;
			mp4_max_buffer_size 5m;
		}

		location ~ \.flv$ {
			flv;
		}
	}
}
```

# 实例
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

			#HLS SETTINGS
			hls on;
			hls_path /tmp/hls;
			hls_nested on;
			hls_type live;
			hls_fragment 5s;
			hls_playlist_length 15s;

			hls_variant _high  BANDWIDTH=1200000;
			hls_variant _mid BANDWIDTH=664000;
			hls_variant _low BANDWIDTH=362000;

			exec ffmpeg -i rtmp://localhost/live/$name
			-c:a libfdk_aac -b:a 48k  -c:v libx264 -b:v 128K -f flv rtmp://localhost/hls/$name_low
			-c:a libfdk_aac -b:a 64k  -c:v libx264 -b:v 256k -f flv rtmp://localhost/hls/$name_mid
			-c:a libfdk_aac -b:a 128k -c:v libx264 -b:v 512K -f flv rtmp://localhost/hls/$name_hi;
		}

		application hls {
			live on;

			hls on;
			hls_path /var/www/html/video/hls;
			hls_nested on;

			hls_variant _low BANDWIDTH=160000;
			hls_variant _mid BANDWIDTH=320000;
			hls_variant _hi  BANDWIDTH=640000;
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

	// 自定义样式
	location / {
		add_before_body /.theme/header.html;
		add_after_body /.theme/footer.html;
	}
}
```