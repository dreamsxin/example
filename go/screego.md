# screego

- https://github.com/screego/server

允许分享指定窗口、浏览器标签页、整个屏幕

- 多用户屏幕共享
- 通过 WebRTC 安全传输
- 低延迟/高分辨率
- 通过Docker /单个二进制文件进行简单安装
- 集成TURN服务器，请参阅NAT 遍历

## 其它

- https://github.com/mistweaverco/bananas

以下是关于Screego屏幕共享原理及其使用的浏览器接口的详细分析：

---

### 一、核心架构与技术基础
1. **WebRTC技术栈**  
   Screego基于WebRTC实现实时屏幕共享，其核心流程包括：
   - **信令服务器**：协调P2P连接的建立，交换SDP（会话描述协议）和ICE候选信息。
   - **媒体传输**：使用SRTP（安全实时传输协议）加密视频流，DTLS握手确保密钥安全交换。
   - **NAT穿透**：内置TURN服务器（默认端口3478）解决复杂网络环境下的连接问题。

2. **浏览器端实现**  
   Screego作为基于浏览器的工具，主要依赖以下WebRTC标准API：
   - **`getDisplayMedia`**：捕获用户屏幕或特定窗口的媒体流，支持全屏、应用窗口或浏览器标签页的共享。
   - **`RTCPeerConnection`**：管理P2P连接，处理媒体流的编码、传输及网络协商。
   - **`RTCDataChannel`**（可选）：用于辅助数据传输（如聊天或文件共享）。

---

### 二、屏幕共享的具体实现
#### 1. **屏幕捕获流程**
- **权限请求**：浏览器调用`getDisplayMedia`时，会弹出权限对话框供用户选择共享范围（全屏/窗口/标签页）。
- **流处理**：捕获的媒体流通过`RTCPeerConnection`传输，支持动态调整分辨率与帧率以适应网络状况。
- **特殊处理**：Screego可能通过以下方式优化：
  - **光标显示**：保留鼠标指针（`cursor: always`参数）。
  - **区域选择**：允许用户拖动选择共享区域，而非强制全屏。

#### 2. **多用户协作**
- 通过房间ID管理会话，共享链接可分发至多个参与者。
- 支持实时交互，如主讲者切换共享窗口或调整画质。

---

### 三、关键浏览器接口详解
| **API**               | **作用**                                                                 | **Screego中的使用场景**                     |
|-----------------------|-------------------------------------------------------------------------|--------------------------------------------|
| `getDisplayMedia`     | 获取屏幕/窗口的媒体流                                                     | 用户发起共享时的初始捕获                     |
| `RTCPeerConnection`   | 建立P2P连接，管理媒体流传输                                               | 核心传输通道，处理编码、网络适应及加密         |
| `RTCDataChannel`      | 提供辅助数据传输通道                                                      | 可选功能（如聊天或注释）                     |
| `MediaStream`         | 封装音视频流数据                                                          | 存储和传递捕获的屏幕内容                     |

---

### 四、技术限制与替代方案
1. **Wayland兼容性**  
   - 原生不支持Wayland协议，需依赖XWayland或第三方Hook库。
2. **高级功能缺失**  
   - 无内置远程控制或文件传输，需配合其他工具（如SSH）使用。
3. **替代方案**  
   - **RustDesk**：更适合高实时性需求的P2P场景。
   - **NoMachine**：提供VDI隔离，适合企业级部署。

---

### 五、配置示例（浏览器端）
```javascript
// 示例：Screego可能的客户端代码逻辑
navigator.mediaDevices.getDisplayMedia({ video: true })
  .then(stream => {
    const peerConnection = new RTCPeerConnection(configuration);
    stream.getTracks().forEach(track => peerConnection.addTrack(track, stream));
    // 信令交换及连接建立...
  });
```

---

以上分析基于WebRTC标准实现及开发者社区的技术讨论，具体实现细节可能因Screego版本差异而调整。

引用链接：
1.[Chromium 136 编译指南 - Android 篇:获取源码(五)-CSDN博客 - CSDN博客](https://blog.csdn.net/qqyy_sj/article/details/149266192)
2.[简单屏幕共享 通过web screego windows 生成证书 - CSDN博客](https://blog.csdn.net/nongcunqq/article/details/132425874)
3.[GitHub 创始人资助的开源浏览器「GitHub 热点速览」 - 开源中国](https://my.oschina.net/u/3556271/blog/11417631)
4.[Chrome音视频会议项目实战:WebRTC技术应用 - CSDN博客](https://blog.csdn.net/weixin_42584758/article/details/148321382)
5.[webrtc基本流程与使用 - CSDN博客](https://blog.csdn.net/mabc1234/article/details/130034189)
6.[基于WebRtc和WebSocket实现视频聊天  - 掘金开发者社区](https://juejin.cn/post/7348362217145172006)
7.[MediaDevices: getDisplayMedia() メソッド - MDNWeb文档平台](https://developer.mozilla.org/ja/docs/Web/API/MediaDevices/getDisplayMedia)
8.[媒體投影 | Android media | Android Developers - Android](https://developer.android.google.cn/guide/topics/large-screens/media-projection-large-screens)
9.[Android技术分享| 一行代码实现安卓屏幕采集编码 - InfoQ 写作平台 - 专业技术博客社区](https://xie.infoq.cn/article/667657c27c15756a83017b32c)
10.[电脑出现screencapture报错 - 深空见闻](https://zhidao.baidu.com/question/995904222058117059.html)
11.[安卓APP限制截屏录屏解决方案 - CSDN技术社区](https://huaweicloud.csdn.net/64edaa9c2ea0282871eab3a1.html)
12.[自荐:开源截图工具ScreenCapture:超多控制指令,支持截长图 - CSDN博客](https://blog.csdn.net/weixin_41204578/article/details/149182968)
