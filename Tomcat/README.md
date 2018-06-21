## 安装

```shell
sudo add-apt-repository ppa:webupd8team/java
sudo apt-get install tomcat8
```

## 自定义安装

* 安装 Java

```shell
sudo apt-get install default-jdk 
update-alternatives --config java 
```

* 增加 JAVA_HOME

```shell
nano /etc/environment
```
`JAVA_HOME="/usr/lib/jvm/java-8-openjdk-amd64/jre/bin/java"`
 
```shell
source /etc/environment
echo $JAVA_HOME 
```

```shell
groupadd tomcat
useradd -s /bin/false -g tomcat -d /opt/tomcat tomcat
```
 

*  下载 Tomcat.

```shell
wget http://ftp.mirror.tw/pub/apache/tomcat/tomcat-8/v8.5.31/bin/apache-tomcat-8.5.31.tar.gz
mkdir /opt/tomcat
tar xvf apache-tomcat-8*tar.gz -C /opt/tomcat --strip-components=1
cd /opt/
sudo chown -R tomcat tomcat/
```

* 配置 Tomcat

创建配置文件
```shell
nano /etc/systemd/system/tomcat.service 
```
内容如下
```text
[Unit]
Description=Apache Tomcat Web Application Container
After=network.target

[Service]
Type=forking

Environment=JAVA_HOME=/usr/lib/jvm/java-1.8.0-openjdk-amd64/jre
Environment=CATALINA_PID=/opt/tomcat/temp/tomcat.pid
Environment=CATALINA_HOME=/opt/tomcat
Environment=CATALINA_BASE=/opt/tomcat
Environment='CATALINA_OPTS=-Xms512M -Xmx1024M -server -XX:+UseParallelGC'
Environment='JAVA_OPTS=-Djava.awt.headless=true -Djava.security.egd=file:/dev/./urandom'

ExecStart=/opt/tomcat/bin/startup.sh
ExecStop=/opt/tomcat/bin/shutdown.sh

User=tomcat
Group=tomcat
UMask=0007
RestartSec=10
Restart=always

[Install]
WantedBy=multi-user.target
```
Reload the SystemD daemon so that the service settings that were added are included.
```shell
sudo systemctl daemon-reload
```
Start the Tomcat service.
```shell
sudo systemctl start tomcat
sudo systemctl status tomcat 
```

## 调试

```shell
# 查看占用资源数
lsof|grep tomcat|wc -l
# 查看 TOMCAT PID
top
# or
jps -m
# 查看指定进程下的线程cpu占用比例
top -p 101973 -H
# 查看线程信息 0x38e7
sudo -u tomcat7 jstack 14537|grep -A 10 0x38e7
```

```shell
#!/bin/bash  

# 入参只有一个，即目标java的pid，如果没有，则默认找cpu最高的java进程  
if [ -z "$1" ]; then  
    ### 1.先找到消耗cpu最高的Java进程 ###  
    pid=`ps -eo pid,%cpu,cmd --sort=-%cpu | grep java | grep -v grep | head -1 | awk 'END{print $1}' `  
    if [ "$pid" =  ""  ]; then  
	    echo "无Java进程，退出。"  
	    exit  
    fi  
else  
    pid=$1  
fi  

### 2.生成dump后的文件名 ###  
curTime=$(date +%Y%m%dT%H:%M:%S)  
# jstack后的文件会加上时间，便于对一个进程dump多次  
dumpFilePath="/tmp/pid-$pid-$curTime.jstack"  
echo -e "cpu最高的java进程: "`jps | grep $pid`"\n" > $dumpFilePath  

### 3.取到该进程的所有线程及其cpu(只显示cpu大于0.0的线程) ###  
echo -e "进程内线程cpu占比如下（不显示cpu占比为0的线程）:\n" >> $dumpFilePath  
ps H -eo pid,tid,%cpu --sort=-%cpu | grep $pid | awk '$3 > 0.0 {totalCpu+=$3; printf("nid=0x%x, cpu=%s\n", $2, $3) >> "'$dumpFilePath'"}  
END{printf("cpu总占比:%s\n\n", totalCpu) >> "'$dumpFilePath'"}'  

### 4.dump该进程 ###  
echo -e "如下是原生jstack后的结果:\n" >> $dumpFilePath  
jstack -l $pid >> $dumpFilePath  

echo "dump成功，请前往查看(文件名包含时间，为了采集更准确，可以多执行几次该命令):" $dumpFilePath  

exit
```

## 打印日志

* 控制台
```shell
tail -f /var/log/tomcat8/catalina.out
```

* 输出到控制台
```java
System.out.print(jb.toString());
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

<Executor name="tomcatThreadPool" namePrefix="catalina-exec-" 
	maxThreads="500" minSpareThreads="100" prestartminSpareThreads = "true" maxQueueSize = "100" />

<Connector executor="tomcatThreadPool" port="8080" protocol="org.apache.coyote.http11.Http11Nio2Protocol" 
	connectionTimeout="20000" maxConnections="10000" redirectPort="8443" enableLookups="false" 
	acceptCount="100" maxPostSize="10485760" compression="on" disableUploadTimeout="true" 
	compressionMinSize="2048" acceptorThreadCount="2" 
	compressableMimeType="text/html,text/xml,text/plain,text/css,text/javascript,application/javascript" 
	URIEncoding="utf-8" />
```

```conf
upstream tomcats {
    server localhost:8080 weight=1;
}
server {
        listen 8181;
        server_name localhost;
        underscores_in_headers on;
        location / {
		proxy_redirect off;
		proxy_buffering off;

		proxy_http_version 1.1;
		proxy_request_buffering off;
		proxy_pass http://tomcats;
        }
}
```