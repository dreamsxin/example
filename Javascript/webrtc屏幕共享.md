# WebRTC屏幕共享实现指南：使用getDisplayMedia API

## 核心API介绍

`navigator.mediaDevices.getDisplayMedia`是WebRTC技术中用于屏幕共享的核心API，它允许网页应用在获得用户授权后捕获屏幕内容作为媒体流。该方法返回一个Promise，解析为包含视频轨道的MediaStream对象，视频内容来自用户选择的屏幕区域。

## 基本使用方法

以下是实现屏幕共享的基本代码框架：

```html
<!DOCTYPE html>
<html>
<head>
    <title>屏幕共享示例</title>
</head>
<body>
    <video id="screenShare" autoplay playsinline></video>
    <button id="startBtn">开始共享</button>
    
    <script>
        const startBtn = document.getElementById('startBtn');
        const videoElement = document.getElementById('screenShare');
        
        startBtn.addEventListener('click', async () => {
            try {
                const stream = await navigator.mediaDevices.getDisplayMedia({
                    video: {
                        cursor: "always"  // 显示鼠标指针
                    },
                    audio: false  // 是否捕获音频
                });
                videoElement.srcObject = stream;
            } catch (error) {
                console.error('屏幕共享失败:', error);
            }
        });
    </script>
</body>
</html>
```

## 参数配置详解

`getDisplayMedia`方法接受一个可选的`constraints`参数，用于配置捕获选项：

- **video**：配置视频捕获
  - `cursor`：控制是否显示鼠标指针（"always"|"motion"|"never"）
  - `frameRate`：设置帧率
  - 分辨率等参数（但实际分辨率由用户选择决定）

- **audio**：配置音频捕获（可选）
  - `echoCancellation`：回声消除
  - `noiseSuppression`：噪声抑制
  - `sampleRate`：采样率

## 完整实现示例

以下是一个完整的屏幕共享实现，包含错误处理和状态管理：

```javascript
const startScreenShare = async () => {
    if (!navigator.mediaDevices || !navigator.mediaDevices.getDisplayMedia) {
        console.error('您的浏览器不支持屏幕共享功能');
        return;
    }

    const displayMediaOptions = {
        video: {
            cursor: "always",
            frameRate: { ideal: 30, max: 60 }
        },
        audio: {
            echoCancellation: true,
            noiseSuppression: true,
            sampleRate: 44100
        }
    };

    try {
        const stream = await navigator.mediaDevices.getDisplayMedia(displayMediaOptions);
        
        // 处理媒体流
        const videoTrack = stream.getVideoTracks();
        videoTrack.onended = () => {
            console.log('用户停止了屏幕共享');
            // 执行清理操作
        };
        
        return stream;
    } catch (err) {
        console.error('屏幕共享错误:', err.name, err.message);
        throw err;
    }
};
```

## 实现本地屏幕共享

```html
<!DOCTYPE html>
<html>
<head>
    <title>本地屏幕共享(带STUN服务器)</title>
    <style>
        .container { display: flex; flex-direction: column; gap: 10px; }
        video { width: 100%; max-width: 600px; border: 1px solid #ccc; }
        textarea { width: 100%; height: 100px; }
        .status { padding: 10px; background: #f0f0f0; }
        .error { color: red; }
    </style>
</head>
<body>
    <div class="container">
        <div class="status" id="connectionStatus">未连接</div>
        
        <div>
            <h3>本地信令(复制给对方)</h3>
            <textarea id="localSignal" readonly></textarea>
            <button onclick="createOffer()">生成Offer</button>
            <button onclick="stopSharing()">停止共享</button>
        </div>
        
        <div>
            <h3>远程信令(粘贴对方的)</h3>
            <textarea id="remoteSignal"></textarea>
            <button onclick="applySignal()">应用信令</button>
        </div>
        
        <video id="localScreen" autoplay muted></video>
        <video id="remoteScreen" autoplay></video>
    </div>

    <script>
        let pc;
        let screenStream;
        
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
            
            pc.ontrack = (event) => {
                document.getElementById('remoteScreen').srcObject = event.streams[0];
            };
        }
        
        function updateStatus(state) {
            const statusElement = document.getElementById('connectionStatus');
            statusElement.textContent = `连接状态: ${state}`;
            statusElement.className = `status ${state === 'connected' ? 'connected' : ''}`;
        }
        
        async function startScreenShare() {
            try {
                screenStream = await navigator.mediaDevices.getDisplayMedia({
                    video: { cursor: "always" },
                    audio: false
                });
                
                document.getElementById('localScreen').srcObject = screenStream;
                screenStream.getTracks().forEach(track => {
                    pc.addTrack(track, screenStream);
                });
                
                screenStream.getTracks().forEach(track => {
                    track.onended = () => {
                        stopSharing();
                    };
                });
                
                return true;
            } catch (err) {
                console.error('屏幕共享错误:', err);
                document.getElementById('connectionStatus').textContent = `错误: ${err.message}`;
                return false;
            }
        }
        
        async function createOffer() {
            initConnection();
            if (!await startScreenShare()) return;
            
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
                document.getElementById('connectionStatus').textContent = `错误: ${err.message}`;
            }
        }
        
        async function applySignal() {
            try {
                const signal = JSON.parse(document.getElementById('remoteSignal').value);
                
                if (signal.sdp) {
                    if (signal.type === 'offer') {
                        initConnection();
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
                document.getElementById('connectionStatus').textContent = `错误: ${err.message}`;
            }
        }
        
        function stopSharing() {
            if (screenStream) {
                screenStream.getTracks().forEach(track => track.stop());
                document.getElementById('localScreen').srcObject = null;
            }
            if (pc) {
                pc.close();
                pc = null;
            }
            updateStatus('closed');
            document.getElementById('localSignal').value = '';
        }
    </script>
</body>
</html>
```

