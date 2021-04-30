```shell
go get git.oschina.net/jscode/go-package-plantuml
go build git.oschina.net/jscode/go-package-plantuml
```

- --codedir 目标分析的代码目录
- --gopath 本地设置GOPATH环境变量目录
- --outputfile 分析结果保存到该文件
- --ignoredir 不需要进行代码分析的目录（可以不用设置）

```shell
./go-package-plantuml --gopath go --codedir go/src/github.com/go-kit/kit/ --outputfile kit.txt
```
