package example

import (
	"testing"

	clb "github.com/tencentcloud/tencentcloud-sdk-go/tencentcloud/clb/v20180317"
	"github.com/tencentcloud/tencentcloud-sdk-go/tencentcloud/common"
	"github.com/tencentcloud/tencentcloud-sdk-go/tencentcloud/common/errors"
	"github.com/tencentcloud/tencentcloud-sdk-go/tencentcloud/common/profile"
)

func TestDescribeLoadBalancers(t *testing.T) {
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
	cpf.HttpProfile.Endpoint = "clb.tencentcloudapi.com"
	// 实例化要请求产品的client对象,clientProfile是可选的
	client, _ := clb.NewClient(credential, "na-siliconvalley", cpf)

	// 实例化一个请求对象,每个接口都会对应一个request对象
	request := clb.NewDescribeLoadBalancersRequest()

	//request.LoadBalancerType = common.StringPtr("OPEN")     // 仅查询公网负载均衡器
	request.LoadBalancerType = common.StringPtr("INTERNAL") // 仅查询内网负载均衡器
	// 返回的resp是一个DescribeLoadBalancersResponse的实例，与请求对象对应
	response, err := client.DescribeLoadBalancers(request)
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
