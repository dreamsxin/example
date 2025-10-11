# 完整的爬虫浏览器池和IP代理池系统

## 1. 项目结构和依赖

首先创建项目结构：
```
crawler-pool/
├── main.go
├── browser/
│   ├── browser_pool.go
│   └── browser_manager.go
├── proxy/
│   ├── proxy_pool.go
│   └── proxy_manager.go
├── redis/
│   └── redis_client.go
└── config/
    └── config.go
```

go.mod:
```go
module crawler-pool

go 1.21

require (
    github.com/go-redis/redis/v8 v8.11.5
    github.com/chromedp/chromedp v0.9.1
    github.com/robfig/cron/v3 v3.0.1
)
```

## 2. Redis客户端

```go
// redis/redis_client.go
package redis

import (
    "context"
    "fmt"
    "time"

    "github.com/go-redis/redis/v8"
)

type Client struct {
    client *redis.Client
    ctx    context.Context
}

func NewRedisClient(addr, password string, db int) *Client {
    rdb := redis.NewClient(&redis.Options{
        Addr:     addr,
        Password: password,
        DB:       db,
    })

    ctx := context.Background()

    // 测试连接
    if err := rdb.Ping(ctx).Err(); err != nil {
        panic(fmt.Sprintf("Failed to connect to Redis: %v", err))
    }

    return &Client{
        client: rdb,
        ctx:    ctx,
    }
}

func (c *Client) GetClient() *redis.Client {
    return c.client
}

func (c *Client) GetContext() context.Context {
    return c.ctx
}

func (c *Client) Close() error {
    return c.client.Close()
}
```

## 3. 配置管理

```go
// config/config.go
package config

type Config struct {
    Redis struct {
        Addr     string
        Password string
        DB       int
    }
    BrowserPool struct {
        MaxCapacity     int
        CheckInterval   int // 检查间隔(秒)
        IdleTimeout     int // 空闲超时(秒)
        CleanupInterval int // 清理间隔(秒)
    }
    ProxyPool struct {
        CheckInterval    int // 检查间隔(秒)
        MaxRetryCount   int // 最大重试次数
        IdleTimeout     int // 空闲超时(秒)
        CleanupInterval int // 清理间隔(秒)
    }
}

func LoadConfig() *Config {
    cfg := &Config{}
    
    // Redis配置
    cfg.Redis.Addr = "localhost:6379"
    cfg.Redis.Password = ""
    cfg.Redis.DB = 0
    
    // 浏览器池配置
    cfg.BrowserPool.MaxCapacity = 10
    cfg.BrowserPool.CheckInterval = 30
    cfg.BrowserPool.IdleTimeout = 300
    cfg.BrowserPool.CleanupInterval = 60
    
    // 代理池配置
    cfg.ProxyPool.CheckInterval = 60
    cfg.ProxyPool.MaxRetryCount = 5
    cfg.ProxyPool.IdleTimeout = 600
    cfg.ProxyPool.CleanupInterval = 120
    
    return cfg
}
```

## 4. IP代理池实现

