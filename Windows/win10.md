## Powershell ExecutionPolicy 执行策略

```shell
Get-ExecutionPolicy -List #查看当前的执行策略
Set-ExecutionPolicy -Scope CurrentUser RemoteSigned #设置执行策略为要求远程脚本签名，范围为当前用户
```

* 策略 Policies：

`Restricted / AllSigned / RemoteSigned / Unrestricted / Bypass / Undefined`

* 范围 Scopes：

`Process / CurrentUser / LocalMachine`


临时使用，不想修改执行策略为较低的权限，可使用：
```shell
Set-ExecutionPolicy Bypass -Scope Process -Force; npm-windows-upgrade
```
开发人员需频繁使用powershell脚本，允许本地脚本运行，可使用：
```shell
Set-ExecutionPolicy RemoteSigned -Scope CurrentUser
```
