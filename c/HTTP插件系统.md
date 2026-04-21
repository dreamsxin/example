# 🚀 HTTP 插件系统 v3.0（平台级规范）

---

# 一、设计目标（重新定义）

### ✅ 核心目标

1. **零配置接入（Zero Config）**
2. **自描述（Self-Describing Plugin）**
3. **自动注册（Auto Registration）**
4. **协议标准化（Protocol First）**
5. **SDK 优先（SDK First）**

---

# 二、整体架构（升级版）

```
Client
   │
   ▼
Main Program (Plugin Gateway)
   │
   ├── Plugin Registry（注册中心）
   ├── Manifest Loader（能力发现）
   ├── Reverse Proxy（可选）
   └── SDK Generator（可选）
   │
   ▼
Plugins（任意语言 / HTTP）
```

---

# 三、核心机制（最关键）

---

## 3.1 插件启动 & 自动注册

### ✅ 插件启动后必须执行：

```http
POST /__plugin__/register
```

```json
{
  "id": "com.company.data",
  "endpoint": "http://127.0.0.1:54321"
}
```

---

### ✅ 或 stdout 模式（更简单）

插件启动输出：

```
PLUGIN_READY: http://127.0.0.1:54321
```

---

### 👉 主程序行为：

1. 启动插件进程
2. 等待注册（或解析 stdout）
3. 拉取 manifest
4. 自动挂载路由

---

## 3.2 插件自描述（Manifest）

### ✅ 标准接口（必须）

```http
GET /__plugin__/manifest
```

---

### ✅ 返回结构（核心协议）

```json
{
  "plugin": {
    "id": "com.company.data",
    "name": "数据处理器",
    "version": "1.2.3",
    "description": "高性能数据处理"
  },
  "tools": [
    {
      "name": "clean",
      "description": "数据清洗",
      "method": "POST",
      "path": "/clean",
      "input_schema": {...},
      "output_schema": {...}
    }
  ],
  "health": {
    "path": "/health"
  }
}
```

---

### 🚨 关键变化

| 旧方案                      | 新方案  |
| ------------------------ | ---- |
| plugin.json capabilities | ❌ 删除 |
| tools 静态配置               | ❌ 删除 |
| 主程序定义能力                  | ❌ 删除 |
| 插件自描述                    | ✅ 强制 |

---

# 四、路由规范（完全自动化）

---

## 4.1 标准路由格式

```
/api/plugins/{plugin_id}/{tool_path}
```

---

### 示例：

```
POST /api/plugins/com.company.data/clean
```

---

### 👉 主程序自动完成：

```cpp
route = "/api/plugins/" + plugin_id + tool.path
```

---

### ✅ 好处

* 无冲突
* 无需 route_prefix
* 天然支持多版本

---

# 五、统一调用协议（重要）

---

## 5.1 标准响应格式（强制）

### 成功

```json
{
  "success": true,
  "data": {...}
}
```

---

### 失败

```json
{
  "success": false,
  "error": {
    "code": "INVALID_INPUT",
    "message": "Missing field 'data'",
    "details": {}
  }
}
```

---

## 5.2 HTTP 状态码规范

| 状态码 | 含义     |
| --- | ------ |
| 200 | 成功     |
| 400 | 参数错误   |
| 404 | 工具不存在  |
| 500 | 插件内部错误 |
| 502 | 插件不可用  |
| 504 | 超时     |

---

# 六、主程序（核心实现）

---

## 6.1 插件注册结构

```cpp
struct PluginInstance {
    std::string id;
    std::string endpoint;   // http://127.0.0.1:54321
    std::vector<Tool> tools;
    bool healthy;
};
```

---

## 6.2 自动加载流程

```cpp
LoadPlugins():
  for dir in plugins:
    StartProcess()
    WaitForRegister()
    FetchManifest()
    RegisterRoutes()
```

---

## 6.3 路由转发（核心）

