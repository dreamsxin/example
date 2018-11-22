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

## 查看线程数

```shell
ps -ef|grep tomcat
ps -Lf pid |wc -l
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

## 设置tomcat线程池大小

众所周知，tomcat接受一个request后处理过程中，会涉及到cpu和IO时间。其中IO等待时间，cpu被动放弃执行，其他线程就可以利用这段时间片进行操作。所以我们可以采用服务器IO优化的通用规则。
线程大小 = ( (线程io时间 + 线程cpu) / 线程cpu time) * cpu核数

线程io时间为100ms(IO操作比如数据库查询，同步远程调用等)，线程cpu时间10ms，服务器物理机核数为4个。
通过上面的公式，我们计算出来的大小是 ((100 + 10 )/10 ) *4 = 44。理论上我们有依据，但是实际计算过程中我们怎么知道线程IO时间和cpu时间呢？

## 创建新实例

```shell
sudo apt-get install tomcat8-user
cd /opt
sudo tomcat8-instance-create -p 8080 -c 8081 tomcat8-php
sudo chown tomcat8:root ./tomcat8-php -R
sudo ln -s /etc/tomcat8/policy.d /opt/tomcat8-php/conf/policy.d
sudo cp /etc/init.d/tomcat8 /etc/init.d/tomcat8-php
```


编辑 `/opt/tomcat8-php/conf/server.xml`：
```xml
<Server port="8006" shutdown="SHUTDOWN">
	<Executor name="tomcatThreadPool" namePrefix="catalina-exec-" 
		maxThreads="500" minSpareThreads="100" prestartminSpareThreads = "true" maxQueueSize = "100" />

	<Connector executor="tomcatThreadPool" port="8081" protocol="org.apache.coyote.http11.Http11Nio2Protocol" 
		connectionTimeout="3000" redirectPort="8443" enableLookups="false" 
		acceptCount="100"
	/>
</Server>
```

编辑 `/etc/init.d/tomcat8-php`：
```conf
#!/bin/sh
#
# /etc/init.d/tomcat8 -- startup script for the Tomcat 8 servlet engine
#
# Written by Miquel van Smoorenburg <miquels@cistron.nl>.
# Modified for Debian GNU/Linux by Ian Murdock <imurdock@gnu.ai.mit.edu>.
# Modified for Tomcat by Stefan Gybas <sgybas@debian.org>.
# Modified for Tomcat6 by Thierry Carrez <thierry.carrez@ubuntu.com>.
# Modified for Tomcat7 by Ernesto Hernandez-Novich <emhn@itverx.com.ve>.
# Additional improvements by Jason Brittain <jason.brittain@mulesoft.com>.
#
### BEGIN INIT INFO
# Provides:          tomcat8-php
# Required-Start:    $local_fs $remote_fs $network
# Required-Stop:     $local_fs $remote_fs $network
# Should-Start:      $named
# Should-Stop:       $named
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Short-Description: Start Tomcat.
# Description:       Start the Tomcat servlet engine.
### END INIT INFO

NAME=tomcat8-php
DESC="Tomcat Pumba servlet engine"
#DEFAULT=/etc/default/$NAME
DEFAULT=/etc/default/tomcat8
JVM_TMP=/tmp/$NAME-tmp