```go
// proxy/proxy_pool.go
package proxy

import (
    "context"
    "encoding/json"
    "fmt"
    "log"
    "time"

    "github.com/go-redis/redis/v8"
)

type ProxyIP struct {
    IP         string    `json:"ip"`
    Port       string    `json:"port"`
    Protocol   string    `json:"protocol"`
    Location   string    `json:"location"`
    Status     string    `json:"status"`     // available, checking, invalid
    LastUsed   time.Time `json:"last_used"`
    LastCheck  time.Time `json:"last_check"`
    FailCount  int       `json:"fail_count"`
    CreateTime time.Time `json:"create_time"`
}

type ProxyPool struct {
    redisClient *redis.Client
    ctx         context.Context
    config      *config.Config
}

func NewProxyPool(redisClient *redis.Client, cfg *config.Config) *ProxyPool {
    return &ProxyPool{
        redisClient: redisClient,
        ctx:         context.Background(),
        config:      cfg,
    }
}

// 添加新代理到待检测队列
func (p *ProxyPool) AddNewProxy(proxy *ProxyIP) error {
    proxy.CreateTime = time.Now()
    proxy.LastCheck = time.Now()
    proxy.Status = "checking"
    proxy.FailCount = 0

    data, err := json.Marshal(proxy)
    if err != nil {
        return err
    }

    // 添加到待检测Zset，分数为检测次数
    key := "proxy:checking"
    member := fmt.Sprintf("%s:%s", proxy.IP, proxy.Port)
    
    pipe := p.redisClient.TxPipeline()
    pipe.ZAdd(p.ctx, key, &redis.Z{
        Score:  0,
        Member: member,
    })
    pipe.HSet(p.ctx, "proxy:details", member, data)
    _, err = pipe.Exec(p.ctx)
    
    return err
}

// 从数据库获取新代理（模拟）
func (p *ProxyPool) FetchNewProxiesFromDB() ([]*ProxyIP, error) {
    // 这里模拟从数据库获取新代理
    // 实际应用中应该连接真实的数据库
    log.Println("Fetching new proxies from database...")
    
    // 返回空数组模拟没有新代理
    return []*ProxyIP{}, nil
}

// 检查代理状态
func (p *ProxyPool) CheckProxyStatus(proxy *ProxyIP) bool {
    // 这里实现代理IP的可用性检查
    // 可以通过发送HTTP请求到测试网站来验证
    
    // 模拟检查，实际应该实现真实的检查逻辑
    log.Printf("Checking proxy status: %s:%s", proxy.IP, proxy.Port)
    
    // 模拟80%的成功率
    return time.Now().Unix()%5 != 0 // 80%返回true
}

// 定期检查新代理
func (p *ProxyPool) CheckNewProxies() {
    proxies, err := p.FetchNewProxiesFromDB()
    if err != nil {
        log.Printf("Error fetching new proxies: %v", err)
        return
    }

    for _, proxy := range proxies {
        if err := p.AddNewProxy(proxy); err != nil {
            log.Printf("Error adding new proxy: %v", err)
        } else {
            log.Printf("Added new proxy to checking queue: %s:%s", proxy.IP, proxy.Port)
        }
    }
}

// 定期检查可用队列中的代理
func (p *ProxyPool) CheckAvailableProxies() {
    key := "proxy:available"
    
    // 获取所有可用代理
    members, err := p.redisClient.ZRange(p.ctx, key, 0, -1).Result()
    if err != nil {
        log.Printf("Error getting available proxies: %v", err)
        return
    }

    for _, member := range members {
        // 获取代理详情
        data, err := p.redisClient.HGet(p.ctx, "proxy:details", member).Result()
        if err != nil {
            continue
        }

        var proxy ProxyIP
        if err := json.Unmarshal([]byte(data), &proxy); err != nil {
            continue
        }

        // 检查是否超时未使用
        if time.Since(proxy.LastUsed).Seconds() > float64(p.config.ProxyPool.IdleTimeout) {
            // 重新检查状态
            if !p.CheckProxyStatus(&proxy) {
                // 状态异常，移到待检测队列
                p.MoveToCheckingQueue(&proxy)
                log.Printf("Moved idle proxy to checking queue: %s", member)
            } else {
                // 更新最后检查时间
                proxy.LastCheck = time.Now()
                p.updateProxyDetails(&proxy)
            }
        }
    }
}

// 定期检查待检测队列
func (p *ProxyPool) CheckCheckingProxies() {
    key := "proxy:checking"
    
    // 获取所有待检测代理
    members, err := p.redisClient.ZRangeWithScores(p.ctx, key, 0, -1).Result()
    if err != nil {
        log.Printf("Error getting checking proxies: %v", err)
        return
    }

    for _, z := range members {
        member := z.Member.(string)
        failCount := int(z.Score)

        // 获取代理详情
        data, err := p.redisClient.HGet(p.ctx, "proxy:details", member).Result()
        if err != nil {
            continue
        }

        var proxy ProxyIP
        if err := json.Unmarshal([]byte(data), &proxy); err != nil {
            continue
        }

        if failCount >= p.config.ProxyPool.MaxRetryCount {
            // 超过最大重试次数，从待检测队列移除并同步到数据库
            p.RemoveProxy(&proxy)
            log.Printf("Removed proxy after max retries: %s", member)
            continue
        }

        // 检查代理状态
        if p.CheckProxyStatus(&proxy) {
            // 状态正常，移到可用队列
            p.MoveToAvailableQueue(&proxy)
            log.Printf("Moved proxy to available queue: %s", member)
        } else {
            // 状态异常，增加失败计数
            p.IncrementFailCount(&proxy)
            log.Printf("Proxy check failed, incremented fail count: %s", member)
        }
    }
}

// 移动到待检测队列
func (p *ProxyPool) MoveToCheckingQueue(proxy *ProxyIP) {
    member := fmt.Sprintf("%s:%s", proxy.IP, proxy.Port)
    proxy.Status = "checking"
    proxy.LastCheck = time.Now()

    data, _ := json.Marshal(proxy)

    pipe := p.redisClient.TxPipeline()
    pipe.ZRem(p.ctx, "proxy:available", member)
    pipe.ZAdd(p.ctx, "proxy:checking", &redis.Z{
        Score:  float64(proxy.FailCount),
        Member: member,
    })
    pipe.HSet(p.ctx, "proxy:details", member, data)
    pipe.Exec(p.ctx)
}

// 移动到可用队列
func (p *ProxyPool) MoveToAvailableQueue(proxy *ProxyIP) {
    member := fmt.Sprintf("%s:%s", proxy.IP, proxy.Port)
    proxy.Status = "available"
    proxy.LastCheck = time.Now()
    proxy.FailCount = 0

    data, _ := json.Marshal(proxy)

    pipe := p.redisClient.TxPipeline()
    pipe.ZRem(p.ctx, "proxy:checking", member)
    pipe.ZAdd(p.ctx, "proxy:available", &redis.Z{
        Score:  float64(time.Now().Unix()),
        Member: member,
    })
    pipe.HSet(p.ctx, "proxy:details", member, data)
    pipe.Exec(p.ctx)
}

// 增加失败计数
func (p *ProxyPool) IncrementFailCount(proxy *ProxyIP) {
    member := fmt.Sprintf("%s:%s", proxy.IP, proxy.Port)
    proxy.FailCount++
    proxy.LastCheck = time.Now()

    data, _ := json.Marshal(proxy)

    pipe := p.redisClient.TxPipeline()
    pipe.ZAdd(p.ctx, "proxy:checking", &redis.Z{
        Score:  float64(proxy.FailCount),
        Member: member,
    })
    pipe.HSet(p.ctx, "proxy:details", member, data)
    pipe.Exec(p.ctx)
}

// 移除代理
func (p *ProxyPool) RemoveProxy(proxy *ProxyIP) {
    member := fmt.Sprintf("%s:%s", proxy.IP, proxy.Port)

    pipe := p.redisClient.TxPipeline()
    pipe.ZRem(p.ctx, "proxy:checking", member)
    pipe.ZRem(p.ctx, "proxy:available", member)
    pipe.HDel(p.ctx, "proxy:details", member)
    pipe.Exec(p.ctx)

    // 同步状态到数据库
    p.SyncProxyStatusToDB(proxy)
}

// 同步代理状态到数据库
func (p *ProxyPool) SyncProxyStatusToDB(proxy *ProxyIP) {
    // 这里实现将代理状态同步到数据库的逻辑
    log.Printf("Syncing proxy status to DB: %s:%s - FailCount: %d", 
        proxy.IP, proxy.Port, proxy.FailCount)
}

// 更新代理详情
func (p *ProxyPool) updateProxyDetails(proxy *ProxyIP) {
    member := fmt.Sprintf("%s:%s", proxy.IP, proxy.Port)
    data, _ := json.Marshal(proxy)
    p.redisClient.HSet(p.ctx, "proxy:details", member, data)
}

// 获取可用代理
func (p *ProxyPool) GetAvailableProxy() (*ProxyIP, error) {
    key := "proxy:available"
    
    // 获取最早添加的可用代理（分数最小的）
    members, err := p.redisClient.ZRange(p.ctx, key, 0, 0).Result()
    if err != nil || len(members) == 0 {
        return nil, fmt.Errorf("no available proxy")
    }

    member := members[0]
    data, err := p.redisClient.HGet(p.ctx, "proxy:details", member).Result()
    if err != nil {
        return nil, err
    }

    var proxy ProxyIP
    if err := json.Unmarshal([]byte(data), &proxy); err != nil {
        return nil, err
    }

    // 更新最后使用时间
    proxy.LastUsed = time.Now()
    p.updateProxyDetails(&proxy)

    // 更新Zset分数为当前时间
    p.redisClient.ZAdd(p.ctx, key, &redis.Z{
        Score:  float64(time.Now().Unix()),
        Member: member,
    })

    return &proxy, nil
}

// 返还代理并检查状态
func (p *ProxyPool) ReturnProxy(proxy *ProxyIP, isHealthy bool) {
    if isHealthy {
        // 状态正常，放回可用队列
        p.MoveToAvailableQueue(proxy)
    } else {
        // 状态异常，移到待检测队列
        p.MoveToCheckingQueue(proxy)
    }
}
```

