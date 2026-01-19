
## 1. 基础实现代码

### 钉钉机器人客户端

```go
package dingtalk

import (
	"bytes"
	"encoding/json"
	"fmt"
	"net/http"
	"time"
)

// DingRobot 钉钉机器人客户端
type DingRobot struct {
	WebhookURL string
	Secret     string
	Client     *http.Client
}

// NewDingRobot 创建钉钉机器人实例
func NewDingRobot(webhookURL, secret string) *DingRobot {
	return &DingRobot{
		WebhookURL: webhookURL,
		Secret:     secret,
		Client:     &http.Client{Timeout: 10 * time.Second},
	}
}

// Message 基础消息结构
type Message struct {
	MsgType  string      `json:"msgtype"`
	At       *AtInfo     `json:"at,omitempty"`
	Markdown *Markdown   `json:"markdown,omitempty"`
	Text     *Text       `json:"text,omitempty"`
	Link     *Link       `json:"link,omitempty"`
}

type AtInfo struct {
	AtMobiles []string `json:"atMobiles,omitempty"`
	AtUserIds []string `json:"atUserIds,omitempty"`
	IsAtAll   bool     `json:"isAtAll,omitempty"`
}

type Markdown struct {
	Title string `json:"title"`
	Text  string `json:"text"`
}

type Text struct {
	Content string `json:"content"`
}

type Link struct {
	Title      string `json:"title"`
	Text       string `json:"text"`
	MessageURL string `json:"messageUrl"`
	PicURL     string `json:"picUrl,omitempty"`
}

// Send 发送消息
func (r *DingRobot) Send(msg *Message) error {
	body, err := json.Marshal(msg)
	if err != nil {
		return fmt.Errorf("marshal message error: %v", err)
	}

	req, err := http.NewRequest("POST", r.WebhookURL, bytes.NewReader(body))
	if err != nil {
		return fmt.Errorf("create request error: %v", err)
	}
	req.Header.Set("Content-Type", "application/json")

	resp, err := r.Client.Do(req)
	if err != nil {
		return fmt.Errorf("send request error: %v", err)
	}
	defer resp.Body.Close()

	if resp.StatusCode != http.StatusOK {
		return fmt.Errorf("dingtalk API error: status=%d", resp.StatusCode)
	}

	var result map[string]interface{}
	if err := json.NewDecoder(resp.Body).Decode(&result); err != nil {
		return fmt.Errorf("decode response error: %v", err)
	}

	if code, ok := result["errcode"].(float64); ok && code != 0 {
		return fmt.Errorf("dingtalk error: %v", result)
	}

	return nil
}
```

## 2. 报警模板管理器

