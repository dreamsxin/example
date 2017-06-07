# Websocketd

Websocketd 将标准输入（stdin）作为它的输入，标准输出（stdout）作为它的输出。可以使用任意语言写后台脚本。

## 下载编译

```shell
git clone https://github.com/joewalnes/websocketd.git
cd websocketd
make
```

## 使用 Bash 脚本counter.sh

每个值之间间隔1秒输出

```shell
#!/bin/bash

echo 1
sleep 1

echo 2
sleep 1

echo 3
```

官方提供的每隔0.5秒输出

```shell
#!/bin/bash

# Simple example script that counts to 10 at ~2Hz, then stops.
for ((COUNT = 1; COUNT <= 10; COUNT++))
do
  echo $COUNT
  sleep 0.5
done
```

启动 `websocketd`，指定这个脚本作为服务：
```shell
./websocketd --port=8080 bash ./counter.sh
```

编写客户端 javascript 脚本：
```javascript
var ws = new WebSocket('ws://localhost:8080/');

ws.onmessage = function(event) {
  console.log(event.data);
};
```
官方客户端例子：
```html
<!DOCTYPE html>
<html>
<head>
<title>websocketd count example</title>
<style>
#count {
	font: bold 150px arial;
	margin: auto;
	padding: 10px;
	text-align: center;
}
</style>
</head>
<body>

<div id="count"></div>

<script>
	var ws = new WebSocket('ws://localhost:8080/');
	ws.onopen = function() {
		document.body.style.backgroundColor = '#cfc';
	};
	ws.onclose = function() {
		document.body.style.backgroundColor = null;
	};
	ws.onmessage = function(event) {
		document.getElementById('count').textContent = event.data;
	};
</script>

</body>
</html>
```

每当客户端连接这个服务器，就会执行counter.sh脚本，并将它的输出推送给客户端。

* Bash 脚本读取客户端输入

我们通过 Bash 实现聊天室功能。

```shell
#!/bin/bash

echo "Please enter your name:"; read USER
echo "[$(date)] ${USER} joined the chat" >> chat.log
echo "[$(date)] Welcome to the chat ${USER}!"
# 不断的从 chat.log 读入最新一行
tail -n 0 -f chat.log --pid=$$ | grep --line-buffered -v "] ${USER}>" &
while read MSG; do echo "[$(date)] ${USER}> ${MSG}" >> chat.log; done
```

`tail`：
- -f 循环读取
- --pid=PID 与-f合用，表示在进程 PID 死掉之后结束
`grep`：
- --line-buffered 实现行缓冲
- -v 反向选择，亦即显示出不包含对应字符的

客户端：
```html
<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8"/>
<title>websocketd chat example</title>
</head>
<body>

<div id="messageList"></div>
<input id="message" type="text" />
<button id="sendBtn" type="button">发送</button>
<script>
	var ws = new WebSocket('ws://localhost:8080/');
	ws.onopen = function() {
		document.body.style.backgroundColor = '#cfc';
		document.getElementById('messageList').innerHTML += '已连接<br/>';
	};
	ws.onclose = function() {
		document.body.style.backgroundColor = null;
	};
	ws.onmessage = function(event) {
		document.getElementById('messageList').innerHTML += event.data + '<br/>';
	};
	ws.onerror = function(event) {
		document.getElementById('messageList').innerHTML += '发送异常<br/>';
	};
	document.getElementById('sendBtn').onclick = function(){
		ws.send(document.getElementById('message').value);
		document.getElementById('message').value = '';
	};
</script>

</body>
</html>
```