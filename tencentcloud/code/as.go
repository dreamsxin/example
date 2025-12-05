package tencent

import (
	"dilu/common/config"
	"fmt"
	"log/slog"
	"sync"

	as "github.com/tencentcloud/tencentcloud-sdk-go/tencentcloud/as/v20180419"
	"github.com/tencentcloud/tencentcloud-sdk-go/tencentcloud/common"
	"github.com/tencentcloud/tencentcloud-sdk-go/tencentcloud/common/profile"
)

// AutoScalingManager 伸缩组管理器
type AutoScalingManager struct {
	client *as.Client
	region string
	mu     sync.RWMutex
}

// NewAutoScalingManager 创建伸缩组管理器
func NewAutoScalingManager(cfg config.QCloud) (*AutoScalingManager, error) {
	if cfg.SecretID == "" || cfg.SecretKey == "" {
		return nil, fmt.Errorf("secretID and secretKey are required")
	}
	if cfg.Region == "" {
		return nil, fmt.Errorf("region is required")
	}

	credential := common.NewCredential(cfg.SecretID, cfg.SecretKey)

	cpf := profile.NewClientProfile()
	cpf.HttpProfile.Endpoint = "as.tencentcloudapi.com"

	client, err := as.NewClient(credential, cfg.Region, cpf)
	if err != nil {
		slog.Error("failed to create AS client", "error", err)
		return nil, err
	}

	return &AutoScalingManager{
		client: client,
		region: cfg.Region,
	}, nil
}

// 查询伸缩组信息
func (asm *AutoScalingManager) GroupInfo(asgId string) ([]*as.AutoScalingGroup, error) {
	asm.mu.RLock()
	defer asm.mu.RUnlock()
	// 实例化一个请求对象,每个接口都会对应一个request对象
	request := as.NewDescribeAutoScalingGroupsRequest()

	request.AutoScalingGroupIds = common.StringPtrs([]string{asgId})
	// 返回的resp是一个DescribeAutoScalingGroupsResponse的实例，与请求对象对应
	response, err := asm.client.DescribeAutoScalingGroups(request)
	if err != nil {
		slog.Error("failed to get group info", "asgId", asgId, "err", err)
		return nil, err
	}
	//slog.Info("get group info", "asgId", asgId, "info", response.ToJsonString())

	return response.Response.AutoScalingGroupSet, nil
}

// 修改期望值
func (asm *AutoScalingManager) DesiredCapacity(asgId string, desiredCapacity uint64) (*as.ModifyDesiredCapacityResponse, error) {
	asm.mu.RLock()
	defer asm.mu.RUnlock()
	// 实例化一个请求对象,每个接口都会对应一个request对象
	request := as.NewModifyDesiredCapacityRequest()

	request.AutoScalingGroupId = common.StringPtr(asgId)
	request.DesiredCapacity = common.Uint64Ptr(desiredCapacity)
	//request.MinSize = common.Uint64Ptr(0)
	// 返回的resp是一个ModifyDesiredCapacityResponse的实例，与请求对象对应
	response, err := asm.client.ModifyDesiredCapacity(request)
	if err != nil {
		slog.Error("failed to modify group info", "asgId", asgId, "err", err)
		return nil, err
	}
	slog.Info("modify group info", "asgId", asgId, "info", response.ToJsonString())
	return response, nil
}