```go
package alert

import (
	"fmt"
	"strings"
	"time"
)

// AlertLevel 报警级别
type AlertLevel string

const (
	LevelInfo    AlertLevel = "INFO"
	LevelWarning AlertLevel = "WARNING"
	LevelError   AlertLevel = "ERROR"
	LevelCritical AlertLevel = "CRITICAL"
)

// AlertTemplate 报警模板接口
type AlertTemplate interface {
	Render(level AlertLevel, data map[string]interface{}) (*dingtalk.Message, error)
}

// SerpAlertTemplate SERP接口报警模板
type SerpAlertTemplate struct {
	ServiceName string
	AtMobiles   []string
}

func (t *SerpAlertTemplate) Render(level AlertLevel, data map[string]interface{}) (*dingtalk.Message, error) {
	var emoji string
	var color string
	
	switch level {
	case LevelInfo:
		emoji = "ℹ️"
		color = "#3498db"
	case LevelWarning:
		emoji = "⚠️"
		color = "#f39c12"
	case LevelError:
		emoji = "❌"
		color = "#e74c3c"
	case LevelCritical:
		emoji = "🚨"
		color = "#c0392b"
	default:
		emoji = "📊"
		color = "#95a5a6"
	}

	title := fmt.Sprintf("%s SERP接口报警 - %s", emoji, level)
	
	// 构建Markdown内容
	var builder strings.Builder
	builder.WriteString(fmt.Sprintf("### %s %s\n\n", emoji, title))
	builder.WriteString(fmt.Sprintf("**服务名称**: %s\n\n", t.ServiceName))
	builder.WriteString(fmt.Sprintf("**报警时间**: %s\n\n", time.Now().Format("2006-01-02 15:04:05")))
	
	if endpoint, ok := data["endpoint"].(string); ok {
		builder.WriteString(fmt.Sprintf("**接口地址**: `%s`\n\n", endpoint))
	}
	
	if errorMsg, ok := data["error"].(string); ok {
		builder.WriteString(fmt.Sprintf("**错误信息**: %s\n\n", errorMsg))
	}
	
	if latency, ok := data["latency"].(float64); ok {
		builder.WriteString(fmt.Sprintf("**响应延迟**: %.2fms\n\n", latency))
	}
	
	if statusCode, ok := data["status_code"].(int); ok {
		builder.WriteString(fmt.Sprintf("**状态码**: %d\n\n", statusCode))
	}
	
	if requestID, ok := data["request_id"].(string); ok {
		builder.WriteString(fmt.Sprintf("**请求ID**: %s\n\n", requestID))
	}
	
	if suggestion, ok := data["suggestion"].(string); ok {
		builder.WriteString(fmt.Sprintf("**处理建议**: %s\n\n", suggestion))
	}
	
	builder.WriteString("---\n")
	builder.WriteString(fmt.Sprintf("> 级别: **%s** | 服务: %s", level, t.ServiceName))

	msg := &dingtalk.Message{
		MsgType: "markdown",
		Markdown: &dingtalk.Markdown{
			Title: title,
			Text:  builder.String(),
		},
	}
	
	// 错误及以上级别@相关人员
	if level == LevelError || level == LevelCritical {
		msg.At = &dingtalk.AtInfo{
			AtMobiles: t.AtMobiles,
			IsAtAll:   false,
		}
	}

	return msg, nil
}

// ServiceStatusTemplate 服务状态报警模板
type ServiceStatusTemplate struct {
	Cluster    string
	AtMobiles  []string
}

func (t *ServiceStatusTemplate) Render(level AlertLevel, data map[string]interface{}) (*dingtalk.Message, error) {
	var emoji string
	
	switch level {
	case LevelInfo:
		emoji = "✅"
	case LevelWarning:
		emoji = "🔄"
	case LevelError:
		emoji = "❌"
	case LevelCritical:
		emoji = "💥"
	default:
		emoji = "📡"
	}

	title := fmt.Sprintf("%s 服务状态变更 - %s", emoji, level)
	
	var builder strings.Builder
	builder.WriteString(fmt.Sprintf("### %s 服务状态报警\n\n", emoji))
	builder.WriteString(fmt.Sprintf("**集群**: %s\n\n", t.Cluster))
	builder.WriteString(fmt.Sprintf("**时间**: %s\n\n", time.Now().Format("2006-01-02 15:04:05")))
	
	if service, ok := data["service"].(string); ok {
		builder.WriteString(fmt.Sprintf("**服务名称**: %s\n\n", service))
	}
	
	if host, ok := data["host"].(string); ok {
		builder.WriteString(fmt.Sprintf("**主机地址**: %s\n\n", host))
	}
	
	if port, ok := data["port"].(int); ok {
		builder.WriteString(fmt.Sprintf("**服务端口**: %d\n\n", port))
	}
	
	if oldStatus, ok := data["old_status"].(string); ok {
		if newStatus, ok := data["new_status"].(string); ok {
			builder.WriteString(fmt.Sprintf("**状态变更**: %s → %s\n\n", oldStatus, newStatus))
		}
	}
	
	if uptime, ok := data["uptime"].(string); ok {
		builder.WriteString(fmt.Sprintf("**运行时间**: %s\n\n", uptime))
	}
	
	if cpu, ok := data["cpu_usage"].(float64); ok {
		builder.WriteString(fmt.Sprintf("**CPU使用率**: %.1f%%\n\n", cpu))
	}
	
	if memory, ok := data["memory_usage"].(float64); ok {
		builder.WriteString(fmt.Sprintf("**内存使用率**: %.1f%%\n\n", memory))
	}
	
	if errorMsg, ok := data["error"].(string); ok {
		builder.WriteString(fmt.Sprintf("**错误详情**: %s\n\n", errorMsg))
	}
	
	builder.WriteString("---\n")
	builder.WriteString(fmt.Sprintf("> 📊 集群监控 | %s", t.Cluster))

	msg := &dingtalk.Message{
		MsgType: "markdown",
		Markdown: &dingtalk.Markdown{
			Title: title,
			Text:  builder.String(),
		},
	}
	
	if level == LevelCritical {
		msg.At = &dingtalk.AtInfo{
			AtMobiles: t.AtMobiles,
			IsAtAll:   false,
		}
	}

	return msg, nil
}

// RateLimitTemplate 限流报警模板
type RateLimitTemplate struct {
	AtMobiles []string
}

func (t *RateLimitTemplate) Render(level AlertLevel, data map[string]interface{}) (*dingtalk.Message, error) {
	title := "🚦 接口限流报警"
	
	var builder strings.Builder
	builder.WriteString("### 🚦 接口限流报警\n\n")
	builder.WriteString(fmt.Sprintf("**报警时间**: %s\n\n", time.Now().Format("2006-01-02 15:04:05")))
	
	if endpoint, ok := data["endpoint"].(string); ok {
		builder.WriteString(fmt.Sprintf("**受限接口**: `%s`\n\n", endpoint))
	}
	
	if limit, ok := data["limit"].(float64); ok {
		builder.WriteString(fmt.Sprintf("**限流阈值**: %.0f QPS\n\n", limit))
	}
	
	if current, ok := data["current"].(float64); ok {
		builder.WriteString(fmt.Sprintf("**当前QPS**: %.1f\n\n", current))
	}
	
	if clientIP, ok := data["client_ip"].(string); ok {
		builder.WriteString(fmt.Sprintf("**客户端IP**: %s\n\n", clientIP))
	}
	
	if window, ok := data["window"].(string); ok {
		builder.WriteString(fmt.Sprintf("**时间窗口**: %s\n\n", window))
	}
	
	builder.WriteString("**建议操作**:\n")
	builder.WriteString("1. 检查是否有异常流量\n")
	builder.WriteString("2. 确认限流配置是否合理\n")
	builder.WriteString("3. 如需调整请联系运维\n")

	msg := &dingtalk.Message{
		MsgType: "markdown",
		Markdown: &dingtalk.Markdown{
			Title: title,
			Text:  builder.String(),
		},
		At: &dingtalk.AtInfo{
			AtMobiles: t.AtMobiles,
			IsAtAll:   false,
		},
	}

	return msg, nil
}
```

