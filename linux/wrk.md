# wrk HTTP基准测试工具参数详解

wrk 是一款现代HTTP基准测试工具，使用多线程和事件驱动机制，能够产生大量负载。以下是 wrk 的详细参数说明和使用方法：

## 基本语法

```bash
wrk <选项> <测试URL>
```

## 主要参数

### 基本参数

| 参数 | 简写 | 说明 |
|------|------|------|
| `--connections` | `-c` | 保持打开的连接总数 |
| `--duration` | `-d` | 测试持续时间 |
| `--threads` | `-t` | 使用的线程数 |
| `--script` | `-s` | 加载Lua测试脚本 |
| `--header` | `-H` | 添加HTTP请求头 |
| `--latency` | `-L` | 打印延迟统计信息 |
| `--timeout` | `-T` | 设置套接字/请求超时时间 |
| `--version` | `-v` | 打印版本信息 |

## 详细参数说明

### 连接和线程参数

```bash
# 使用12个线程，400个连接，测试30秒
wrk --latency -t12 -c400 -d30s http://localhost:8080/

# 使用CPU核心数的线程，1000个连接，测试1分钟
wrk -t$(nproc) -c1000 -d1m https://api.example.com/
```

### 超时设置

```bash
# 设置请求超时为2秒
wrk -t4 -c100 -d10s -T2s http://localhost:8080/

# 设置连接超时为5秒
wrk -t4 -c100 -d10s --timeout 5s http://localhost:8080/
```

### 自定义请求头

```bash
# 添加单个请求头
wrk -t2 -c50 -d10s -H "User-Agent: wrk" http://localhost:8080/

# 添加多个请求头
wrk -t2 -c50 -d10s \
  -H "User-Agent: wrk" \
  -H "Authorization: Bearer token123" \
  -H "Content-Type: application/json" \
  http://localhost:8080/api/v1/users
```

## Lua脚本高级用法

### 基本脚本结构

```lua
-- example.lua
-- 初始化阶段
function init(args)
  -- 初始化代码
end

-- 请求构建阶段
function request()
  -- 构建请求
  return wrk.format(method, path, headers, body)
end

-- 响应处理阶段
function response(status, headers, body)
  -- 处理响应
end

-- 完成阶段
function done(summary, latency, requests)
  -- 测试完成处理
end
```

### 常用脚本示例

#### 1. POST请求测试
```lua
-- post.lua
wrk.method = "POST"
wrk.body   = '{"username":"test","password":"123456"}'
wrk.headers["Content-Type"] = "application/json"
```

```bash
wrk -t4 -c100 -d30s -s post.lua http://localhost:8080/api/login
```

#### 2. 动态参数请求
```lua
-- dynamic.lua
counter = 1

request = function()
   local path = "/api/users/" .. counter
   counter = counter + 1
   return wrk.format("GET", path)
end
```

#### 3. 延迟统计脚本
```lua
-- latency.lua
responses = {}

function response(status, headers, body)
   responses[#responses + 1] = status
end

function done(summary, latency, requests)
   -- 计算成功率
   local success = 0
   for i, status in ipairs(responses) do
      if status == 200 then
         success = success + 1
      end
   end
   
   local rate = success / #responses * 100
   print(string.format("成功率: %.2f%%", rate))
end
```

#### 4. Bearer Token认证
```lua
-- auth.lua
wrk.headers["Authorization"] = "Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9..."
wrk.headers["Content-Type"] = "application/json"
```

#### 5. 复杂场景测试
```lua
-- complex.lua
local methods = {"GET", "POST"}
local urls = {
   "/api/v1/users",
   "/api/v1/products", 
   "/api/v1/orders"
}

request = function()
   local method = methods[math.random(#methods)]
   local url = urls[math.random(#urls)]
   
   if method == "POST" then
      local body = string.format('{"id":%d,"name":"test"}', math.random(1000))
      wrk.headers["Content-Type"] = "application/json"
      return wrk.format("POST", url, wrk.headers, body)
   else
      return wrk.format("GET", url)
   end
end
```

## 输出结果解读

wrk的输出包含以下关键指标：