```cpp
server.Post("/api/plugins/(.*)", [&](req, res) {
    auto plugin_id = parsePluginId(req.path);
    auto plugin = registry.Get(plugin_id);

    auto target = plugin.endpoint + stripPrefix(req.path);

    ForwardRequest(target, req, res);
});
```

---

## 6.4 健康检查

```http
GET {plugin.endpoint}/health
```

失败 → 自动重启

---

# 七、插件实现规范（极简版）

---

## 7.1 必须实现的接口

| 接口                     | 必须 |
| ---------------------- | -- |
| `/__plugin__/manifest` | ✅  |
| `/health`              | ✅  |

---

## 7.2 Python 示例（v3 推荐写法）

```python
from flask import Flask, request, jsonify

app = Flask(__name__)

@app.route("/__plugin__/manifest")
def manifest():
    return {
        "plugin": {
            "id": "com.company.data",
            "name": "Data Processor",
            "version": "1.0.0"
        },
        "tools": [
            {
                "name": "clean",
                "method": "POST",
                "path": "/clean",
                "input_schema": {
                    "type": "object",
                    "properties": {
                        "data": {"type": "array"}
                    },
                    "required": ["data"]
                }
            }
        ],
        "health": {"path": "/health"}
    }

@app.route("/health")
def health():
    return "OK"

@app.route("/clean", methods=["POST"])
def clean():
    body = request.json
    data = body.get("data", [])

    return jsonify({
        "success": True,
        "data": list(set(data))
    })

if __name__ == "__main__":
    app.run(port=54321)
```

---

# 八、插件 SDK（强烈推荐）

---

## 8.1 Python SDK（设计）

```python
from plugin_sdk import Plugin

plugin = Plugin("com.company.data")

@plugin.tool
def clean(data: list):
    return list(set(data))

plugin.run()
```

---

## 8.2 SDK 自动生成：

* manifest
* schema
* health
* 注册逻辑

👉 插件开发者几乎不用写协议

---

# 九、配置文件（极简）

---

## 主程序 config.yaml

```yaml
server:
  port: 8080

plugins:
  directory: ./plugins
  auto_start: true
```

---

## 插件侧（可选 plugin.json）

```json
{
  "command": "python main.py"
}
```

👉 可以进一步删除

---

# 十、扩展能力（可选）

---

## 10.1 异步任务（推荐）

```http
POST /clean → 返回 job_id
GET /jobs/{id}
```

---

## 10.2 SSE 流式

```http
GET /events
```

---

## 10.3 Webhook

插件回调主程序

---

## 10.4 Direct 模式（高性能）

客户端直连插件：

```http
GET /api/plugins/{id}/endpoint
```

---

# 十一、安全（必须补）

---

## ✅ 建议加入

* API Key
* mTLS（进阶）
* 插件沙箱（资源限制）
* rate limit

---
好，我们直接进入**可运行级实现**，给你一版**最小可用但结构正确的 C++ 主程序（v3）**，你可以在此基础上扩展成生产系统。

我会控制复杂度，但保证架构是对的。

---

# 🚀 一、主程序功能目标（本实现覆盖）

这版实现支持：

* ✅ 插件进程启动
* ✅ 解析 stdout 获取 endpoint
* ✅ 拉取 `/__plugin__/manifest`
* ✅ 自动注册插件 & 工具
* ✅ 自动路由转发 `/api/plugins/...`
* ✅ 健康检查（基础版）

---

# 🧱 二、技术选型

* HTTP Server: `cpp-httplib`
* JSON: `nlohmann/json`
* 进程管理: `popen`（简化版，生产建议 fork/exec）

---

# 📁 三、目录结构

```id="z17bgs"
main/
├── main.cpp
├── plugin_manager.h
├── plugin_manager.cpp
├── proxy.h
├── proxy.cpp
├── utils.h
```

---

# 🧩 四、核心代码

---

# 4.1 Plugin 数据结构