## 5. 浏览器池实现

```go
// browser/browser_pool.go
package browser

import (
    "context"
    "encoding/json"
    "fmt"
    "log"
    "time"

    "github.com/chromedp/chromedp"
    "github.com/go-redis/redis/v8"
    "crawler-pool/config"
    "crawler-pool/proxy"
)

type BrowserInstance struct {
    ID         string    `json:"id"`
    Proxy      *proxy.ProxyIP `json:"proxy"`
    Status     string    `json:"status"` // available, in_use, closed
    LastUsed   time.Time `json:"last_used"`
    CreateTime time.Time `json:"create_time"`
    Context    context.Context `json:"-"`
    Cancel     context.CancelFunc `json:"-"`
}

type BrowserPool struct {
    redisClient *redis.Client
    ctx         context.Context
    config      *config.Config
    proxyPool   *proxy.ProxyPool
}

func NewBrowserPool(redisClient *redis.Client, proxyPool *proxy.ProxyPool, cfg *config.Config) *BrowserPool {
    return &BrowserPool{
        redisClient: redisClient,
        ctx:         context.Background(),
        config:      cfg,
        proxyPool:   proxyPool,
    }
}

// 创建新的浏览器实例
func (b *BrowserPool) CreateBrowserInstance() (*BrowserInstance, error) {
    // 从代理池获取代理
    proxyIP, err := b.proxyPool.GetAvailableProxy()
    if err != nil {
        return nil, fmt.Errorf("failed to get proxy: %v", err)
    }

    // 创建浏览器实例
    instance := &BrowserInstance{
        ID:         fmt.Sprintf("browser_%d", time.Now().UnixNano()),
        Proxy:      proxyIP,
        Status:     "available",
        CreateTime: time.Now(),
        LastUsed:   time.Now(),
    }

    // 使用chromedp创建浏览器上下文
    opts := append(chromedp.DefaultExecAllocatorOptions[:],
        chromedp.Flag("headless", true),
        chromedp.Flag("disable-gpu", true),
        chromedp.Flag("no-sandbox", true),
        chromedp.Flag("disable-dev-shm-usage", true),
    )

    if proxyIP != nil {
        proxyURL := fmt.Sprintf("%s://%s:%s", proxyIP.Protocol, proxyIP.IP, proxyIP.Port)
        opts = append(opts, chromedp.ProxyServer(proxyURL))
    }

    allocCtx, _ := chromedp.NewExecAllocator(b.ctx, opts...)
    instance.Context, instance.Cancel = chromedp.NewContext(allocCtx)

    // 保存实例到Redis
    if err := b.saveBrowserInstance(instance); err != nil {
        instance.Cancel()
        return nil, err
    }

    // 添加到可用列表
    if err := b.redisClient.LPush(b.ctx, "browser:available", instance.ID).Err(); err != nil {
        instance.Cancel()
        return nil, err
    }

    log.Printf("Created new browser instance: %s with proxy: %s:%s", 
        instance.ID, proxyIP.IP, proxyIP.Port)

    return instance, nil
}

// 保存浏览器实例详情
func (b *BrowserPool) saveBrowserInstance(instance *BrowserInstance) error {
    data, err := json.Marshal(instance)
    if err != nil {
        return err
    }

    return b.redisClient.HSet(b.ctx, "browser:instances", instance.ID, data).Err()
}

// 获取浏览器实例
func (b *BrowserPool) GetBrowserInstance() (*BrowserInstance, error) {
    // 从可用列表中获取
    id, err := b.redisClient.RPop(b.ctx, "browser:available").Result()
    if err != nil {
        // 如果没有可用实例，检查是否需要创建新的
        count, _ := b.GetInstanceCount()
        if count < b.config.BrowserPool.MaxCapacity {
            return b.CreateBrowserInstance()
        }
        return nil, fmt.Errorf("no available browser instance")
    }

    // 获取实例详情
    data, err := b.redisClient.HGet(b.ctx, "browser:instances", id).Result()
    if err != nil {
        return nil, err
    }

    var instance BrowserInstance
    if err := json.Unmarshal([]byte(data), &instance); err != nil {
        return nil, err
    }

    // 更新状态和使用时间
    instance.Status = "in_use"
    instance.LastUsed = time.Now()
    b.saveBrowserInstance(&instance)

    return &instance, nil
}

// 返还浏览器实例
func (b *BrowserPool) ReturnBrowserInstance(instance *BrowserInstance, isHealthy bool) {
    if !isHealthy {
        // 实例异常，销毁并返还代理
        b.DestroyBrowserInstance(instance, true)
        return
    }

    // 更新状态
    instance.Status = "available"
    instance.LastUsed = time.Now()
    b.saveBrowserInstance(instance)

    // 放回可用列表
    b.redisClient.LPush(b.ctx, "browser:available", instance.ID)
}

// 销毁浏览器实例
func (b *BrowserPool) DestroyBrowserInstance(instance *BrowserInstance, returnProxy bool) {
    // 取消浏览器上下文
    if instance.Cancel != nil {
        instance.Cancel()
    }

    // 从Redis中移除
    pipe := b.redisClient.TxPipeline()
    pipe.LRem(b.ctx, "browser:available", 0, instance.ID)
    pipe.HDel(b.ctx, "browser:instances", instance.ID)
    pipe.Exec(b.ctx)

    // 返还代理
    if returnProxy && instance.Proxy != nil {
        b.proxyPool.ReturnProxy(instance.Proxy, false) // 代理可能也有问题
    }

    log.Printf("Destroyed browser instance: %s", instance.ID)
}

// 获取实例数量
func (b *BrowserPool) GetInstanceCount() (int, error) {
    count, err := b.redisClient.HLen(b.ctx, "browser:instances").Result()
    return int(count), err
}

// 定期检查并补充实例
func (b *BrowserPool) CheckAndReplenishInstances() {
    count, err := b.GetInstanceCount()
    if err != nil {
        log.Printf("Error getting instance count: %v", err)
        return
    }

    if count < b.config.BrowserPool.MaxCapacity {
        needed := b.config.BrowserPool.MaxCapacity - count
        log.Printf("Need to create %d new browser instances", needed)

        for i := 0; i < needed; i++ {
            if _, err := b.CreateBrowserInstance(); err != nil {
                log.Printf("Error creating browser instance: %v", err)
                break
            }
        }
    }
}

// 定期清理空闲实例
func (b *BrowserPool) CleanupIdleInstances() {
    // 获取所有实例
    instances, err := b.redisClient.HGetAll(b.ctx, "browser:instances").Result()
    if err != nil {
        log.Printf("Error getting browser instances: %v", err)
        return
    }

    for _, data := range instances {
        var instance BrowserInstance
        if err := json.Unmarshal([]byte(data), &instance); err != nil {
            continue
        }

        // 检查空闲时间
        if instance.Status == "available" && 
           time.Since(instance.LastUsed).Seconds() > float64(b.config.BrowserPool.IdleTimeout) {
            
            // 检查实例是否健康
            if !b.CheckBrowserHealth(&instance) {
                log.Printf("Destroying unhealthy browser instance: %s", instance.ID)
                b.DestroyBrowserInstance(&instance, true)
            }
        }
    }
}

// 检查浏览器健康状态
func (b *BrowserPool) CheckBrowserHealth(instance *BrowserInstance) bool {
    // 这里实现浏览器健康检查逻辑
    // 可以通过访问一个测试页面来验证
    
    // 模拟检查，实际应该实现真实的健康检查
    log.Printf("Checking browser health: %s", instance.ID)
    return time.Now().Unix()%10 != 0 // 90%返回true
}
```

