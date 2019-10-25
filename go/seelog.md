

## 滚动日志配置

```xml
<seelog>
    <outputs formatid="main">
        <filter levels="info,debug">
                 <rollingfile type="size" filename="../log/common.log" maxsize="102400" maxrolls="5"/>
        </filter>
        <filter levels="error,critical">
                <rollingfile type="size" filename="../log/error.log" maxsize="102400" maxrolls="5"/>
        </filter>
    </outputs>
    <formats>
        <format id="main" format="%Date(2006-01-02 15:04:05.999999) [%File.%Line] %LEV %Msg%n"/>
    </formats>
</seelog>
```