```cpp
// plugin_manager.h
#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <nlohmann/json.hpp>

struct Tool {
    std::string name;
    std::string method;
    std::string path;
};

struct Plugin {
    std::string id;
    std::string endpoint;
    std::vector<Tool> tools;
    bool healthy = true;
};

class PluginManager {
public:
    bool LoadPlugins(const std::string& dir);
    bool StartPlugin(const std::string& command);

    Plugin* GetPlugin(const std::string& id);

    std::unordered_map<std::string, Plugin>& GetAll() {
        return plugins_;
    }

private:
    bool FetchManifest(Plugin& plugin);

    std::unordered_map<std::string, Plugin> plugins_;
};
```

---

# 4.2 插件加载与注册

```cpp
// plugin_manager.cpp
#include "plugin_manager.h"
#include <iostream>
#include <cstdio>
#include <sstream>
#include <httplib.h>

bool PluginManager::StartPlugin(const std::string& command) {
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) return false;

    char buffer[256];
    std::string line;

    // 等待 stdout 中的 endpoint
    while (fgets(buffer, sizeof(buffer), pipe)) {
        line = buffer;

        if (line.find("PLUGIN_READY:") != std::string::npos) {
            std::string endpoint = line.substr(line.find(":") + 1);
            endpoint.erase(endpoint.find_last_not_of(" \n\r\t") + 1);

            Plugin plugin;
            plugin.endpoint = endpoint;

            // 拉取 manifest
            if (!FetchManifest(plugin)) {
                std::cerr << "Failed to fetch manifest\n";
                return false;
            }

            plugins_[plugin.id] = plugin;

            std::cout << "Plugin loaded: " << plugin.id << std::endl;
            return true;
        }
    }

    return false;
}
```

---

# 4.3 拉取 Manifest

```cpp
bool PluginManager::FetchManifest(Plugin& plugin) {
    httplib::Client cli(plugin.endpoint);

    auto res = cli.Get("/__plugin__/manifest");
    if (!res || res->status != 200) {
        return false;
    }

    auto json = nlohmann::json::parse(res->body);

    plugin.id = json["plugin"]["id"];

    for (auto& t : json["tools"]) {
        Tool tool;
        tool.name = t["name"];
        tool.method = t["method"];
        tool.path = t["path"];
        plugin.tools.push_back(tool);
    }

    return true;
}
```

---

# 4.4 反向代理（核心）

```cpp
// proxy.cpp
#include <httplib.h>
#include "plugin_manager.h"

void SetupProxy(httplib::Server& server, PluginManager& pm) {

    server.Post(R"(/api/plugins/(.*))", [&](const httplib::Request& req, httplib::Response& res) {
        
        std::string path = req.path; 
        // /api/plugins/{plugin_id}/xxx

        auto parts = Split(path, '/');

        if (parts.size() < 4) {
            res.status = 400;
            return;
        }

        std::string plugin_id = parts[3];

        auto plugin = pm.GetPlugin(plugin_id);
        if (!plugin) {
            res.status = 404;
            return;
        }

        // 构造目标路径
        std::string subpath = path.substr(
            std::string("/api/plugins/" + plugin_id).length()
        );

        httplib::Client cli(plugin->endpoint);

        auto result = cli.Post(subpath.c_str(), req.body, "application/json");

        if (!result) {
            res.status = 502;
            return;
        }

        res.status = result->status;
        res.set_content(result->body, "application/json");
    });

}
```

---

# 4.5 main.cpp

```cpp
#include <httplib.h>
#include "plugin_manager.h"

int main() {
    httplib::Server server;
    PluginManager pm;

    // 启动插件（示例）
    pm.StartPlugin("python3 plugin.py");

    // 设置代理
    SetupProxy(server, pm);

    std::cout << "Server started at 8080\n";

    server.listen("0.0.0.0", 8080);
}
```

---

# 🧪 五、最小插件（测试用）