## 6. 管理器实现

```go
// proxy/proxy_manager.go
package proxy

import (
    "log"
    "time"

    "github.com/robfig/cron/v3"
)

type ProxyManager struct {
    proxyPool *ProxyPool
    config    *config.Config
    cron      *cron.Cron
}

func NewProxyManager(proxyPool *ProxyPool, cfg *config.Config) *ProxyManager {
    return &ProxyManager{
        proxyPool: proxyPool,
        config:    cfg,
        cron:      cron.New(),
    }
}

func (m *ProxyManager) Start() {
    // 定期检查新代理
    m.cron.AddFunc(
        fmt.Sprintf("*/%d * * * *", m.config.ProxyPool.CheckInterval/60), 
        m.proxyPool.CheckNewProxies,
    )

    // 定期检查可用代理
    m.cron.AddFunc(
        fmt.Sprintf("*/%d * * * *", m.config.ProxyPool.CheckInterval/60), 
        m.proxyPool.CheckAvailableProxies,
    )

    // 定期检查待检测代理
    m.cron.AddFunc(
        fmt.Sprintf("*/%d * * * *", m.config.ProxyPool.CleanupInterval/60), 
        m.proxyPool.CheckCheckingProxies,
    )

    m.cron.Start()
    log.Println("Proxy manager started")
}

func (m *ProxyManager) Stop() {
    m.cron.Stop()
    log.Println("Proxy manager stopped")
}
```

