package tencent

import (
	"dilu/common/config"
	"fmt"
	"log/slog"
	"sync"
	"time"

	"github.com/dreamsxin/go-now"
	"github.com/tencentcloud/tencentcloud-sdk-go/tencentcloud/common"
	"github.com/tencentcloud/tencentcloud-sdk-go/tencentcloud/common/profile"
	monitor "github.com/tencentcloud/tencentcloud-sdk-go/tencentcloud/monitor/v20180724"
)

// MonitorManager 监控管理器
type MonitorManager struct {
	client *monitor.Client
	mu     sync.RWMutex
}

// NewAutoScalingManager 创建伸缩组管理器
func NewMonitorManager(cfg config.QCloud) (*MonitorManager, error) {
	if cfg.SecretID == "" || cfg.SecretKey == "" {
		return nil, fmt.Errorf("secretID and secretKey are required")
	}
	if cfg.Region == "" {
		return nil, fmt.Errorf("region is required")
	}

	credential := common.NewCredential(cfg.SecretID, cfg.SecretKey)

	cpf := profile.NewClientProfile()
	cpf.HttpProfile.Endpoint = "monitor.tencentcloudapi.com"

	client, err := monitor.NewClient(credential, cfg.Region, cpf)
	if err != nil {
		slog.Error("failed to create monitor client", "error", err)
		return nil, err
	}

	return &MonitorManager{
		client: client,
	}, nil
}

// 获取监控指标数据
func (m *MonitorManager) GetCLBTotalReq(loadBalancerId string) (float64, error) {
	m.mu.RLock()
	defer m.mu.RUnlock()
	// 实例化一个请求对象,每个接口都会对应一个request对象
	request := monitor.NewGetMonitorDataRequest()

	request.Namespace = common.StringPtr("QCE/LB_PRIVATE")
	request.MetricName = common.StringPtr("ClientConnum")
	request.Instances = []*monitor.Instance{
		&monitor.Instance{
			Dimensions: []*monitor.Dimension{
				&monitor.Dimension{
					Name:  common.StringPtr("loadBalancerId"),
					Value: common.StringPtr(loadBalancerId),
				},
			},
		},
	}
	request.Period = common.Uint64Ptr(60)
	nowtime := now.EndOfMinute()
	request.StartTime = common.StringPtr(nowtime.Add(-5 * time.Minute).Format(time.RFC3339))
	request.EndTime = common.StringPtr(nowtime.Format(time.RFC3339))
	// 返回的resp是一个GetMonitorDataResponse的实例，与请求对象对应
	response, err := m.client.GetMonitorData(request)
	if err != nil {
		return 0.0, err
	}
	if len(response.Response.DataPoints) > 0 {
		values := response.Response.DataPoints[0].Values
		if len(values) > 0 {
			// 取平均值
			sum := 0.0
			for _, v := range values {
				sum += *v
			}
			avg := sum / float64(len(values))
			return avg, nil
		}
	}
	// 输出json格式的字符串回包
	slog.Info("get clb total req", "response", response.ToJsonString())
	return 1.0, nil
}

// 获取监控指标数据
func (m *MonitorManager) GetCLBClientNum(loadBalancerId string) (float64, error) {
	m.mu.RLock()
	defer m.mu.RUnlock()
	// 实例化一个请求对象,每个接口都会对应一个request对象
	request := monitor.NewGetMonitorDataRequest()

	request.Namespace = common.StringPtr("QCE/LB_PRIVATE")
	request.MetricName = common.StringPtr("ClientConnum")
	request.Instances = []*monitor.Instance{
		&monitor.Instance{
			Dimensions: []*monitor.Dimension{
				&monitor.Dimension{
					Name:  common.StringPtr("loadBalancerId"),
					Value: common.StringPtr(loadBalancerId),
				},
			},
		},
	}
	request.Period = common.Uint64Ptr(60)
	nowtime := now.EndOfMinute()
	request.StartTime = common.StringPtr(nowtime.Add(-5 * time.Minute).Format(time.RFC3339))
	request.EndTime = common.StringPtr(nowtime.Format(time.RFC3339))
	// 返回的resp是一个GetMonitorDataResponse的实例，与请求对象对应
	response, err := m.client.GetMonitorData(request)
	if err != nil {
		return 0.0, err
	}
	if len(response.Response.DataPoints) > 0 {
		values := response.Response.DataPoints[0].Values
		if len(values) > 0 {
			// 取平均值
			sum := 0.0
			for _, v := range values {
				sum += *v
			}
			avg := sum / float64(len(values))
			return avg, nil
		}
	}
	// 输出json格式的字符串回包
	slog.Info("get clb total req", "response", response.ToJsonString())
	return 1.0, nil
}