```python
# plugin.py
from flask import Flask, jsonify, request
import time

app = Flask(__name__)

@app.route("/__plugin__/manifest")
def manifest():
    return {
        "plugin": {
            "id": "com.demo.plugin",
            "name": "Demo",
            "version": "1.0"
        },
        "tools": [
            {
                "name": "echo",
                "method": "POST",
                "path": "/echo"
            }
        ]
    }

@app.route("/echo", methods=["POST"])
def echo():
    return jsonify({
        "success": True,
        "data": request.json
    })

@app.route("/health")
def health():
    return "OK"

if __name__ == "__main__":
    print("PLUGIN_READY: http://127.0.0.1:5001")
    app.run(port=5001)
```

---

# 🧪 六、测试调用

```bash
curl -X POST http://localhost:8080/api/plugins/com.demo.plugin/echo \
  -H "Content-Type: application/json" \
  -d '{"msg":"hello"}'
```

好，这一步我们直接把你这套系统**从“能跑”升级到“可上线”**。我不会泛讲，而是给你一版**生产级架构 + 关键代码设计 + 必要机制**。

重点是：**稳定性、并发、安全、可运维**

---

# 🚀 一、生产级目标（明确边界）

这一版目标不是“完美”，而是：

### ✅ 可上线能力

* 高并发（1000+ QPS 可扩展）
* 插件崩溃自动恢复
* 请求隔离 & 超时控制
* 线程安全
* 可观测（日志 + metrics）
* 插件热加载

---

# 🧱 二、生产级架构（升级版）

```id="q7xg4j"
Client
   │
   ▼
Gateway (Main Program)
   │
   ├── Router
   ├── Plugin Registry（线程安全）
   ├── Connection Pool（每插件）
   ├── Health Checker（后台线程）
   ├── Process Supervisor（守护进程）
   ├── Metrics（Prometheus）
   │
   ▼
Plugins（隔离进程）
```

---

# 🔥 三、核心升级点（必须做）

---

# 3.1 线程安全 PluginManager

### ❗问题（你当前版本）

```cpp
std::unordered_map<std::string, Plugin> plugins_;
```

👉 ❌ 多线程不安全

---

### ✅ 生产级写法

```cpp id="1mzn1y"
#include <shared_mutex>

class PluginManager {
private:
    std::unordered_map<std::string, Plugin> plugins_;
    mutable std::shared_mutex mutex_;

public:
    void AddPlugin(const Plugin& plugin) {
        std::unique_lock lock(mutex_);
        plugins_[plugin.id] = plugin;
    }

    std::optional<Plugin> GetPlugin(const std::string& id) const {
        std::shared_lock lock(mutex_);
        auto it = plugins_.find(id);
        if (it != plugins_.end()) return it->second;
        return std::nullopt;
    }
};
```

---

# 3.2 连接池（关键性能点）

### ❗问题

```cpp
httplib::Client cli(plugin->endpoint);
```

👉 ❌ 每次请求创建连接（性能差）

---

### ✅ 连接池设计

```cpp id="r4so3n"
class HttpClientPool {
public:
    std::shared_ptr<httplib::Client> Get(const std::string& endpoint) {
        std::lock_guard<std::mutex> lock(mutex_);

        if (pool_[endpoint].empty()) {
            return CreateClient(endpoint);
        }

        auto cli = pool_[endpoint].back();
        pool_[endpoint].pop_back();
        return cli;
    }

    void Release(const std::string& endpoint, std::shared_ptr<httplib::Client> cli) {
        std::lock_guard<std::mutex> lock(mutex_);
        pool_[endpoint].push_back(cli);
    }

private:
    std::unordered_map<std::string, std::vector<std::shared_ptr<httplib::Client>>> pool_;
    std::mutex mutex_;

    std::shared_ptr<httplib::Client> CreateClient(const std::string& endpoint) {
        auto cli = std::make_shared<httplib::Client>(endpoint);
        cli->set_keep_alive(true);
        cli->set_connection_timeout(2);
        cli->set_read_timeout(5);
        return cli;
    }
};
```

---

# 3.3 超时 + 熔断（必须）

---

## ✅ 请求级超时