```go
// browser/browser_manager.go
package browser

import (
    "log"
    "time"

    "github.com/robfig/cron/v3"
    "crawler-pool/config"
)

type BrowserManager struct {
    browserPool *BrowserPool
    config      *config.Config
    cron        *cron.Cron
}

func NewBrowserManager(browserPool *BrowserPool, cfg *config.Config) *BrowserManager {
    return &BrowserManager{
        browserPool: browserPool,
        config:      cfg,
        cron:        cron.New(),
    }
}

func (m *BrowserManager) Start() {
    // 定期检查并补充实例
    m.cron.AddFunc(
        fmt.Sprintf("*/%d * * * *", m.config.BrowserPool.CheckInterval/60), 
        m.browserPool.CheckAndReplenishInstances,
    )

    // 定期清理空闲实例
    m.cron.AddFunc(
        fmt.Sprintf("*/%d * * * *", m.config.BrowserPool.CleanupInterval/60), 
        m.browserPool.CleanupIdleInstances,
    )

    m.cron.Start()
    log.Println("Browser manager started")
}

func (m *BrowserManager) Stop() {
    m.cron.Stop()
    log.Println("Browser manager stopped")
}
```

## 7. 主程序

```go
// main.go
package main

import (
    "log"
    "os"
    "os/signal"
    "syscall"

    "crawler-pool/browser"
    "crawler-pool/config"
    "crawler-pool/proxy"
    "crawler-pool/redis"
)

func main() {
    // 加载配置
    cfg := config.LoadConfig()

    // 初始化Redis客户端
    redisClient := redis.NewRedisClient(
        cfg.Redis.Addr,
        cfg.Redis.Password,
        cfg.Redis.DB,
    )
    defer redisClient.Close()

    // 初始化代理池
    proxyPool := proxy.NewProxyPool(redisClient.GetClient(), cfg)
    
    // 初始化浏览器池
    browserPool := browser.NewBrowserPool(
        redisClient.GetClient(), 
        proxyPool, 
        cfg,
    )

    // 初始化管理器
    proxyManager := proxy.NewProxyManager(proxyPool, cfg)
    browserManager := browser.NewBrowserManager(browserPool, cfg)

    // 启动管理器
    proxyManager.Start()
    browserManager.Start()

    log.Println("Crawler browser pool system started")

    // 等待中断信号
    sigChan := make(chan os.Signal, 1)
    signal.Notify(sigChan, syscall.SIGINT, syscall.SIGTERM)
    <-sigChan

    // 停止管理器
    proxyManager.Stop()
    browserManager.Stop()

    log.Println("Crawler browser pool system stopped")
}
```