## 3. 报警管理器

```go
package alert

import (
	"sync"
	"time"
)

// AlertManager 报警管理器
type AlertManager struct {
	robot      *dingtalk.DingRobot
	templates  map[string]AlertTemplate
	alertCache sync.Map // 用于去重
	cooldown   time.Duration
	mu         sync.RWMutex
}

// NewAlertManager 创建报警管理器
func NewAlertManager(webhookURL, secret string, cooldown time.Duration) *AlertManager {
	return &AlertManager{
		robot:     dingtalk.NewDingRobot(webhookURL, secret),
		templates: make(map[string]AlertTemplate),
		cooldown:  cooldown,
	}
}

// RegisterTemplate 注册模板
func (m *AlertManager) RegisterTemplate(name string, template AlertTemplate) {
	m.mu.Lock()
	defer m.mu.Unlock()
	m.templates[name] = template
}

// SendAlert 发送报警
func (m *AlertManager) SendAlert(templateName string, level AlertLevel, data map[string]interface{}) error {
	m.mu.RLock()
	template, ok := m.templates[templateName]
	m.mu.RUnlock()
	
	if !ok {
		return fmt.Errorf("template %s not found", templateName)
	}
	
	// 去重检查
	alertKey := generateAlertKey(templateName, level, data)
	if m.shouldThrottle(alertKey) {
		return fmt.Errorf("alert throttled: %s", alertKey)
	}
	
	// 渲染消息
	msg, err := template.Render(level, data)
	if err != nil {
		return fmt.Errorf("render template error: %v", err)
	}
	
	// 发送消息
	if err := m.robot.Send(msg); err != nil {
		return fmt.Errorf("send alert error: %v", err)
	}
	
	// 记录发送时间
	m.alertCache.Store(alertKey, time.Now())
	
	return nil
}

// generateAlertKey 生成报警唯一键
func generateAlertKey(templateName string, level AlertLevel, data map[string]interface{}) string {
	key := fmt.Sprintf("%s:%s", templateName, level)
	
	// 根据模板类型提取关键信息
	if endpoint, ok := data["endpoint"].(string); ok {
		key += ":" + endpoint
	}
	if service, ok := data["service"].(string); ok {
		key += ":" + service
	}
	if host, ok := data["host"].(string); ok {
		key += ":" + host
	}
	
	return key
}

// shouldThrottle 检查是否需要限流
func (m *AlertManager) shouldThrottle(key string) bool {
	if lastTime, ok := m.alertCache.Load(key); ok {
		if time.Since(lastTime.(time.Time)) < m.cooldown {
			return true
		}
	}
	return false
}

// CleanupCache 清理过期的缓存
func (m *AlertManager) CleanupCache() {
	m.alertCache.Range(func(key, value interface{}) bool {
		if time.Since(value.(time.Time)) > m.cooldown*2 {
			m.alertCache.Delete(key)
		}
		return true
	})
}
```

## 4. 使用示例