## 常见问题与解决方案

1. **权限问题**：用户必须明确授权才能进行屏幕捕获
   - 解决方案：确保在用户交互（如点击事件）中调用API

2. **浏览器兼容性**：
   - 支持Chrome、Edge、Firefox等现代浏览器
   - 移动端支持有限

3. **错误处理**：
   - `NotAllowedError`：用户拒绝了权限请求
   - `NotFoundError`：没有可用的屏幕源
   - `AbortError`：硬件问题导致失败

4. **Chrome特定问题**：
   - 某些版本可能存在滚动条显示问题
   - 解决方案：调整页面样式或等待浏览器更新

## 高级应用场景

1. **结合WebRTC进行实时传输**：
   - 将获取的MediaStream用于RTCPeerConnection
   - 实现点对点屏幕共享

2. **屏幕录制**：
   - 使用MediaRecorder API录制屏幕流
   - 保存为视频文件

3. **选择性共享**：
   - 通过配置参数控制共享整个屏幕、特定窗口或浏览器标签页

## 最佳实践建议

1. 始终在用户交互（如点击事件）中触发屏幕共享请求
2. 提供清晰的用户界面说明共享的内容和目的
3. 正确处理媒体流和轨道，避免内存泄漏
4. 考虑添加替代方案以处理不兼容的浏览器
5. 测试不同浏览器和设备上的行为差异

通过合理使用`navigator.mediaDevices.getDisplayMedia`API，开发者可以轻松实现强大的屏幕共享功能，为远程协作、在线教育等应用场景提供核心技术支持。

引用链接：
1.[navigator.mediaDevices API 教程 - CSDN博客](https://blog.csdn.net/wscfan/article/details/146190383)
2.[js利用浏览器的api写的在线录屏工具_navigator.mediadevices.getdisplaymedia-CSDN博客 - CSDN博客](https://blog.csdn.net/mochu126621456/article/details/143921361)
3.[web 技术分享| WebRTC 实现屏幕共享 - 专业技术博客社区](https://xie.infoq.cn/article/8f16fde4e43302dc47b6fe6db)
4.[navigator.mediaDevices.getDisplayMedia指定显示录制当前标签页 - 世界险恶你要内心强大 - 博客园 - 博客园](https://www.cnblogs.com/sjruxe/p/18649371)
5.[desktopCapturer - Electron官网](https://www.electronjs.org/zh/docs/latest/api/desktop-capturer)
6.[使用getDisplayMedia实现在Chrome中屏幕共享 - WebRTC中文网](https://webrtc.org.cn/?p=1774)
7.[WebRTC一对一单聊与屏幕共享的功能实现(附实现源码)_服务器 webrtc 代码-CSDN博客 - CSDN博客](https://blog.csdn.net/qq_74868753/article/details/148355296)
8.[基于WebRTC 的一对一屏幕共享项目(一)——项目简介 - CSDN博客](https://blog.csdn.net/2301_76564925/article/details/148151122)
9.[WebRTC教程—屏幕共享 - WebRTC中文网](https://webrtc.org.cn/webrtc-tutorial-screensharing/)
10.[【js】navigator.mediaDevices.getDisplayMedia实现屏幕共享: - CSDN博客](https://blog.csdn.net/weixin_53791978/article/details/133172842)
11.[js 实现录屏功能  - 掘金开发者社区](https://juejin.cn/post/7386089141368487977)
12.[WebRTC中的MediaDevices API:从基础到进阶的实践探索与第三方库推荐 - 铭韧](https://zhuanlan.zhihu.com/p/24834079718)
13.[webRTC(十一),HR的话扎心了,一招彻底弄懂 - CSDN博客](https://blog.csdn.net/2401_83621918/article/details/136853991)
14.[mediaDevices-getUserMedia-getDisplayMedia - CSDN博客](https://blog.csdn.net/qq_42374676/article/details/121358941)
15.[如何应对Chrome 112版本中的getDisplayMedia问题? - 51CTO](https://www.51cto.com/article/753014.html)
16.[利用getDisplayMedia()在WebRTC上实现屏幕捕获 - CSDN博客](https://blog.csdn.net/weixin_41821317/article/details/111176861)
17.[浅析Web 录屏技术方案与实现 - CSDN博客](https://blog.csdn.net/qq_41581588/article/details/128868343)
18.[如何在网页中使用 js 录屏插件? - JYeontu](https://www.zhihu.com/question/633619247/answer/3530885338)
