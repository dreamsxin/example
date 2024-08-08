- Cmdlet	Alias	说明
- Get-History	h	获取命令历史记录。
- Invoke-History	r	运行命令历史记录中的命令。
- Add-History		将命令添加到命令历史记录。
- Clear-History	clhy	从命令历史记录中删除命令。
- Get-PSReadLineOption 查找Powershell历史记录存放位置

```shell
cd  $env:APPDATA\Microsoft\Windows\PowerShell\PSReadLine
```

## 等待 Loading 效果

```shell
$spinner = @('-', '\', '|', '/') # 定义旋转字符集合
for($i=0; $i -lt 100; $i++) { # 循环100次，可以根据需要调整
    Write-Host -NoNewline "`r $($spinner[$i % 4]) Loading" # 显示旋转字符
    Start-Sleep -Milliseconds 100 # 等待100毫秒
}
Write-Host "`r Done!" # 循环结束后显示完成信息
```
