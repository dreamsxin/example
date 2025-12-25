# Frida 使用教程

## 一、Frida 简介

Frida 是一个动态代码插桩工具，允许你将 JavaScript 代码或你自己的库注入到 Windows、macOS、Linux、iOS、Android 和 QNX 的本地应用程序中。

## 二、安装

### 1. 安装基础组件
```bash
# 安装 Python 包
pip install frida-tools

# 或者直接安装 Frida
pip install frida

# 查看版本
frida --version
```

### 2. 下载 Frida Server（针对移动设备）

根据你的设备架构从 [GitHub Releases](https://github.com/frida/frida/releases) 下载对应的 frida-server：

```bash
# 示例：下载 Android arm64 版本
wget https://github.com/frida/frida/releases/download/16.1.4/frida-server-16.1.4-android-arm64.xz
unxz frida-server-16.1.4-android-arm64.xz
```

## 三、Android 设备设置

### 1. 推送并运行 frida-server
```bash
# 将 frida-server 推送到设备
adb push frida-server /data/local/tmp/
adb shell chmod 755 /data/local/tmp/frida-server

# 运行 frida-server
adb shell
su
cd /data/local/tmp
./frida-server &
```

### 2. 端口转发
```bash
adb forward tcp:27042 tcp:27042
adb forward tcp:27043 tcp:27043
```

## 四、基本使用

### 1. 列出进程
```bash
# USB 连接的设备
frida-ps -U

# 远程设备
frida-ps -H 192.168.1.100:27042

# 本地计算机
frida-ps
```

### 2. 附加到进程
```bash
# 按名称附加
frida -U com.example.app

# 按 PID 附加
frida -U 1234

# 附加并运行脚本
frida -U com.example.app -l script.js
```

### 3. 生成进程
```bash
frida -U -f com.example.app
```

## 五、JavaScript API 基础

### 1. 基本结构
```javascript
Java.perform(function() {
    // 你的代码在这里执行
    console.log("Hello from Frida!");
});
```

### 2. Hook Java 方法
```javascript
Java.perform(function() {
    // 获取类
    var MyClass = Java.use("com.example.MyClass");
    
    // Hook 方法
    MyClass.myMethod.implementation = function(param1, param2) {
        console.log("myMethod called with:", param1, param2);
        
        // 调用原方法
        var result = this.myMethod(param1, param2);
        
        // 修改返回值
        return result + " modified";
    };
});
```

### 3. 修改字段值
```javascript
Java.perform(function() {
    var MyClass = Java.use("com.example.MyClass");
    
    // 修改静态字段
    MyClass.staticField.value = "new value";
    
    // 修改非静态字段（需要实例）
    Java.choose("com.example.MyClass", {
        onMatch: function(instance) {
            instance.nonStaticField.value = "new value";
        },
        onComplete: function() {}
    });
});
```

### 4. 创建新实例并调用方法
```javascript
Java.perform(function() {
    var MyClass = Java.use("com.example.MyClass");
    
    // 创建新实例
    var instance = MyClass.$new();
    
    // 调用方法
    var result = instance.someMethod("test");
    console.log("Result:", result);
});
```

## 六、Native 层 Hook

### 1. Hook 导出函数
```javascript
Interceptor.attach(Module.getExportByName("libnative.so", "native_function"), {
    onEnter: function(args) {
        console.log("native_function called");
        console.log("arg0:", args[0].readCString());
    },
    onLeave: function(retval) {
        console.log("Return value:", retval);
    }
});
```

### 2. Hook 任意地址
```javascript
var base = Module.getBaseAddress("libnative.so");
var offset = 0x1234;
var funcPtr = base.add(offset);

Interceptor.attach(funcPtr, {
    onEnter: function(args) {
        console.log("Function at offset 0x1234 called");
    }
});
```

## 七、实用脚本示例

### 1. 绕过 SSL Pinning
```javascript
Java.perform(function() {
    var Certificate = Java.use("java.security.cert.Certificate");
    var X509Certificate = Java.use("java.security.cert.X509Certificate");
    
    // Hook checkClientTrusted
    var checkClientTrusted = Java.use("javax.net.ssl.X509TrustManager").checkClientTrusted.overload(
        '[Ljava.security.cert.Certificate;', 'java.lang.String'
    );
    checkClientTrusted.implementation = function(certs, authType) {
        console.log("Bypassing checkClientTrusted");
    };
    
    // Hook checkServerTrusted
    var checkServerTrusted = Java.use("javax.net.ssl.X509TrustManager").checkServerTrusted.overload(
        '[Ljava.security.cert.Certificate;', 'java.lang.String'
    );
    checkServerTrusted.implementation = function(certs, authType) {
        console.log("Bypassing checkServerTrusted");
    };
});
```

### 2. 监控加密操作
```javascript
Java.perform(function() {
    var Cipher = Java.use("javax.crypto.Cipher");
    
    Cipher.doFinal.overload("[B").implementation = function(input) {
        console.log("Cipher.doFinal called");
        console.log("Input:", JSON.stringify(input));
        
        var result = this.doFinal(input);
        console.log("Output:", JSON.stringify(result));
        
        return result;
    };
});
```

### 3. 拦截 HTTP 请求
```javascript
Java.perform(function() {
    var URL = Java.use("java.net.URL");
    var HttpURLConnection = Java.use("java.net.HttpURLConnection");
    
    var openConnection = URL.openConnection.overload();
    openConnection.implementation = function() {
        var connection = this.openConnection();
        console.log("URL:", this.toString());
        
        if (connection) {
            var urlConnection = Java.cast(connection, HttpURLConnection);
            
            // 添加请求头
            urlConnection.setRequestProperty("User-Agent", "Frida-Agent");
            
            // 拦截响应
            var originalGetInputStream = urlConnection.getInputStream;
            urlConnection.getInputStream = function() {
                console.log("Request Method:", this.getRequestMethod());
                console.log("Response Code:", this.getResponseCode());
                return originalGetInputStream.call(this);
            };
        }
        
        return connection;
    };
});
```

## 八、Python 集成

### 1. 基本 Python 脚本
```python
import frida
import sys

def on_message(message, data):
    if message['type'] == 'send':
        print(f"[*] {message['payload']}")
    else:
        print(message)

# 连接到设备
device = frida.get_usb_device()

# 附加到进程
session = device.attach("com.example.app")

# 加载 JavaScript 代码
with open("script.js", "r") as f:
    jscode = f.read()

script = session.create_script(jscode)
script.on('message', on_message)

print("[*] Running script")
script.load()

# 保持运行
sys.stdin.read()
```

### 2. 生成进程并注入
```python
import frida
import time

device = frida.get_usb_device()

# 生成新进程
pid = device.spawn(["com.example.app"])
session = device.attach(pid)

# 加载脚本
with open("script.js", "r") as f:
    jscode = f.read()

script = session.create_script(jscode)

def on_message(message, data):
    print(message)

script.on('message', on_message)
script.load()

# 恢复进程运行
device.resume(pid)

# 等待
time.sleep(10)
```

## 九、Frida-trace 工具

### 1. 跟踪函数调用
```bash
# 跟踪特定函数
frida-trace -U com.example.app -i "open"
frida-trace -U com.example.app -i "*read*"

# 跟踪 Objective-C 方法
frida-trace -U com.example.app -m "-[NSURL *]"

# 跟踪 Java 方法
frida-trace -U com.example.app -j "java.net.URL.openConnection"
```

### 2. 自定义跟踪处理程序
```bash
frida-trace -U com.example.app -i "open" -p handlers/open.js
```

handlers/open.js 内容：
```javascript
onEnter: function(log, args, state) {
    log("open called with path: " + Memory.readCString(args[0]));
}
```

## 十、实用技巧

### 1. 搜索内存
```javascript
// 搜索字符串
var results = Memory.scanSync(Module.getBaseAddress("libtarget.so"), 
                             Module.getBaseSize("libtarget.so"), 
                             "41 42 43 ?? 45");

// 搜索并替换
Memory.scan(Module.getBaseAddress("libtarget.so"), 
           Module.getBaseSize("libtarget.so"), 
           "41 42 43", {
    onMatch: function(address, size) {
        console.log("Found at:", address);
        Memory.writeByteArray(address, [0x44, 0x45, 0x46]);
    },
    onComplete: function() {}
});
```

### 2. 枚举模块和导出
```javascript
// 枚举所有加载的模块
Process.enumerateModules().forEach(function(module) {
    console.log("Module:", module.name, "Base:", module.base);
    
    // 枚举导出函数
    module.enumerateExports().forEach(function(export) {
        console.log("  Export:", export.name, "at", export.address);
    });
    
    // 枚举导入函数
    module.enumerateImports().forEach(function(import) {
        console.log("  Import:", import.name, "from", import.module);
    });
});
```

### 3. 处理多线程
```javascript
Java.perform(function() {
    // 在主线程执行
    Java.scheduleOnMainThread(function() {
        console.log("Running on main thread");
    });
});
```

### 4. 发送消息到 Python
```javascript
// JavaScript 中
send({type: "data", payload: "Hello from JS"});

// Python 中接收
def on_message(message, data):
    if message['type'] == 'send':
        print("Received:", message['payload'])
```

## 十一、故障排除

### 常见问题解决

1. **连接失败**
   ```bash
   # 检查设备连接
   adb devices
   
   # 重启 frida-server
   adb shell killall frida-server
   ```

2. **版本不匹配**
   ```bash
   # 确保 frida-tools 和 frida-server 版本一致
   pip install frida-tools==16.1.4
   ```

3. **权限问题**
   ```bash
   # 确保设备已 root
   adb root
   
   # 重新挂载为可写
   adb remount
   ```

## 十二、学习资源

1. **官方文档**: https://frida.re/docs/
2. **API 参考**: https://frida.re/docs/javascript-api/
3. **示例仓库**: https://github.com/frida/frida-agent-example
4. **社区脚本**: https://codeshare.frida.re/

这个教程涵盖了 Frida 的基本到中级用法。建议从简单的脚本开始，逐步尝试更复杂的功能。实践是最好的学习方式！
