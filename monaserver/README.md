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