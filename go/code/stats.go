package stats

import (
	"fmt"
	"log/slog"
	"sync"
	"time"
)

type RequestStats struct {
	RequestType  string
	TotalCount   int64
	SuccessCount int64
	FailCount    int64
	Latencies    []time.Duration
	SendDingTime time.Time
	lock         sync.RWMutex
}

type StatsRequest struct {
	RequestType string
	Success     bool
	Latency     time.Duration
}

var requestStatsMap = make(map[string]*RequestStats)
var statsLock sync.RWMutex
var statsChan chan StatsRequest
var statsWorkerOnce sync.Once

func init() {
	// 初始化通道并启动后台处理 goroutine
	statsChan = make(chan StatsRequest, 10000) // 缓冲通道，避免阻塞
	startStatsWorker()
}

func startStatsWorker() {
	statsWorkerOnce.Do(func() {
		go statsWorker()
	})
}

func statsWorker() {
	for req := range statsChan {
		stats := getRequestStats(req.RequestType)
		stats.record(req.Success, req.Latency)
	}
}

func getRequestStats(requestType string) *RequestStats {
	statsLock.RLock()
	stats, ok := requestStatsMap[requestType]
	statsLock.RUnlock()

	if !ok {
		statsLock.Lock()
		// Double check
		stats, ok = requestStatsMap[requestType]
		if !ok {
			stats = &RequestStats{
				RequestType: requestType,
				Latencies:   make([]time.Duration, 0),
			}
			requestStatsMap[requestType] = stats
		}
		statsLock.Unlock()
	}
	return stats
}

func (s *RequestStats) record(success bool, latency time.Duration) {
	s.lock.Lock()
	defer s.lock.Unlock()

	s.TotalCount++
	if success {
		s.SuccessCount++
	} else {
		s.FailCount++
	}
	s.Latencies = append(s.Latencies, latency)

	if len(s.Latencies) >= 100 {
		s.Latencies = s.Latencies[100:]
	}

	if s.FailCount > 1000 && (float64(s.SuccessCount)/float64(s.SuccessCount+s.FailCount)) < 0.8 && (s.SendDingTime.IsZero() || time.Since(s.SendDingTime) > 10*time.Minute) {
		slog.Debug("RecordRequest sendding", "stats", s)
		SendDingErrorMsg(fmt.Sprintf("%s - t: %s, ok: %d, fail: %d", time.Now().Format("2006-01-02 15:04:05"), s.RequestType, s.SuccessCount, s.FailCount))
		s.SendDingTime = time.Now()
		s.SuccessCount = 0
		s.FailCount = 0
	}
}

func RecordRequest(requestType string, success bool, latency time.Duration) {
	select {
	case statsChan <- StatsRequest{
		RequestType: requestType,
		Success:     success,
		Latency:     latency,
	}:
		// 成功写入通道
	default:
		// 通道已满，记录日志但不阻塞
		slog.Warn("Stats channel is full, dropping request", "requestType", requestType)
	}
}
