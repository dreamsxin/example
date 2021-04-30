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

## plantuml 生成图片

https://sourceforge.net/projects/plantuml/

`java -jar plantuml.jar /tmp/uml.txt -DPLANTUML_LIMIT_SIZE=8192`

plantuml 默认限制图片的长度和宽度为4096，如果图片过大会被截取，所这里使用`-DPLANTUML_LIMIT_SIZE`参数修改大小。

