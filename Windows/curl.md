
## 安装 cURL

- https://curl.se/windows/

## 内置

Invoke-WebRequest 是 PowerShell 中用于执行 HTTP、HTTPS 和 FTP 请求的 cmdlet，支持发送 GET、POST、PUT 等类型的请求，并可配置认证、超时、重试等参数。 ‌

### 基础语法

```shell
Invoke-WebRequest [-Uri] <Uri> [-Method <WebRequestMethod>] [-Headers <IDictionary>] [-Body <Object>]  
```

- -Uri‌：指定目标资源的 URI。
- -Method‌：HTTP 请求方法（如 GET、POST）。
- -Headers‌：自定义 HTTP 头。
- -Body‌：请求正文内容。
‌
### 高级功能

- 代理设置‌：可通过 -Proxy 参数配置 HTTP/HTTPS/SOCKS 代理，或通过环境变量 http_proxy、https_proxy 设置。 ‌‌
- 认证方式‌：支持基本认证、证书认证等，可通过 -Credential、-CertificateThumbprint 等参数配置。 ‌
- 重试机制‌：支持设置最大重试次数、重试间隔等参数。 ‌

### 注意事项

在 PowerShell 5.1 及更高版本中，wget 是 `Invoke-WebRequest` 的别名。 ‌‌
默认使用基本解析（Basic Parsing），可通过 `-UseDefaultCredentials` 参数切换为默认凭证认证。 ‌
