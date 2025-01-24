```bat
@echo 启用Dump

reg add "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\Windows Error Reporting\LocalDumps"
reg add "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\Windows Error Reporting\LocalDumps" /v DumpFolder /t REG_EXPAND_SZ /d "C:\xxx" /f
reg add "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\Windows Error Reporting\LocalDumps" /v DumpType /t REG_DWORD /d 2 /f
reg add "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\Windows Error Reporting\LocalDumps" /v DumpCount /t REG_DWORD /d 10 /f

@echo Dump已经启用
```

## 参数说明
- DumpFolder

dump文件生成路径 eg:C:\xxx

- DDumpType

0 = Create a custom dump //自定义dump 
1 = Mini dump  //mini核心dump文件
2 = Full dum  //所有dump文件，产生的文件较大

- DDumpCount

默认:10

## 方法二：生动创建转储文件
当windows应用程序出现无响应时，可以打开任务管理器，找到无响应的进程，右键选择创建转储文件即可，即生成崩溃对应的dmp文件，该文件一般位于 C:\Users\happy\AppData\Local\Temp目录(显然这需要用户自己动手，不合适)

## 方法三：通过代码设置异常回调函数
使用windows系统api，程序中加入存储Dump的代码。通过SetUnhandledExceptionFilter
设置捕获dump的入口，然后通过MiniDumpWriteDump生成dump文件