```cpp id="d0fwcj"
cli->set_read_timeout(3, 0);  // 3秒
cli->set_write_timeout(2, 0);
```

---

## ✅ 熔断器（简化版）

```cpp id="z0izw3"
struct CircuitBreaker {
    int failure_count = 0;
    bool open = false;
    std::chrono::steady_clock::time_point last_fail;

    bool Allow() {
        if (!open) return true;

        auto now = std::chrono::steady_clock::now();
        if (now - last_fail > std::chrono::seconds(10)) {
            open = false;
            failure_count = 0;
            return true;
        }
        return false;
    }

    void OnFail() {
        failure_count++;
        last_fail = std::chrono::steady_clock::now();
        if (failure_count >= 5) open = true;
    }

    void OnSuccess() {
        failure_count = 0;
    }
};
```

👉 每个 plugin 一个

---

# 3.4 健康检查 + 自动重启

---

## ✅ 后台线程

```cpp id="l3rtjk"
void StartHealthCheck(PluginManager& pm) {
    std::thread([&]() {
        while (true) {
            for (auto& [id, plugin] : pm.GetAll()) {
                httplib::Client cli(plugin.endpoint);

                auto res = cli.Get("/health");

                if (!res || res->status != 200) {
                    std::cout << "Restart plugin: " << id << std::endl;
                    RestartPlugin(id);
                }
            }
            std::this_thread::sleep_for(std::chrono::seconds(5));
        }
    }).detach();
}
```

---

# 3.5 统一错误包装（网关层）

---

```cpp id="uj3u9v"
void SendError(httplib::Response& res, int status, std::string code, std::string msg) {
    nlohmann::json err = {
        {"success", false},
        {"error", {
            {"code", code},
            {"message", msg}
        }}
    };

    res.status = status;
    res.set_content(err.dump(), "application/json");
}
```

---

# 3.6 多方法支持（必须补）

---

```cpp id="7jybj4"
auto forward = [&](const Request& req, Response& res) {
    if (req.method == "GET") {
        cli->Get(...)
    } else if (req.method == "POST") {
        cli->Post(...)
    } else if (req.method == "PUT") {
        cli->Put(...)
    } else if (req.method == "DELETE") {
        cli->Delete(...)
    }
};
```

---

# 3.7 日志系统（必须）

建议：

```id="k8u7qz"
spdlog
```

---

## 示例

```cpp id="s2o3gi"
spdlog::info("Request: {} {}", req.method, req.path);
spdlog::error("Plugin {} failed", plugin_id);
```

---

# 3.8 Metrics（Prometheus）

---

## 示例接口

```http id="ndtf1c"
GET /metrics
```

---

## 输出

```id="l67noo"
plugin_requests_total{plugin="data"} 1234
plugin_errors_total{plugin="data"} 12
```

---

# 3.9 资源隔离（重要）

---

### Linux 推荐：

```id="8v94n7"
cgroups / ulimit
```

限制：

* CPU
* 内存
* 文件数

---

# 3.10 安全（必须）

---

## 最低要求

* API Key

```http id="1d8htk"
Authorization: Bearer xxx
```

---

## 网关校验

```cpp id="p2ntz8"
if (req.get_header_value("Authorization") != expected) {
    return 401;
}
```

---

# 🧪 四、性能预期

| 项目      | 能力           |
| ------- | ------------ |
| 单实例 QPS | 1000~3000    |
| 延迟      | < 10ms（网关开销） |
| 插件崩溃恢复  | < 5 秒        |
| 并发      | 线程池扩展        |

---

# 📦 五、部署建议

---

## ✅ 推荐结构

```id="0qijed"
systemd / docker
```

---

## Docker 示例

```dockerfile id="4fz6k3"
FROM ubuntu:22.04
RUN apt install -y libssl-dev
COPY main /app
CMD ["/app"]
```

---

# 🎯 六、你现在达到了什么水平？

这套做完，你的系统已经是：

> ✅ 企业级插件网关
> ✅ 类似：API Gateway + Plugin Runtime
