package example

import (
	"fmt"
	"testing"

	as "github.com/tencentcloud/tencentcloud-sdk-go/tencentcloud/as/v20180419"
	"github.com/tencentcloud/tencentcloud-sdk-go/tencentcloud/common"
	"github.com/tencentcloud/tencentcloud-sdk-go/tencentcloud/common/errors"
	"github.com/tencentcloud/tencentcloud-sdk-go/tencentcloud/common/profile"
)

/*
	{
		"Response": {
			"AutoScalingGroupSet": [{
				"AutoScalingGroupId": "asg-6bch7foc",
				"AutoScalingGroupName": "serp-us-auto",
				"AutoScalingGroupStatus": "NORMAL",
				"CreatedTime": "2025-12-02T08:54:20Z",
				"DefaultCooldown": 300,
				"DesiredCapacity": 0,
				"EnabledStatus": "ENABLED",
				"ForwardLoadBalancerSet": [{
					"LoadBalancerId": "lb-990pbe4c",
					"ListenerId": "lbl-kuushcl6",
					"TargetAttributes": [{
						"Port": 10,
						"Weight": 10
					}],
					"LocationId": "loc-7tzcdhsk",
					"Region": "na-siliconvalley"
				}],
				"InstanceCount": 0,
				"InServiceInstanceCount": 0,
				"LaunchConfigurationId": "asc-48ehu4pw",
				"LaunchConfigurationName": "serp-us-auto",
				"LoadBalancerIdSet": [],
				"MaxSize": 100,
				"MinSize": 0,
				"ProjectId": 0,
				"SubnetIdSet": ["subnet-no67j4m8", "subnet-459mqqaq"],
				"TerminationPolicySet": ["OLDEST_INSTANCE"],
				"VpcId": "vpc-b1s6aejd",
				"ZoneSet": [],
				"RetryPolicy": "IMMEDIATE_RETRY",
				"InActivityStatus": "NOT_IN_ACTIVITY",
				"Tags": [],
				"ServiceSettings": {
					"ReplaceMonitorUnhealthy": false,
					"ScalingMode": "CLASSIC_SCALING",
					"ReplaceLoadBalancerUnhealthy": false,
					"ReplaceMode": "RECREATE",
					"AutoUpdateInstanceTags": false,
					"DesiredCapacitySyncWithMaxMinSize": false,
					"PriorityScaleInUnhealthy": false
				},
				"Ipv6AddressCount": 0,
				"MultiZoneSubnetPolicy": "PRIORITY",
				"HealthCheckType": "CLB",
				"LoadBalancerHealthCheckGracePeriod": 0,
				"InstanceAllocationPolicy": "LAUNCH_CONFIGURATION",
				"SpotMixedAllocationPolicy": {},
				"CapacityRebalance": false
			}],
			"TotalCount": 1,
			"RequestId": "ae86c9b3-8563-4aed-8cdb-a421b29dd5ad"
		}
	}
*/
// 查询伸缩组信息
func TestTencentAsGroupInfo(t *testing.T) {
	// 密钥信息从环境变量读取，需要提前在环境变量中设置 TENCENTCLOUD_SECRET_ID 和 TENCENTCLOUD_SECRET_KEY
	// 使用环境变量方式可以避免密钥硬编码在代码中，提高安全性
	// 生产环境建议使用更安全的密钥管理方案，如密钥管理系统(KMS)、容器密钥注入等
	// 请参见：https://cloud.tencent.com/document/product/1278/85305
	// 密钥可前往官网控制台 https://console.cloud.tencent.com/cam/capi 进行获取
	credential := common.NewCredential(AccessKeyId, AccessKeySecret)
	// 使用临时密钥示例
	// credential := common.NewTokenCredential("SecretId", "SecretKey", "Token")
	// 实例化一个client选项，可选的，没有特殊需求可以跳过
	cpf := profile.NewClientProfile()
	cpf.HttpProfile.Endpoint = "as.tencentcloudapi.com"
	// 实例化要请求产品的client对象,clientProfile是可选的
	client, _ := as.NewClient(credential, "na-siliconvalley", cpf)

	// 实例化一个请求对象,每个接口都会对应一个request对象
	request := as.NewDescribeAutoScalingGroupsRequest()

	request.AutoScalingGroupIds = common.StringPtrs([]string{"asg-6bch7foc"})
	// 返回的resp是一个DescribeAutoScalingGroupsResponse的实例，与请求对象对应
	response, err := client.DescribeAutoScalingGroups(request)
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

// 修改期望值
func TestTencentDesiredCapacity(t *testing.T) {
	credential := common.NewCredential(AccessKeyId, AccessKeySecret)

	cpf := profile.NewClientProfile()
	//cpf.HttpProfile.Endpoint = "as.na-siliconvalley.tencentcloudapi.com"
	// 实例化要请求产品的client对象,clientProfile是可选的
	client, _ := as.NewClient(credential, "na-siliconvalley", cpf)

	// 实例化一个请求对象,每个接口都会对应一个request对象
	request := as.NewModifyDesiredCapacityRequest()

	request.AutoScalingGroupId = common.StringPtr("asg-6bch7foc")
	request.DesiredCapacity = common.Uint64Ptr(1)
	//request.MinSize = common.Uint64Ptr(0)
	// 返回的resp是一个ModifyDesiredCapacityResponse的实例，与请求对象对应
	response, err := client.ModifyDesiredCapacity(request)
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

// 扩容到指定数量
func TestTencentAsGroupScale(t *testing.T) {
	credential := common.NewCredential(AccessKeyId, AccessKeySecret)

	// 使用临时密钥示例
	// credential := common.NewTokenCredential("SecretId", "SecretKey", "Token")
	// 实例化一个client选项，可选的，没有特殊需求可以跳过
	cpf := profile.NewClientProfile()
	cpf.HttpProfile.Endpoint = "as.na-siliconvalley.tencentcloudapi.com"
	// 实例化要请求产品的client对象,clientProfile是可选的
	client, _ := as.NewClient(credential, "na-siliconvalley", cpf)

	// 实例化一个请求对象,每个接口都会对应一个request对象
	request := as.NewScaleOutInstancesRequest()

	request.AutoScalingGroupId = common.StringPtr("asg-6bch7foc")
	request.ScaleOutNumber = common.Uint64Ptr(1)
	// 返回的resp是一个ScaleOutInstancesResponse的实例，与请求对象对应
	response, err := client.ScaleOutInstances(request)
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

// 获取伸缩组实例信息
func TestTencentAsGroupInstances(t *testing.T) {
	credential := common.NewCredential(AccessKeyId, AccessKeySecret)

	// 使用临时密钥示例
	// credential := common.NewTokenCredential("SecretId", "SecretKey", "Token")
	// 实例化一个client选项，可选的，没有特殊需求可以跳过
	cpf := profile.NewClientProfile()
	cpf.HttpProfile.Endpoint = "as.na-siliconvalley.tencentcloudapi.com"
	// 实例化要请求产品的client对象,clientProfile是可选的
	client, _ := as.NewClient(credential, "na-siliconvalley", cpf)

	// 构造请求参数
	request := as.NewDescribeAutoScalingInstancesRequest()
	request.Filters = []*as.Filter{
		{
			Name:   common.StringPtr("auto-scaling-group-id"),
			Values: []*string{common.StringPtr("asg-6bch7foc")},
		},
	}
	request.Limit = common.Int64Ptr(100)

	var allInstances []*as.Instance
	var offset int64 = 0

	// 分页获取所有实例
	for {
		request.Offset = common.Int64Ptr(offset)

		response, err := client.DescribeAutoScalingInstances(request)
		if err != nil {
			t.Error(err)
			break
		}

		t.Logf("DescribeAutoScalingInstances offset:%d, instances:%s", offset, response.ToJsonString())
		if response.Response.AutoScalingInstanceSet == nil || len(response.Response.AutoScalingInstanceSet) == 0 {
			break
		}

		allInstances = append(allInstances, response.Response.AutoScalingInstanceSet...)

		if len(response.Response.AutoScalingInstanceSet) < 100 {
			break
		}

		offset += 100
	}
}
