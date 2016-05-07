# 安装


```shell
git clone http://luajit.org/git/luajit-2.0.git
cd luajit && make -j4 && sudo make install && cd ..
```

```shell
git clone https://github.com/MonaSolutions/MonaServer.git
cd MonaServer && make -j4
```

# 配置 `MonaServer.ini` 可以省略
```ini
;MonaServer.ini
socketBufferSize = 114688
[RTMFP]
port = 1985
keepAlivePeer = 10
keepAliveServer = 15
[logs]
name=log
directory=logs
```

# 运行
假如推送的地址是 `rtmp://localhost/live` 需要建立目录 `www/live`
```shell
sudo ./MonaServer
sudo ./MonaServer --daemon [--pidfile=/var/run/MonaServer.pid]
```

# 通过 Lua 录制视频

新增文件 `www/live/main.lua`

## 自动录制

```lua
function onConnection(client)
	
	INFO("Connection of client from address ", client.address)
	
	function client:onPublish(publication)
		
		-- ************** DUMP FLV ****************	
		local pathFile = mona:absolutePath(path) .. "dump_" .. publication.name .. ".mp4"
		NOTE("Begin dumping file 'dump_" , publication.name , ".mp4'")
		publication.file = io.open(pathFile, "wb")
		publication.flvWriter = mona:createMediaWriter("mp4") --  保存为 ts 文件，也可以保存为 flv
		
		function publication:onVideo(time,packet)
			publication:write(2, time, packet)
		end
		
		function publication:onAudio(time,packet)
			publication:write(1, time, packet)
		end
		
		-- MediaContainer FLV maker
		function publication:write(amfType, time, packet)
			
			local flvData = publication.flvWriter:write(amfType, time, packet)
			publication.file:write(flvData)
		end
		
	end
	
	function client:onUnpublish(publication)
		if publication.file then
			NOTE("End dumping file 'dump_" , publication.name , ".mp4'")
			publication.file:close()
		end
	end
	return {index="VideoPublisher.html"}
end
```

## 通过客户端来控制是否录制

```lua
function onConnection(client)
	
	INFO("Connection of client from address ", client.address)
	
	function client.dumpON()
		NOTE("Dump activated")
		client.dump = true
	end
	
	function client:onPublish(publication)
		
		if not client.dump then return end
		
		-- ************** DUMP FLV ****************	
		local pathFile = mona:absolutePath(path) .. "dump_" .. publication.name .. ".ts"
		NOTE("Begin dumping file 'dump_" , publication.name , ".ts'")
		publication.file = io.open(pathFile, "wb")
		publication.flvWriter = mona:createMediaWriter("mp2t")
		
		function publication:onVideo(time,packet)
			publication:write(2, time, packet)
		end
		
		function publication:onAudio(time,packet)
			publication:write(1, time, packet)
		end
		
		-- MediaContainer FLV maker
		function publication:write(amfType, time, packet)
			
			local flvData = publication.flvWriter:write(amfType, time, packet)
			publication.file:write(flvData)
		end
		
	end
	
	function client:onUnpublish(publication)
		if publication.file then
			NOTE("End dumping file 'dump_" , publication.name , ".ts'")
			publication.file:close()
		end
	end
	return {index="VideoPublisher.html"}
end
```

客户端调用
```as3
NetConnection(evt.target).call("dumpON", null);
```

rtmp 的视频流`rtmp://localhost/live/test`，也可以通过 http 方式读取`http://localhost/live/test.flv`， 也可以通过 rtsp 方式读取 `rtsp://localhost/live/test.sdp`


# 使用自带 http 服务

MonaServer本身已经自带了HTTP服务，只需要把网页文件放入“www”目录下即可访问。

# 使用 OpenVSX 转换编码

`bin/vsx -v --capture=rtmp://localhost/live/test --live=8080 --mkvlive --httplive`
sudo ./vsx -v --capture=rtmp://192.168.1.239/live/test2 --live=8080 --mkvlive --httplive

--httplive=8080 or --httplive=http://0.0.0.0:8080
HTTPLive Streaming Server listening address and port string delimited by a colon. An HTTPLive capable client, such as any Apple iOS device (iPhone, iPad) or Safari can access the stream at the URL http[s]://[username:password@][listener-address]:[port]/httplive

--mkvlive=8080 or --mkvlive=http://0.0.0.0:8080
HTTP Matroska / WebM encapsulated live content streaming server listening address and port string delimited by a colon. An HTML5 capable client can access the stream at the URL http[s]://[username:password@][listener-address]:[port]/mkvlive