## 8. 使用示例

```go
// example/usage_example.go
package main

import (
    "context"
    "log"
    "time"

    "github.com/chromedp/chromedp"
    "crawler-pool/browser"
    "crawler-pool/config"
    "crawler-pool/proxy"
    "crawler-pool/redis"
)

func main() {
    cfg := config.LoadConfig()
    redisClient := redis.NewRedisClient(cfg.Redis.Addr, cfg.Redis.Password, cfg.Redis.DB)
    defer redisClient.Close()

    proxyPool := proxy.NewProxyPool(redisClient.GetClient(), cfg)
    browserPool := browser.NewBrowserPool(redisClient.GetClient(), proxyPool, cfg)

    // 获取浏览器实例
    instance, err := browserPool.GetBrowserInstance()
    if err != nil {
        log.Fatal(err)
    }

    // 使用浏览器实例
    var title string
    err = chromedp.Run(instance.Context,
        chromedp.Navigate("https://httpbin.org/ip"),
        chromedp.Title(&title),
    )

    // 返还浏览器实例
    browserPool.ReturnBrowserInstance(instance, err == nil)

    if err != nil {
        log.Printf("Browser operation failed: %v", err)
    } else {
        log.Printf("Page title: %s", title)
    }
}
```

浏览器池和IP代理池管理功能，具有以下特点：

1. **浏览器池管理**：
   - 使用Redis List保存可用浏览器实例
   - 使用Redis Hash保存浏览器实例详情
   - 自动扩容和缩容
   - 健康检查和异常处理

2. **IP代理池管理**：
   - 使用Redis Zset管理代理状态
   - 多级代理检测机制
   - 自动代理轮换和失效处理

3. **容错机制**：
   - 实例异常自动销毁和重建
   - 代理IP状态监控和自动切换
   - 优雅的错误处理和资源回收

系统通过定时任务自动维护资源状态，确保爬虫任务的稳定执行。