```
Running 30s test @ http://localhost:8080/
  12 threads and 400 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency    20.31ms   15.42ms 200.15ms   85.34%
    Req/Sec     1.65k   250.32     2.50k    69.87%
  Latency Distribution
     50%   16.20ms
     75%   24.66ms
     90%   35.59ms
     99%   78.59ms
  591200 requests in 30.10s, 86.42MB read
Requests/sec:  19641.23
Transfer/sec:      2.87MB
```

### 指标说明

- **Latency**: 请求延迟统计
  - `Avg`: 平均延迟
  - `Stdev`: 标准差
  - `Max`: 最大延迟
  - `+/- Stdev`: 在标准差范围内的请求比例

- **Req/Sec**: 每个线程的每秒请求数

- **Latency Distribution**: 延迟分布百分位
  - `50%`: P50延迟（中位数）
  - `75%`: P75延迟
  - `90%`: P90延迟  
  - `99%`: P99延迟

- **总体统计**:
  - 总请求数
  - 测试持续时间
  - 总数据传输量
  - 平均每秒请求数
  - 平均每秒传输量

## 实用测试示例

### 1. 基础负载测试
```bash
# 渐进式增加负载测试
for connections in 100 200 500 1000; do
  echo "测试连接数: $connections"
  wrk -t8 -c$connections -d30s http://localhost:8080/
  echo "----------------------------------------"
done
```

### 2. 不同端点的对比测试
```bash
#!/bin/bash
# multi_endpoint_test.sh

ENDPOINTS=(
  "/api/v1/users"
  "/api/v1/products"
  "/api/v1/orders"
  "/api/v1/inventory"
)

for endpoint in "${ENDPOINTS[@]}"; do
  echo "测试端点: $endpoint"
  wrk -t4 -c100 -d15s "http://localhost:8080$endpoint"
  echo "========================================"
done
```

### 3. 长时间稳定性测试
```bash
# 长时间运行测试（10分钟）
wrk -t4 -c500 -d10m http://localhost:8080/health

# 配合监控
watch -n 5 "wrk -t2 -c50 -d5s http://localhost:8080/health | grep Requests/sec"
```

### 4. 带认证的API测试
```bash
# 使用脚本处理认证
wrk -t4 -c200 -d30s -s auth_test.lua http://localhost:8080/api/protected
```

## 性能调优建议

### 1. 线程数选择
```bash
# 通常设置为CPU核心数
wrk -t$(nproc) -c1000 -d30s http://localhost:8080/

# 或者稍多于核心数
wrk -t$(($(nproc) + 2)) -c1000 -d30s http://localhost:8080/
```

### 2. 连接数设置
```bash
# 根据应用特性调整
# 高并发场景
wrk -t8 -c5000 -d30s http://localhost:8080/

# 低延迟场景
wrk -t4 -c100 -d30s http://localhost:8080/
```

### 3. 测试时长建议
```bash
# 快速验证（开发环境）
wrk -t2 -c50 -d10s http://localhost:8080/

# 正式测试（生产环境）
wrk -t8 -c1000 -d5m http://localhost:8080/

# 稳定性测试
wrk -t4 -c500 -d30m http://localhost:8080/
```

## 常见问题排查

### 1. 连接被拒绝
```bash
# 检查目标服务是否运行
curl -I http://localhost:8080/

# 检查防火墙设置
sudo ufw status
```

### 2. 端口耗尽
```bash
# 检查系统限制
sysctl net.ipv4.ip_local_port_range

# 临时增加端口范围
sudo sysctl -w net.ipv4.ip_local_port_range="1024 65535"
```

### 3. 内存不足
```bash
# 监控内存使用
free -h

# 减少连接数或线程数
wrk -t2 -c100 -d30s http://localhost:8080/
```

## 使用 lua

我来介绍几种使用wrk进行随机关键词测试的方法：

### 1. 使用Lua脚本实现随机关键词

#### 基本Lua脚本示例
```lua
-- random_keywords.lua
math.randomseed(os.time())

-- 定义关键词列表
keywords = {
    "technology", "programming", "software", 
    "development", "testing", "performance",
    "optimization", "benchmark", "analysis"
}

request = function()
    -- 随机选择关键词
    local random_index = math.random(1, #keywords)
    local keyword = keywords[random_index]
    
    -- 构建URL路径
    local path = "/search?q=" .. keyword
    
    return wrk.format("GET", path)
end
```

