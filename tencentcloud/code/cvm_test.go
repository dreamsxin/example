package example

import (
	"fmt"
	"testing"

	"github.com/tencentcloud/tencentcloud-sdk-go/tencentcloud/common"
	"github.com/tencentcloud/tencentcloud-sdk-go/tencentcloud/common/errors"
	"github.com/tencentcloud/tencentcloud-sdk-go/tencentcloud/common/profile"
	cvm "github.com/tencentcloud/tencentcloud-sdk-go/tencentcloud/cvm/v20170312"
)

func TestCvmDescribeInstances(t *testing.T) {

	Region := "na-siliconvalley"
	credential := common.NewCredential(AccessKeyId, AccessKeySecret)
	client, err := cvm.NewClient(credential, Region, profile.NewClientProfile())
	if err != nil {
		t.Logf("NewClient err:%+v", err)
	}
	req := cvm.NewDescribeInstancesRequest()
	res, err := client.DescribeInstances(req)
	if err != nil {
		t.Logf("DescribeInstances err:%+v", err)
	}
	t.Logf("%#v", res.ToJsonString())
}

// 创建实例
func TestCvmCreateInstance(t *testing.T) {

	Region := "na-siliconvalley"

	credential := common.NewCredential(AccessKeyId, AccessKeySecret)
	client, err := cvm.NewClient(credential, Region, profile.NewClientProfile())
	if err != nil {
		t.Logf("NewClient err:%+v", err)
	}

	request := cvm.NewRunInstancesRequest()
	request.DryRun = common.BoolPtr(true)
	// 实例启动模板ID，通过该参数可使用实例模板中的预设参数创建实例。
	// request.LaunchTemplate = &cvm.LaunchTemplate{
	// 	LaunchTemplateId: common.StringPtr("lt-9h9h9h9h"),
	// 	//LaunchTemplateVersion: common.Uint64Ptr(1),
	// }

	request.ImageId = common.StringPtr("img-8toqc6s3")
	request.Placement = &cvm.Placement{
		Zone: common.StringPtr("na-siliconvalley-1"),
	}

	request.InstanceChargeType = common.StringPtr("POSTPAID_BY_HOUR")
	request.InstanceCount = common.Int64Ptr(1)
	request.InstanceName = common.StringPtr("test-API-SDK-GO")
	request.InstanceType = common.StringPtr("S2.SMALL1")
	request.InternetAccessible = &cvm.InternetAccessible{
		InternetChargeType:      common.StringPtr("BANDWIDTH_POSTPAID_BY_HOUR"),
		InternetMaxBandwidthOut: common.Int64Ptr(10),
		PublicIpAssigned:        common.BoolPtr(true),
	}
	request.LoginSettings = &cvm.LoginSettings{
		Password: common.StringPtr("passw0rdExample"),
	}
	request.SecurityGroupIds = common.StringPtrs([]string{"sg-icy671l9"})
	request.SystemDisk = &cvm.SystemDisk{
		DiskSize: common.Int64Ptr(50),
		DiskType: common.StringPtr("CLOUD_BASIC"),
	}
	request.VirtualPrivateCloud = &cvm.VirtualPrivateCloud{
		SubnetId: common.StringPtr("subnet-b1wk8b10"),
		VpcId:    common.StringPtr("vpc-8ek64x3d"),
	}

	// get response structure
	res, err := client.RunInstances(request)
	// API errors
	if _, ok := err.(*errors.TencentCloudSDKError); ok {
		fmt.Printf("An API error has returned: %s", err)
		return
	}
	// unexpected errors
	if err != nil {
		panic(err)
	}
	t.Logf("%#v", res.ToJsonString())
}

// 退还实例
func TestCvmTerminateInstances(t *testing.T) {

	Region := "na-siliconvalley"

	credential := common.NewCredential(AccessKeyId, AccessKeySecret)
	client, err := cvm.NewClient(credential, Region, profile.NewClientProfile())
	if err != nil {
		t.Logf("NewClient err:%+v", err)
	}

	request := cvm.NewTerminateInstancesRequest()
	request.InstanceIds = common.StringPtrs([]string{"ins-12345678"})
	// get response structure
	res, err := client.TerminateInstances(request)
	// API errors
	if _, ok := err.(*errors.TencentCloudSDKError); ok {
		fmt.Printf("An API error has returned: %s", err)
		return
	}
	// unexpected errors
	if err != nil {
		panic(err)
	}
	t.Logf("%#v", res.ToJsonString())
}
