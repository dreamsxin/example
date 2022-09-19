# Lint
Lint 是用来进行代码的静态分析工具。既在不运行代码的前提下，找出代码中不规范以及存在bug的地方。存在各种各样的lint

## GolangCI-Lint

GolangCI-Lint 是一个lint聚合器，它的速度很快，平均速度是gometalinter的5倍。它易于集成和使用，具有良好的输出并且具有最小数量的误报。而且它还支持go modules。最重要的是免费开源。

### 安装

大多数安装都是为CI（continuous integration）准备的，强烈推荐安装固定版本的golangci-lint。
```shell
// 二进制文件将会被安装在$(go env GOPATH)/bin/golangci-lint目录
curl -sfL https://install.goreleaser.com/github.com/golangci/golangci-lint.sh | sh -s -- -b $(go env GOPATH)/bin vX.Y.Z
// 或者安装它到./bin/目录
curl -sfL https://install.goreleaser.com/github.com/golangci/golangci-lint.sh | sh -s vX.Y.Z
// 在alpine Linux中，curl不是自带的，你需要使用下面命令
wget -O - -q https://install.goreleaser.com/github.com/golangci/golangci-lint.sh | sh -s vX.Y.Z
```

使用`golangci-lint --version`来查看它的版本。

* 本地安装
```shell
go get -u github.com/golangci/golangci-lint/cmd/golangci-lint

O111MODULE=on go get -u github.com/golangci/golangci-lint/cmd/golangci-lint@v1.16.0

cd $(go env GOPATH)/src/github.com/golangci/golangci-lint/cmd/golangci-lint
go install -ldflags "-X 'main.version=$(git describe --tags)' -X 'main.commit=$(git rev-parse --short HEAD)' -X 'main.date=$(date)'"
```

## 使用
```shell
golangci-lint run main.go
```

## 支持的linter
可以传入参数 `-E/--enable` 来使某个linter可用，也可以使用`-D/--disable`参数来使某个linter不可用。例如：
```shell
golangci-lint help linters #查看它支持的linters
golangci-lint run --disable-all -E errcheck
```