```go
package main

import (
	"log"
	"time"
	
	"your-project/alert"
)

func main() {
	// 初始化报警管理器
	alertManager := alert.NewAlertManager(
		"https://oapi.dingtalk.com/robot/send?access_token=YOUR_TOKEN",
		"YOUR_SECRET",
		5*time.Minute, // 5分钟去重
	)
	
	// 注册模板
	serpTemplate := &alert.SerpAlertTemplate{
		ServiceName: "SERP-Service",
		AtMobiles:   []string{"13800001111", "13900002222"},
	}
	alertManager.RegisterTemplate("serp", serpTemplate)
	
	serviceTemplate := &alert.ServiceStatusTemplate{
		Cluster:   "production",
		AtMobiles: []string{"13800001111"},
	}
	alertManager.RegisterTemplate("service", serviceTemplate)
	
	rateLimitTemplate := &alert.RateLimitTemplate{
		AtMobiles: []string{"13900002222"},
	}
	alertManager.RegisterTemplate("ratelimit", rateLimitTemplate)
	
	// 示例1: SERP接口报警
	go func() {
		serpData := map[string]interface{}{
			"endpoint":    "/api/v1/search",
			"error":       "Connection timeout after 30s",
			"latency":     32000.5,
			"status_code": 504,
			"request_id":  "req_1234567890",
			"suggestion":  "检查后端服务状态和网络连接",
		}
		
		if err := alertManager.SendAlert("serp", alert.LevelError, serpData); err != nil {
			log.Printf("Send serp alert failed: %v", err)
		}
	}()
	
	// 示例2: 服务状态报警
	go func() {
		serviceData := map[string]interface{}{
			"service":       "user-service",
			"host":          "10.0.0.1",
			"port":          8080,
			"old_status":    "healthy",
			"new_status":    "unhealthy",
			"uptime":        "5d 3h 20m",
			"cpu_usage":     95.5,
			"memory_usage":  88.2,
			"error":         "Health check failed: connection refused",
		}
		
		if err := alertManager.SendAlert("service", alert.LevelCritical, serviceData); err != nil {
			log.Printf("Send service alert failed: %v", err)
		}
	}()
	
	// 示例3: 限流报警
	go func() {
		rateLimitData := map[string]interface{}{
			"endpoint":  "/api/v1/query",
			"limit":     100.0,
			"current":   150.5,
			"client_ip": "192.168.1.100",
			"window":    "1m",
		}
		
		if err := alertManager.SendAlert("ratelimit", alert.LevelWarning, rateLimitData); err != nil {
			log.Printf("Send rate limit alert failed: %v", err)
		}
	}()
	
	// 定期清理缓存
	ticker := time.NewTicker(time.Hour)
	defer ticker.Stop()
	
	for range ticker.C {
		alertManager.CleanupCache()
	}
}

// SERP监控示例
type SERPMonitor struct {
	alertManager *alert.AlertManager
	endpoint     string
}

func (m *SERPMonitor) CheckHealth() {
	// 模拟检查SERP接口
	latency := measureLatency()
	statusCode := checkStatusCode()
	
	if statusCode >= 500 {
		data := map[string]interface{}{
			"endpoint":    m.endpoint,
			"status_code": statusCode,
			"latency":     latency,
			"error":       fmt.Sprintf("Server error: %d", statusCode),
			"suggestion":  "检查后端服务和数据库连接",
		}
		
		m.alertManager.SendAlert("serp", alert.LevelError, data)
	} else if latency > 10000 { // 超过10秒
		data := map[string]interface{}{
			"endpoint":   m.endpoint,
			"latency":    latency,
			"suggestion": "优化查询性能，检查索引",
		}
		
		m.alertManager.SendAlert("serp", alert.LevelWarning, data)
	}
}
```

## 5. 配置示例

```yaml
# config.yaml
dingtalk:
  webhook: "https://oapi.dingtalk.com/robot/send?access_token=your_token"
  secret: "your_secret"
  
alerts:
  cooldown: 300  # 秒
  
templates:
  serp:
    service_name: "SERP-API"
    at_mobiles:
      - "13800001111"
      - "13900002222"
      
  service:
    cluster: "production"
    at_mobiles:
      - "13800001111"
      
  ratelimit:
    at_mobiles:
      - "13900002222"
```

## 主要特性

1. **多种报警模板**：SERP接口、服务状态、限流报警等
2. **分级报警**：INFO/WARNING/ERROR/CRITICAL四级
3. **智能去重**：防止短时间内重复报警
4. **Markdown格式**：支持富文本展示
5. **@功能**：关键报警自动@相关人员
6. **易于扩展**：可自定义新的报警模板
