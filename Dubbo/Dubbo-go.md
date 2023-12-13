# 安装 Dubbo-go 开发环境

https://cn.dubbo.apache.org/zh-cn/overview/quickstart/go/install/

## 安装 dubbogo-cli
```shell
go install github.com/dubbogo/dubbogo-cli@latest
```
## 安装依赖的工具插件

安装 protoc
https://github.com/protocolbuffers/protobuf/releases
protoc-25.0-win64.zip
```shell
protoc --version
```

```shell
dubbogo-cli install all
```
```out
go install github.com/dubbogo/tools/cmd/imports-formatter@latest
go install github.com/dubbogo/tools/cmd/protoc-gen-dubbo3grpc@latest
go install github.com/dubbogo/tools/cmd/protoc-gen-go-triple@v1.0.10-
```
```shell
protoc-gen-go-triple --version
```
## 创建项目
```shell
mkdir quickstart
cd quickstart 
dubbogo-cli newDemo .
# 查看目录
tree .
```

```shell
go mod tidy
```
可手动编译测试 pb 接口文件是否正确
```shell
$ cd api
$ protoc --go_out=. --go-triple_out=. ./samples_api.proto
```
运行测试，在 go-server/cmd 和 go-client/cmd 文件夹下分别执行 go run . , 可在客户端看到输出：

`client response result: name:"Hello laurence" id:"12345" age:21`
可能还有一行
```out
config/root_config.go:150       [Config Center] Config center doesn't start
```