#### 使用方式
```bash
wrk -t4 -c100 -d30s -s random_keywords.lua http://your-api.com
```

### 2. 动态生成随机关键词

```lua
-- dynamic_keywords.lua
math.randomseed(os.time())

function random_string(length)
    local chars = "abcdefghijklmnopqrstuvwxyz0123456789"
    local result = ""
    for i = 1, length do
        local rand_index = math.random(1, #chars)
        result = result .. chars:sub(rand_index, rand_index)
    end
    return result
end

request = function()
    -- 生成长度3-10的随机关键词
    local keyword_length = math.random(3, 10)
    local keyword = random_string(keyword_length)
    
    local path = "/api/search?keyword=" .. keyword
    return wrk.format("GET", path)
end
```

### 3. 从文件读取关键词

#### 关键词文件 (keywords.txt)
```
apple
banana
orange
computer
keyboard
programming
```

#### Lua脚本
```lua
-- file_keywords.lua
math.randomseed(os.time())

-- 读取关键词文件
function load_keywords(filename)
    local keywords = {}
    local file = io.open(filename, "r")
    if file then
        for line in file:lines() do
            table.insert(keywords, line)
        end
        file:close()
    end
    return keywords
end

keywords = load_keywords("keywords.txt")

request = function()
    local random_index = math.random(1, #keywords)
    local keyword = keywords[random_index]
    local path = "/search?q=" .. wrk.urlencode(keyword)
    return wrk.format("GET", path)
end
```

### 4. 高级随机测试脚本

```lua
-- advanced_random_test.lua
math.randomseed(os.time())

-- 配置参数
local config = {
    base_url = "http://your-api.com",
    endpoints = {
        "/search",
        "/api/v1/products",
        "/api/v1/users"
    },
    search_params = {"q", "keyword", "search", "query"}
}

function get_random_endpoint()
    return config.endpoints[math.random(1, #config.endpoints)]
end

function get_random_param()
    return config.search_params[math.random(1, #config.search_params)]
end

function generate_random_keyword()
    local lengths = {3, 4, 5, 6, 7, 8}
    local length = lengths[math.random(1, #lengths)]
    local chars = "abcdefghijklmnopqrstuvwxyz"
    local result = ""
    
    for i = 1, length do
        result = result .. chars:sub(math.random(1, #chars), math.random(1, #chars))
    end
    
    return result
end

request = function()
    local endpoint = get_random_endpoint()
    local param = get_random_param()
    local keyword = generate_random_keyword()
    
    local url = endpoint .. "?" .. param .. "=" .. keyword
    
    -- 随机添加其他参数
    if math.random() > 0.7 then
        url = url .. "&page=" .. math.random(1, 5)
    end
    
    if math.random() > 0.5 then
        url = url .. "&limit=" .. math.random(10, 50)
    end
    
    return wrk.format("GET", url)
end

-- 响应处理（可选）
response = function(status, headers, body)
    if status ~= 200 then
        print("Error: " .. status)
    end
end
```

### 5. POST请求随机数据测试

```lua
-- post_random.lua
math.randomseed(os.time())

function random_string(length)
    local chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
    local result = ""
    for i = 1, length do
        result = result .. chars:sub(math.random(1, #chars), math.random(1, #chars))
    end
    return result
end

request = function()
    local keyword = random_string(math.random(5, 15))
    
    local body = string.format('{"query": "%s", "type": "search"}', keyword)
    local headers = {}
    headers["Content-Type"] = "application/json"
    
    return wrk.format("POST", "/api/search", headers, body)
end
```

### 完整的测试命令示例

```bash
# 基础测试
wrk -t12 -c100 -d30s -s random_keywords.lua http://localhost:8080

# 带延迟统计
wrk -t12 -c100 -d30s --latency -s random_keywords.lua http://localhost:8080

# 输出详细结果
wrk -t12 -c100 -d60s --latency --timeout 10s -s advanced_random_test.lua http://api.example.com
```