CATALINA_HOME=/usr/share/tomcat8
#CATALINA_BASE=/var/lib/$NAME
CATALINA_BASE=/opt/tomcat8-php
```

```shell
sudo update-rc.d tomcat8-php defaults 90
```

## jvm参数说明

-  -server 一定要作为第一个参数，启用JDK的server版本，在多个CPU时性能佳
-  -Xms java Heap初始大小。 默认是物理内存的1/64。
-  -Xmx java heap最大值。建议均设为物理内存的80%。不可超过物理内存。
-  -Xmn java heap最小值，一般设置为Xmx的3、4分之一。
-  -XX:PermSize 设定内存的永久保存区初始大小，缺省值为64M。
-  -XX:MaxPermSize 设定内存的永久保存区最大大小，缺省值为64M。
-  -XX:SurvivorRatio=2 生还者池的大小，默认是2。如
-  -XX:NewSize 新生成的池的初始大小。 缺省值为2M。
-  -XX:MaxNewSize 新生成的池的最大大小。 缺省值为32M。
-  +XX:AggressiveHeap 让jvm忽略Xmx参数，疯狂地吃完一个G物理内存，再吃尽一个G的swap。
-  -Xss 每个线程的Stack大小
-  -verbose:gc 现实垃圾收集信息
-  -Xloggc:gc.log 指定垃圾收集日志文件
-  -XX:+UseParNewGC 缩短minor收集的时间
-  -XX:+UseConcMarkSweepGC 缩短major收集的时间
-  -XX:userParNewGC 可用来设置并行收集(多CPU)
-  -XX:ParallelGCThreads 可用来增加并行度(多CPU)
-  -XX:UseParallelGC 设置后可以使用并行清除收集器(多CPU)

建议和注意事项：

- Java 8 以后 -XX:PermSize 与 -XX:MaxPermSize 两个配置项被废弃
- -Xms和-Xmx选项设置为相同堆内存分配，以避免在每次GC 后调整堆的大小，堆内存建议占内存的60%~80%;非堆内存是不可回收内存，大小视项目而定;线程栈大小推荐256k.

32G内存配置如下：

`JAVA_OPTS="-Xms20480m -Xmx20480m -Xss1024K -XX:PermSize=512m -XX:MaxPermSize=2048m"`


## 管理操作

1、连接数

假设Tomcat接收http请求的端口是8080，则可以使用如下语句查看连接情况：

```shell
netstat –nat | grep 8080
```

2、线程

ps命令可以查看进程状态，如执行如下命令：

```shell	
ps –e | grep java
```

通过如下命令，可以看到该进程内有多少个线程；其中，nlwp含义是number of light-weight process。

```shell
ps –o nlwp 27989
```

要想获得真正在running的线程数量，可以通过以下语句完成：

```shell	
ps -eLo pid,stat | grep 27989 | grep running | wc -l
```
其中ps -eLo pid ,stat可以找出所有线程，并打印其所在的进程号和线程当前的状态；两个grep命令分别筛选进程号和线程状态；wc统计个数。

## Server Status 使用

```shell
sudo apt-get install tomcat8-admin
```

`/etc/tomcat8/tomcat-users.xml`

```xml
<tomcat-users xmlns="http://tomcat.apache.org/xml"
              xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
              xsi:schemaLocation="http://tomcat.apache.org/xml tomcat-users.xsd"
              version="1.0">
<!--
  NOTE:  By default, no user is included in the "manager-gui" role required
  to operate the "/manager/html" web application.  If you wish to use this app,
  you must define such a user - the username and password are arbitrary.
-->
<!--
  NOTE:  The sample user and role entries below are wrapped in a comment
  and thus are ignored when reading this file. Do not forget to remove
  <!.. ..> that surrounds them.
-->
  <role rolename="admin"/>
  <role rolename="admin-gui"/>
  <role rolename="admin-script"/>
  <role rolename="manager-gui"/>
  <role rolename="manager-script"/>
  <role rolename="manager-jmx"/>
  <role rolename="manager-status"/>
  <user username="admin" password="admin" roles="admin,admin-gui,admin-script,manager-gui,manager-script,manager-jmx,manager-status"/>
</tomcat-users>
```

新建 `conf/Catalina/localhost/manager.xml` 文件：
```xml
<Context privileged="true" antiResourceLocking="false"
         docBase="${catalina.home}/webapps/manager">
    <Valve className="org.apache.catalina.valves.RemoteAddrValve" allow="^.*$" />
</Context>
```

### 查看状态

curl -u admin:admin 'http://localhost:8080/manager/status'

### Manager 支持的命令

Manager 应用能够处理的命令都是通过下面这样的请求 URL 来指定的：

`http://{host}:{port}/manager/text/{command}?{parameters}`

`{host}` 和 `{port}` 分别代表运行 Tomcat 服务器所在的主机名和端口号。`{command}` 代表所要执行的 Manager 命令。`{parameters}` 代表该命令所专有的查询参数。在后面的实例中，可以为你的安装自定义适当的主机和端口。

这些命令通常是被 HTTP GET 请求执行的。`/deploy` 命令有一种能够被 HTTP PUT 请求所执行的形式。

* 常见参数

多数 Manager 命令都能够接受一个或多个查询参数，这些查询参数如下所示：

-    path 要处理的 Web 应用的上下文路径（包含前面的斜杠）。要想选择 ROOT Web 应用，指定 / 即可。注意：无法对 Manager 应用自身执行管理命令。
-    version 并行部署 所用的 Web 应用版本号。
-    war Web 应用归档（WAR）文件的 URL，或者含有 Web 应用的目录路径名，或者是上下文配置 .xml 文件。你可以按照以下任一格式使用 URL：
-        file:/absolute/path/to/a/directory 解压缩后的 Web 应用所在的目录的绝对路径。它将不做任何改动，直接附加到你所指定的上下文路径上。
-        file:/absolute/path/to/a/webapp.war Web 应用归档（WAR）文件的绝对路径。只对 /deploy 命令有效，也是该命令所唯一能接受的格式。
-        jar:file:/absolute/path/to/a/warfile.war!/ 本地 WAR 文件的 URL。另外，为了能够完整引用一个JAR 文件，你可以采用 JarURLConnection 类的任何有效语法。
-        file:/absolute/path/to/a/context.xml Web 应用上下文配置 .xml 文件的绝对路径，上下文配置文件包含着上下文配置元素。
-        directory 主机应用的基本目录中的 Web 应用的目录名称。
-        webapp.war 主机应用的基本目录中的 WAR 文件的名称