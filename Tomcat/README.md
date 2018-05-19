## 调试

```shell
# 查看占用资源数
lsof|grep tomcat|wc -l
# 查看 TOMCAT PID
top
# 查看指定进程下的线程cpu占用比例
top -p 101973 -H
# 查看线程信息 0x38e7
sudo -u tomcat7 jstack 14537|grep -A 10 0x38e7
```

## 关闭日志

`logging.properties`
```text
1catalina.org.apache.juli.FileHandler.level = OFF
1catalina.org.apache.juli.FileHandler.directory = ${catalina.base}/logs
1catalina.org.apache.juli.FileHandler.prefix = catalina.

2localhost.org.apache.juli.FileHandler.level = OFF
2localhost.org.apache.juli.FileHandler.directory = ${catalina.base}/logs
2localhost.org.apache.juli.FileHandler.prefix = localhost.
```

## 线程池

`Server.xml`
```xml
    <!--The connectors can use a shared executor, you can define one or more named thread pools-->
    <Executor name="tomcatThreadPool" namePrefix="catalina-exec-"
        maxThreads="500" minSpareThreads="10" maxIdleTime="60000" />


    <!-- A "Connector" represents an endpoint by which requests are received
         and responses are returned. Documentation at :
         Java HTTP Connector: /docs/config/http.html (blocking & non-blocking)
         Java AJP  Connector: /docs/config/ajp.html
         APR (HTTP/AJP) Connector: /docs/apr.html
         Define a non-SSL HTTP/1.1 Connector on port 8080
    -->
    <Connector executor="tomcatThreadPool" port="9090" protocol="org.apache.coyote.http11.Http11NioProtocol"
               connectionTimeout="20000"
               URIEncoding="UTF-8"
	       maxKeepAliveRequests="10"
               redirectPort="8443" maxKeepAliveRequests="5" />
```