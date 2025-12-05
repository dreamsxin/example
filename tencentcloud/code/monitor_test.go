package example

import (
	"fmt"
	"testing"

	"github.com/tencentcloud/tencentcloud-sdk-go/tencentcloud/common"
	"github.com/tencentcloud/tencentcloud-sdk-go/tencentcloud/common/errors"
	"github.com/tencentcloud/tencentcloud-sdk-go/tencentcloud/common/profile"
	monitor "github.com/tencentcloud/tencentcloud-sdk-go/tencentcloud/monitor/v20180724"
)

// 获取监控指标属性
func TestGetCLBMetrics(t *testing.T) {
	credential := common.NewCredential(AccessKeyId, AccessKeySecret)
	// 使用临时密钥示例
	// credential := common.NewTokenCredential("SecretId", "SecretKey", "Token")
	// 实例化一个client选项，可选的，没有特殊需求可以跳过
	cpf := profile.NewClientProfile()
	cpf.HttpProfile.Endpoint = "monitor.tencentcloudapi.com"
	// 实例化要请求产品的client对象,clientProfile是可选的
	client, _ := monitor.NewClient(credential, "na-siliconvalley", cpf)

	// 实例化一个请求对象,每个接口都会对应一个request对象
	request := monitor.NewDescribeBaseMetricsRequest()

	request.Namespace = common.StringPtr("QCE/LB_PUBLIC")
	//request.Namespace = common.StringPtr("QCE/LB_PRIVATE")
	request.MetricName = common.StringPtr("TotalReq")
	// 返回的resp是一个DescribeBaseMetricsResponse的实例，与请求对象对应
	response, err := client.DescribeBaseMetrics(request)
	if err != nil {
		if _, ok := err.(*errors.TencentCloudSDKError); ok {
			t.Logf("An API error has returned: %s", err)
			return
		}
		panic(err)
	}
	// 输出json格式的字符串回包
	t.Logf("%s", response.ToJsonString())
}

// 伸缩组id: asg-6bch7foc
// clb id: lb-990pbe4c

/*
{
	"Response": {
		"Period": 60,
		"MetricName": "TotalReq",
		"DataPoints": [{
			"Dimensions": [{
				"Name": "loadBalancerId",
				"Value": "lb-990pbe4c"
			}],
			"Timestamps": [1764894900, 1764894960, 1764895020, 1764895080, 1764895140, 1764895200, 1764895260, 1764895320, 1764895380, 1764895440, 1764895500, 1764895560, 1764895620, 1764895680, 1764895740, 1764895800],
			"Values": [0.15, 0.15, 0.2, 0.15, 0.15, 0.2, 0.15, 0.15, 0.2, 0.15, 0.15, 0.2, 0.15, 0.15, 0.2, 0.15]
		}],
		"StartTime": "2025-12-05 08:35:00",
		"EndTime": "2025-12-05 08:50:00",
		"Msg": "Success",
		"RequestId": "75735f47-4a72-4854-8d9e-f9e7057632c2"
	}
}
*/
// 获取监控指标数据
func TestGetCLBMetricDatas(t *testing.T) {
	credential := common.NewCredential(AccessKeyId, AccessKeySecret)
	// 使用临时密钥示例
	// credential := common.NewTokenCredential("SecretId", "SecretKey", "Token")
	// 实例化一个client选项，可选的，没有特殊需求可以跳过
	cpf := profile.NewClientProfile()
	cpf.HttpProfile.Endpoint = "monitor.tencentcloudapi.com"
	// 实例化要请求产品的client对象,clientProfile是可选的
	client, _ := monitor.NewClient(credential, "na-siliconvalley", cpf)

	// 实例化一个请求对象,每个接口都会对应一个request对象
	request := monitor.NewGetMonitorDataRequest()

	request.Namespace = common.StringPtr("QCE/LB_PUBLIC")
	request.MetricName = common.StringPtr("TotalReq")
	request.Instances = []*monitor.Instance{
		&monitor.Instance{
			Dimensions: []*monitor.Dimension{
				&monitor.Dimension{
					Name:  common.StringPtr("loadBalancerId"),
					Value: common.StringPtr("lb-990pbe4c"),
				},
			},
		},
	}
	request.Period = common.Uint64Ptr(60)
	request.StartTime = common.StringPtr("2025-12-05 08:35:00")
	request.EndTime = common.StringPtr("2025-12-05 08:50:00")
	// 返回的resp是一个GetMonitorDataResponse的实例，与请求对象对应
	response, err := client.GetMonitorData(request)
	if _, ok := err.(*errors.TencentCloudSDKError); ok {
		t.Logf("An API error has returned: %s", err)
		return
	}
	if err != nil {
		panic(err)
	}
	// 输出json格式的字符串回包
	t.Logf("%s", response.ToJsonString())
}

// 获取多个指标数据
func TestGetCLBMetricDatas2(t *testing.T) {
	credential := common.NewCredential(AccessKeyId, AccessKeySecret)
	// 使用临时密钥示例
	// credential := common.NewTokenCredential("SecretId", "SecretKey", "Token")
	// 实例化一个client选项，可选的，没有特殊需求可以跳过
	cpf := profile.NewClientProfile()
	cpf.HttpProfile.Endpoint = "monitor.tencentcloudapi.com"
	// 实例化要请求产品的client对象,clientProfile是可选的
	client, _ := monitor.NewClient(credential, "na-siliconvalley", cpf)

	// 实例化一个请求对象,每个接口都会对应一个request对象
	request := monitor.NewDescribeStatisticDataRequest()

	request.Module = common.StringPtr("monitor")
	request.Namespace = common.StringPtr("QCE/LB_PUBLIC")
	request.MetricNames = common.StringPtrs([]string{"ClientNewConn", "TotalReq"})
	// 返回的resp是一个DescribeStatisticDataResponse的实例，与请求对象对应
	response, err := client.DescribeStatisticData(request)
	if _, ok := err.(*errors.TencentCloudSDKError); ok {
		fmt.Printf("An API error has returned: %s", err)
		return
	}
	if err != nil {
		panic(err)
	}
	// 输出json格式的字符串回包
	fmt.Printf("%s", response.ToJsonString())
}
