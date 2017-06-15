# 编译

## 执行

```shell
./curl-loader -f 10K.conf
```

`conf-examples/10k.conf` 配置如下： 

```conf
########### GENERAL SECTION ################################ 
BATCH_NAME= 10K                    // 这里是配置测试对象的名称 
CLIENTS_NUM_MAX=10000              // 这里是配置最大虚拟用户数 
CLIENTS_NUM_START=100              // 这里是配置初始化并发虚拟用户数 
CLIENTS_RAMPUP_INC=50              // 这里是配置每次加载用户数 
INTERFACE   =eth0    
NETMASK=16                         // 子网掩码位数 
IP_ADDR_MIN= 192.168.1.1           // 起始ip地址 
IP_ADDR_MAX= 192.168.53.255        // 最后的ip地址范围 
CYCLES_NUM= -1 
URLS_NUM= 1 
########### URL SECTION #################################### 
URL=http://localhost/index.html    // 配置URL地址 
URL_SHORT_NAME="local-index" 
REQUEST_TYPE=GET                   // 请求类型
TIMER_URL_COMPLETION = 5000        // 时间限制，毫秒
TIMER_AFTER_URL_SLEEP =20 
```

说明：
```conf
########### GENERAL SECTION ##################
BATCH_NAME= 10K-clients     #批处理名称
CLIENTS_NUM_MAX=10000       #虚拟客户端最大数量
CLIENTS_NUM_START=100       #虚拟客户端初始数量
CLIENTS_RAMPUP_INC=50       #虚拟客户端一次递增的数量
INTERFACE =eth0             #选择使工作的网卡
NETMASK=255.255.0.0         #子网掩码
IP_ADDR_MIN= 192.168.1.1    #模拟的ip开始地址段
IP_ADDR_MAX= 192.168.53.255 #模拟的ip结束地址段
CYCLES_NUM= -1              #每个客户执行的周期数不限
URLS_NUM= 1                 #每个客户得到单个url
########### URLs SECTION #######################
URL=http://localhost/index.html #待测目标的url
URL_SHORT_NAME="local-index"    #这个名字是url的代名词将被出现在负载开始时的
REQUEST_TYPE=GET                #请求类型
TIMER_URL_COMPLETION = 0        #url将使用http GET方法被提取且在时间上不受限制
TIMER_AFTER_URL_SLEEP = 0       #在完成一个url请求后进行下一个url请求的协议重定向的间隔时间
```