# 使用node.js建立WebSocket服务器
```shell
sudo apt-get install nodejs npm
sudo npm config set registry http://registry.npmjs.org/
sudo npm install ws
```

# 创建package.json的文件，内容如下：
```json
{
  "name": "realtime-server",
  "version": "0.0.1",
  "description": "my first realtime server",
  "dependencies": {}
}
```
# 接下来使用npm命令安装express和socket.io
```shell
npm install --save express
npm install --save socket.io
```
安装成功后，应该可以看到工作目录下生成了一个名为node_modules的文件夹，里面分别是express和socket.io。

# 新建一个文件：index.js

```node
var app = require('express')();
var http = require('http').Server(app);
var io = require('socket.io')(http);

app.get('/', function(req, res){
	res.send('<h1>Welcome Realtime Server</h1>');
});

io.on('connection', function(socket){
	console.log('a user connected');
	
	//监听新用户加入
	socket.on('login', function(obj){
		//将新加入用户的唯一标识当作socket的名称，后面退出的时候会用到
		socket.name = obj.userid;
		
		//检查在线列表，如果不在里面就加入
		if(!onlineUsers.hasOwnProperty(obj.userid)) {
			onlineUsers[obj.userid] = obj.username;
			//在线人数+1
			onlineCount++;
		}
		
		//向所有客户端广播用户加入
		io.emit('login', {onlineUsers:onlineUsers, onlineCount:onlineCount, user:obj});
		console.log(obj.username+'加入了聊天室');
	});
	
	//监听用户退出
	socket.on('disconnect', function(){
		//将退出的用户从在线列表中删除
		if(onlineUsers.hasOwnProperty(socket.name)) {
			//退出用户的信息
			var obj = {userid:socket.name, username:onlineUsers[socket.name]};
			
			//删除
			delete onlineUsers[socket.name];
			//在线人数-1
			onlineCount--;
			
			//向所有客户端广播用户退出
			io.emit('logout', {onlineUsers:onlineUsers, onlineCount:onlineCount, user:obj});
			console.log(obj.username+'退出了聊天室');
		}
	});
	
	//监听用户发布聊天内容
	socket.on('message', function(obj){
		//向所有客户端广播发布的消息
		io.emit('message', obj);
		console.log(obj.username+'说：'+obj.content);
	});
  
});

http.listen(3000, function(){
	console.log('listening on *:3000');
});
```

命令行运行node index.js，如果一切顺利，你应该会看到返回的listening on *:3000字样，这说明服务已经成功搭建了。此时浏览器中打开http://localhost:3000应该可以看到正常的欢迎页面。

# 或者使用 ws 组件

```node
console.log("Server started");
var Msg = '';
var WebSocketServer = require('ws').Server, wss = new WebSocketServer({port: 8010});
wss.on('connection', function(ws) {
    ws.on('message', function(message) {
        console.log('Received from client: %s', message);
        ws.send('Server received from client: ' + message);
    });
});
```

命令执行：
```shell
node server.js
```