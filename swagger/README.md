## 参数
```text
swag init -h
NAME:
   swag init - Create docs.go

USAGE:
   swag init [command options] [arguments...]

OPTIONS:
   --quiet, -q                            Make the logger quiet. (default: false)
   --generalInfo value, -g value          Go file path in which 'swagger general API Info' is written (default: "main.go")
   --dir value, -d value                  Directories you want to parse,comma separated and general-info file must be in the first one (default: "./")
   --exclude value                        Exclude directories and files when searching, comma separated
   --propertyStrategy value, -p value     Property Naming Strategy like snakecase,camelcase,pascalcase (default: "camelcase")
   --output value, -o value               Output directory for all the generated files(swagger.json, swagger.yaml and docs.go) (default: "./docs")
   --outputTypes value, --ot value        Output types of generated files (docs.go, swagger.json, swagger.yaml) like go,json,yaml (default: "go,json,yaml")
   --parseVendor                          Parse go files in 'vendor' folder, disabled by default (default: false)
   --parseDependency, --pd                Parse go files inside dependency folder, disabled by default (default: false)
   --markdownFiles value, --md value      Parse folder containing markdown files to use as description, disabled by default
   --codeExampleFiles value, --cef value  Parse folder containing code example files to use for the x-codeSamples extension, disabled by default
   --parseInternal                        Parse go files in internal packages, disabled by default (default: false)
   --generatedTime                        Generate timestamp at the top of docs.go, disabled by default (default: false)
   --parseDepth value                     Dependency parse depth (default: 100)
   --requiredByDefault                    Set validation required for all fields by default (default: false)
   --instanceName value                   This parameter can be used to name different swagger document instances. It is optional.
   --overridesFile value                  File to read global type overrides from. (default: ".swaggo")
   --parseGoList                          Parse dependency via 'go list' (default: true)
   --tags value, -t value                 A comma-separated list of tags to filter the APIs for which the documentation is generated.Special case if the tag is prefixed with the '!' character then the APIs with that tag will be excluded
   --templateDelims value, --td value     Provide custom delimeters for Go template generation. The format is leftDelim,rightDelim. For example: "[[,]]"
   --collectionFormat value, --cf value   Set default collection format (default: "csv")
   --help, -h                             show help (default: false)
--generalInfo value, -g value          API通用信息所在的go源文件路径，如果是相对路径则基于API解析目录 (默认: "main.go")
   --dir value, -d value                  API解析目录 (默认: "./")
   --exclude value                        解析扫描时排除的目录，多个目录可用逗号分隔（默认：空）
   --propertyStrategy value, -p value     结构体字段命名规则，三种：snakecase,camelcase,pascalcase (默认: "camelcase")
   --output value, -o value               文件(swagger.json, swagger.yaml and doc.go)输出目录 (默认: "./docs")
   --parseVendor                          是否解析vendor目录里的go源文件，默认不
   --parseDependency                      是否解析依赖目录中的go源文件，默认不
   --markdownFiles value, --md value      指定API的描述信息所使用的markdown文件所在的目录
   --generatedTime                        是否输出时间到输出文件docs.go的顶部，默认是
   --codeExampleFiles value, --cef value  解析包含用于 x-codeSamples 扩展的代码示例文件的文件夹，默认禁用
   --parseInternal                        解析 internal 包中的go文件，默认禁用
   --parseDepth value                     依赖解析深度 (默认: 100)
   --instanceName value                   设置文档实例名 (默认: "swagger")
```

## 使用

```shell
swag init --parseDependency --parseInternal
```
