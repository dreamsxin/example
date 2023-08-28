# 关于OpenSCA

## SCA技术
软件成分分析（Software Composition Analysis, SCA）是Gartner定义的一种应用程序安全检测技术，该技术用于分析开源软件以及第三方商业软件涉及的各种源码、模块、框架和库等，以识别和清点开源软件的组件及其构成和依赖关系，并检测是否存在已知的安全和功能漏洞、安全补丁是否已经过时或是否存在许可证合规或兼容性风险等安全问题，帮助确保企业软件供应链中组件的安全。

通过使用SCA技术快速跟踪和分析项目中引入的第三方开源组件，将开源组件及其直接和间接依赖关系、漏洞信息、开源许可证等信息生成软件物料清单(SBOM)，提供项目软件资产的完整信息，帮助对软件资产进行安全管理。

## 使用

```shell
opensca-cli.exe -config config.json
# or
opensca-cli -url https://opensca.xmirror.cn -token ${token} -path ${project_path} -out output.html
```
`config.json`
```json
{
  "path": "D:\\gowork\\src\\game-server",
  "out": "output.json",

  "vuln": false,
  "dedup": true,
  "progress": true,

  "url": "https://opensca.xmirror.cn",
  "token": "xxxx-xxx-xxx-xxx-xxx",

  "origin": {
    "": { "dsn": "" }
  },

  "maven": [
    {
      "repo": "",
      "user": "",
      "password": ""
    }
  ]
}

```
