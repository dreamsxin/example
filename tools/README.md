# 代码统计工具

## cloc

```shell
sudo apt-get install cloc
cd cphalcon/ext
cloc .
```

## sloccount

会估计出总成本。

```shell
sudo apt-get install sloccount
sloccount .
```

## wc

统计当前目录下的所有文件行数：

```shell
wc -l *
```

当前目录以及子目录的所有文件行数：

```shell
find  . * | xargs wc -l
```

## 压力测试

Siege

命令常用参数

-c 200 指定并发数200
-r 5 指定测试的次数5
-f urls.txt 制定url的文件
-i internet系统，随机发送url
-b 请求无需等待 delay=0
-t 5 持续测试5分钟

```shell
sudo apt-get install siege
siege -c 200 -r 100 http://www.google.com
```

## 安全测试工具篇


- Nmap
        网络发现和安全审计
        其基本功能包括：1. 探测主机是否在线；2. 扫描主机端口，嗅探所提供的网络服务（包括应用名称和版本信息）；3. 推断主机所用的操作系统及版本信息；4. 探测使用的防火墙/包过滤器的类型；5. 探测其他各类信息，且可定制扫描策略。
        支持多种平台（Linux, Windows, and Mac OS XA）；支持命令行和图形化界面（GUI）；可灵活绑定其他工具，如Nping, Nidiff，Ncat等
        网站：https://nmap.org/

- OpenVAS
        开放式漏洞评估
        号称世界最先进的开源漏洞扫描和管理工具。OpenVAS包括一个中央服务器和一个图形化的前端。其核心部件是中央服务器，包括一套网络漏洞测试程序，可以检测远程系统和应用程序中的安全问题。
        Nessus项目的一个分支, 服务器仅限Linux，客户端可以使Windows和Linux
        网站：https://openvas.org/

- OSSEC
        主机入侵检测
        主要功能有日志分析、完整性检查、rootkit检测、基于时间的警报和主动响应。除具有入侵检测系统功能外，还一般被用在SEM/SIM（安全事件管理/安全信息管理）解决方案中。因其强大的日志分析引擎，ISP、大学和数据中心用其监控和分析他们的防火墙、入侵检测系统、网页服务和验证等产生的日志。
        支持多种平台（Linux, OpenBSD, FreeBSD, MacOS, Solaris， Windows）
        网站：https://ossec.github.io/

- Metasploit
        渗透测试（安全漏洞检测）
        号称世界使用最多的渗透测试软件，包括扫描和审计功能。Metasploit Framework (MSF) 是一个强大的开源平台，供开发，测试和使用恶意代码，其为渗透测试、shellcode 编写和漏洞研究提供了一个可靠平台。
        支持Windows和Linux平台
        网站：https://www.metasploit.com/

- Security Onion
        入侵检测、网络安全和日志管理
        基于Ubuntu的，包含了入侵检测、网络安全监控、日志管理所需的Snort、Suricata、Bro、OSSEC、Sguil、Squert、ELSA、Xplico、NetworkMiner等众多工具。且易于安装和使用。
        仅支持Linux（Ubuntu)
        网站：https://securityonion.net/

- Snort
        入侵检测和防御
        已发展成为一个多平台(Multi-Platform),实时(Real-Time)流量分析，网络IP数据包(Pocket)记录等特性的强大的网络入侵检测/防御系统。有三种工作模式：嗅探器、数据包记录器、网络入侵检测系统。
        支持多平台（Linux，FreeBSD，Windows）
        最新版本的发布时间：2015年7月
        网站：https://www.snort.org/

- Suricata
        入侵检测和防御
        集IDS, IPS和网络安全监控为一身的引擎
        支持多种平台（Linux/Mac/FreeBSD/UNIX/Windows）
        网站：https://suricata-ids.org

- Scapy
        强大的交互式数据包处理程序
        能够伪造或者解码大量的网络协议数据包，能够发送、捕捉、匹配请求和回复包等等。它可以很容易地处理一些典型操作，比如端口扫描，tracerouting，探测，单元测试，攻击或网络发现（可替代hping，NMAP，arpspoof，ARP-SK，arping，tcpdump，tethereal，P0F等）
        python编写
        网站：http://www.secdev.org/projects/scapy/

- BackTrack
        专业的计算机安全测试/检测的Linux操作系统
        BackTrack是一套信息安全审计专用的Linux发行版。集成了包括Metasploit，RFMON，Aircrack-NG等200多种安全检查工具；其支持众多的RFID工具和ARM平台。
        最新版本的发布时间：2012年8月
        网站：http://www.backtrack-linux.org/， https://www.kali.org/， http://www.kali.org.cn/

- LMD Tool
        Linux恶意软件扫描
        网站：http://www.lmd.de/products/vcl/lmdtools/

- BFBTester
        BFBTester是一款对二进制程序进行安全检查，确定输入参数、环境变量等是否存在溢出问题的工具。
        网站：http://bfbtester.sourceforge.net/

- lynis
        安全漏洞扫描工具
        支持多平台（Linux、FreeBSD和Mac OS）
        网站：https://cisofy.com/

- Auditd
        Linux安全审计
        功能包括：账户管理， 文件系统管理， 权限管理， 日志文件操作管理

- Wireshark
        网络封包分析，可应用于网络安全测试分析
        网站：https://www.wireshark.org/

- Tcpdump/WinDump
        网络监测和数据收集嗅探，可应用于网络安全测试分析
        网站：http://www.tcpdump.org/， http://windump.polito.it/

- THC Amap
        应用软件端口检测
        用于检测软件是否在给定的端口监听。
        网站：https://www.thc.org/thc-amap/

- Cain and Abel
        有密码破解、穷举、嗅探、地址解析协议/DNS污染等功能。
        仅Windows平台
        网站：http://www.oxid.it/cain.html

- WVS
        自动化的Web应用程序安全测试
        可以扫描任何可通过Web浏览器访问的和遵循HTTP/HTTPS规则的Web站点和Web应用程序。适用于任何中小型和大型企业的内联网、外延网和面向客户、雇员、厂商和其它人员的Web网站。
        网站：http://www.acunetix.com/vulnerability-scanner/

- Acunetix
        可用于测试网站和Web应用程序的跨站脚本攻击、SQL注入攻击及其他常见的Web漏洞攻击。
        网站：http://www.acunetix.com/

- Burp Suite
        一个专门用于测试Web应用安全的完整工具包。它可以作为代理服务器、Web爬虫、入侵工具和转发工具，还可以自动发送请求。
        网站：https://portswigger.net/burp/

- Nikto
        Web服务器扫描
        其可对网页服务器进行全面的多种扫描，包含超过6400 种有潜在危险的文件/CGIs；超过1200 种服务器版本；超过270种特定服务器问题, 且扫描项和插件可以自动更新。
        可运行在任何有Perl环境的平台上, 但工具较老，最新版本发布时间是2011年2月
        网站：https://cirt.net/nikto2/
