```html

<!DOCTYPE html>
<html>
<head>
    <title>WebRTC共享白板</title>
    <style>
        .container { display: flex; flex-direction: column; gap: 10px; }
        canvas { border: 1px solid #ccc; background: white; }
        .tools { display: flex; gap: 10px; margin-bottom: 10px; }
        textarea { width: 100%; height: 100px; }
        .status { padding: 10px; background: #f0f0f0; }
    </style>
</head>
<body>
    <div class="container">
        <div class="status" id="connectionStatus">未连接</div>
        
        <div class="tools">
            <button onclick="setTool('pen')">画笔</button>
            <button onclick="setTool('eraser')">橡皮擦</button>
            <input type="color" id="colorPicker" onchange="changeColor(this.value)">
            <input type="range" id="brushSize" min="1" max="50" value="5" onchange="changeSize(this.value)">
            <button onclick="clearCanvas()">清空</button>
        </div>
        
        <canvas id="whiteboard" width="800" height="300"></canvas>
        
        <div>
            <h3>本地信令(复制给对方)</h3>
            <textarea id="localSignal" readonly></textarea>
            <button onclick="createOffer()">生成Offer</button>
        </div>
        
        <div>
            <h3>远程信令(粘贴对方的)</h3>
            <textarea id="remoteSignal"></textarea>
            <button onclick="applySignal()">应用信令</button>
        </div>
    </div>

    <script>
        let pc;
        let dataChannel;
        let canvas = document.getElementById('whiteboard');
        let ctx = canvas.getContext('2d');
        let isDrawing = false;
        let currentTool = 'pen';
        let currentColor = '#000000';
        let brushSize = 5;
        let lastX = 0;
        let lastY = 0;
        
        // 初始化绘图事件
        canvas.addEventListener('mousedown', startDrawing);
        canvas.addEventListener('mousemove', draw);
        canvas.addEventListener('mouseup', stopDrawing);
        canvas.addEventListener('mouseout', stopDrawing);
        
        // 工具设置函数
        function setTool(tool) {
            currentTool = tool;
        }
        
        function changeColor(color) {
            currentColor = color;
        }
        
        function changeSize(size) {
            brushSize = parseInt(size);
        }
        
        function clearCanvas() {
            ctx.clearRect(0, 0, canvas.width, canvas.height);
            sendDrawingData({ type: 'clear' });
        }
        
        // 绘图函数
        function startDrawing(e) {
            isDrawing = true;
            [lastX, lastY] = [e.offsetX, e.offsetY];
        }
        
        function draw(e) {
            if (!isDrawing) return;
            
            ctx.beginPath();
            ctx.moveTo(lastX, lastY);
            ctx.lineTo(e.offsetX, e.offsetY);
            
            if (currentTool === 'pen') {
                ctx.strokeStyle = currentColor;
                ctx.lineWidth = brushSize;
                ctx.lineCap = 'round';
                ctx.stroke();
                
                // 发送绘图数据
                sendDrawingData({
                    type: 'draw',
                    tool: 'pen',
                    color: currentColor,
                    size: brushSize,
                    points: [[lastX, lastY], [e.offsetX, e.offsetY]]
                });
            } else if (currentTool === 'eraser') {
                ctx.strokeStyle = '#FFFFFF';
                ctx.lineWidth = brushSize;
                ctx.stroke();
                
                sendDrawingData({
                    type: 'draw',
                    tool: 'eraser',
                    size: brushSize,
                    points: [[lastX, lastY], [e.offsetX, e.offsetY]]
                });
            }
            
            [lastX, lastY] = [e.offsetX, e.offsetY];
        }
        
        function stopDrawing() {
            isDrawing = false;
        }
        
        // WebRTC相关函数
        function initConnection() {
            const configuration = {
                iceServers: [
                    { urls: "stun:stun.l.google.com:19302" },
                    { urls: "stun:global.stun.twilio.com:3478" }
                ]
            };
            
            pc = new RTCPeerConnection();
            
            pc.onicecandidate = (event) => {
                if (event.candidate) {
                    const currentSignal = JSON.parse(document.getElementById('localSignal').value || '{}');
                    currentSignal.iceCandidates = currentSignal.iceCandidates || [];
                    currentSignal.iceCandidates.push(event.candidate);
                    document.getElementById('localSignal').value = JSON.stringify(currentSignal, null, 2);
                }
            };
            
            pc.onconnectionstatechange = () => {
                updateStatus(pc.connectionState);
            };
        }
        
        function updateStatus(state) {
            document.getElementById('connectionStatus').textContent = `连接状态: ${state}`;
        }
        
        async function createOffer() {
            initConnection();
            
            // 创建数据通道
            dataChannel = pc.createDataChannel('whiteboard');
            dataChannel.onmessage = handleRemoteDrawing;
            dataChannel.onopen = () => updateStatus('已连接');
            dataChannel.onclose = () => updateStatus('连接关闭');

			pc.channel = dataChannel;
            try {
                const offer = await pc.createOffer();
                await pc.setLocalDescription(offer);
                document.getElementById('localSignal').value = JSON.stringify({
                    sdp: offer.sdp,
                    type: offer.type,
                    iceCandidates: []
                }, null, 2);
            } catch (err) {
                console.error('创建Offer错误:', err);
                updateStatus(`错误: ${err.message}`);
            }
        }
        
        async function applySignal() {
            try {
                const signal = JSON.parse(document.getElementById('remoteSignal').value);
                
                if (signal.sdp) {
                    if (!pc) {
						initConnection();

						pc.ondatachannel= e => {
							const receiveChannel = e.channel;
							receiveChannel.onmessage = e =>  {console.log("messsage received!!!"  + e.data ); handleRemoteDrawing(event);}
							receiveChannel.onopen = () => updateStatus('已连接');
							receiveChannel.onclose = () => updateStatus('连接关闭');
							pc.channel = receiveChannel;
						}
					}
                    
                    if (signal.type === 'offer') {
                        await pc.setRemoteDescription(new RTCSessionDescription(signal));
                        const answer = await pc.createAnswer();
                        await pc.setLocalDescription(answer);
                        document.getElementById('localSignal').value = JSON.stringify({
                            sdp: answer.sdp,
                            type: answer.type,
                            iceCandidates: []
                        }, null, 2);
                    } else if (signal.type === 'answer') {
                        await pc.setRemoteDescription(new RTCSessionDescription(signal));
                    }
                }
                
                if (signal.iceCandidates) {
                    for (const candidate of signal.iceCandidates) {
                        await pc.addIceCandidate(new RTCIceCandidate(candidate));
                    }
                }
            } catch (err) {
                console.error('应用信令错误:', err);
                updateStatus(`错误: ${err.message}`);
            }
        }
        
        // 绘图数据传输
        function sendDrawingData(data) {
            if (dataChannel && dataChannel.readyState === 'open') {
				console.log(data);
                dataChannel.send(JSON.stringify(data));
            }
        }
        
        function handleRemoteDrawing(event) {
            const data = JSON.parse(event.data);
            console.log(data);
            if (data.type === 'draw') {
                ctx.beginPath();
                ctx.moveTo(data.points[0][0], data.points[0][1]);
                ctx.lineTo(data.points[1][0], data.points[1][1]);
                
                if (data.tool === 'pen') {
                    ctx.strokeStyle = data.color;
                    ctx.lineWidth = data.size;
                } else {
                    ctx.strokeStyle = '#FFFFFF';
                    ctx.lineWidth = data.size;
                }
                
                ctx.lineCap = 'round';
                ctx.stroke();
            } else if (data.type === 'clear') {
                ctx.clearRect(0, 0, canvas.width, canvas.height);
            }
        }
    </script>
</body>
</html>
```
