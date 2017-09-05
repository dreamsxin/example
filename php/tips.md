# 知识点


## 语法

一个字符串可以用 4 种方式表达： 

- 单引号 
- 双引号 
- heredoc 语法结构 （双引号）
- nowdoc 语法结构（单引号）

## 函数

- php_sapi_name - 返回 web 服务器和 PHP 之间的接口类型
- php_uname - 返回运行 PHP 的系统的有关信息
- php_strip_whitespace - 返回删除注释和空格后的PHP源码
- spl_autoload_register - 注册__autoload()函数
- set_exception_handler - 设置一个用户定义的异常处理函数
- restore_exception_handler
- set_error_handler - 设置一个用户定义的错误处理函数
- restore_error_handler
- trigger_error - 产生一个用户级别的 error/warning/notice 信息
- debug_zval_dump
- debug_print_backtrace
- var_export - 输出或返回一个变量的字符串表示
- var_dump
- scandir - 列出指定路径中的文件和目录
- strspn - 计算字符串中全部字符都存在于指定字符集合中的第一段子串的长度
- substr_replace - 替换字符串的子串
- array_chunk - 将一个数组分割成多个
- array_map - 将回调函数作用到给定数组的单元上 
- array_reduce - 用回调函数迭代地将数组简化为单一的值

## 常量

- PHP_OS
- PHP_SAPI
- PHP_VERSION
- DIRECTORY_SEPARATOR - 目录分隔符
- PHP_EOL - 换行符

## 全局变量

- $_SESSION
- $_COOKIE
- $_POST
- $_GET

## 类

- ReflectionClass
- DirectoryIterator
- SplFixedArray
- SplDoublyLinkedList
- SplFileInfo
- SplFileObject

## 魔术方法

- __construct
- __destruct
- __call
- __callStatic
- __get
- __set
- __isset
- __unset
- __sleep
- __wakeup
- __toString
- __invoke
- __set_state
- __clone

## 代码风格

- 缩进
- 命名
- 注释

### PSR 规范

- PSR-0(Autoloading Standard) 即类自动加载规范，该规范被标记为 Deprecated，由PSR-4替代。它的内容十分简洁。
- PSR-1(Basic Coding Standard) 即基础编码标准，包含了类文件、类名、类方法名的命名方法。
- PSR-2(Coding Style Guide) 即编码风格标准，它以PSR-1为基础，包含了缩进、每行代码长度、换行、方法可见性声明、空格和方法体大括号换行的相关规定。
- PSR-3(Logger Interface) PSR-3是对应用日志类的通过接口的定义。
- PSR-4(Improved Autoloading) 即改进版的自动加载规范，它是PSR-0规范的接替者。它可以与任何其它的自动加载规范兼容，包括PSR-0。

## 基本功

网际层协议：IP协议、ICMP协议、ARP协议、RARP协议。
传输层协议：TCP协议、UDP协议。
应用层协议：FTP、Telnet、SMTP、HTTP、RIP、NFS、DNS。

* 按由低到高的顺序，写出OSI七层模型。
* DNS轮询属于OSI模型中的哪一层。
* 简述 array_map 和 array_reduce 的作用。
* 尽可能多的写出 PHP CLI的命令参数。
* Interface 互相之间是否能够继承。
* Session 销毁之后是否会重新生成新的会话 ID。
* foreach 中使用引用需要注意什么。

- php --help
- php --ini
- php -m

## 实战经验

* 订单ID生成，如何保证唯一

* 改了系统时间，是否会影响 Redis 过期时间

* 改了主机系统时间，是否会影响 Docker 时间

* HTTP 协议中，content-type 作用是什么？

MediaType，即是Internet Media Type，互联网媒体类型；也叫做MIME类型，在Http协议消息头中，使用Content-Type来表示具体请求中的媒体类型信息。

* 请写出 ajax 实现跨域的两种回调方式？

- 以 jsonp 的数据类型进行请求时，JQ会动态在页面中添加script标签，将请求内容放置到src中。
- 服务器端在返回资源的时候，指定这个资源能被哪些域所访问。
- 代理中转

* Pcntl、Phalcon两种扩展你用过哪一个，并简述其作用。

- Pcntl PHP的进程控制支持实现了Unix方式的进程创建，程序执行，信号处理以及进程的中断。

* phpize命令的作用是什么？

根据系统信息生成对应的 configure 文件

* PHP 并发编程中，常用于防止进程成为僵尸进程的函数是？

- 父进程通过 `pcntl_wait` 和 `pcntl_waitpid` 等函数等待子进程结束
- pcntl_signal(SIGCHLD, function(){pcntl_wait($status)});

* Mysql查看主从库状态的命令是什么？

- show master/slave status;

* 请写出 Linux 下编译安装 PHP 扩展（假设要编译安装扩展为mbstring，PHP源码在目录/home/my/php-src/）的过程 和 相关shell命令？

## 扩展知识

* 1.是否实现过一个框架或者库以供别人使用么？如果有，请谈一谈构建框架或者库时候的经验；如果没有，请设想如果设计一个框架，并指出大概需要如何做、需要注意一些什么方面，来使别人容易地使用您的框架。

* 2.是否做过异步的网络处理和通讯方面的工作？如果有，能具体介绍一些实现策略么？

* 3.软件开发的步骤、分工、工作配合。

* 4.消息推送是怎么现实的？

* 5.是否实现过任务队列？

## 面向对象

* 1、面向对象的三个基本特征是什么？
* 2、get_object_vars 在类中使用和类外使用，返回值有何不同？
* 3、use 在类中使用、类外使用及function后使用有何区别？
* 4、构建链式语法的关键语句是return什么？
* 5、PHP接口类、抽象类的声明关键字是什么？

## 架构能力

* 1.请写出您所知道的，除了分层架构外的其他架构模式，并简述其优缺点。

- 分层架构 (Layered Architecture)

它是最通用的架构，也被叫做N层架构模式(n-tier architecture pattern)。这也是Java EE应用经常采用的标准模式。基本上是个程序员都知道它。这种架构模式非常适合传统的IT通信和组织结构，很自然地成为大部分应用的第一架构选择。

- 事件驱动架构 (Event-Driven Architecture)

事件驱动架构是一个流行的分布式异步架构模式，可以用来设计规模很大的应用程序。基于这种架构模式应用可大可小。它由高度解耦的，单一目的的事件处理组件组成，可以异步地接收和处理事件。
它包括两个主要的拓扑结构：mediator 和 broker。Mediator拓扑结构需要你在一个事件通过mediator时精心安排好几个步骤，而broker拓扑结构无需mediator，而是由你串联起几个事件。这两种拓扑架构的特征和实现有很大的不同，所以你需要知道哪一个适合你。

- 微内核架构 (Microkernel Architecture)

微内核架构模式通常又被成为插件架构模式，可以用来实现基于产品的应用, 比如Eclipse，在微内核的基础上添加一些插件，就可以提供不同的产品，如C++, Java等。

- 基于空间的架构 (Space-Based Architecture)

基于空间的架构有时候也被成为基于云的架构。
大部分的基于web的应用的业务流都是一样的。 客户端的请求发送给web服务器，然后是应用服务器，最后是数据库服务器。对于用户很小时不会有问题，但是负载增大时就会遇到瓶颈（想想抢火车票）。首先是web服务器撑不住，web服务器能撑住应用服务器又不行，然后是数据库服务器。通常解决方案是增加web服务器，便宜，简单，但很多情况下负载会传递给应用服务器，然后传递给数据库服务器。有时候增加数据库服务器也没有办法，因为数据库也有锁，有事务的限制。
基于空间的架构用来解决规模和并发的问题。

* 2.面对一个中大型BS架构的工程，简述您如何平衡性能和效率？
* 3.假若当前有一个公司产品线非常丰富，由于历史原因需要全部重构，且部署方案已经确定采用docker，在两台服务器上进行集群化，为了配合方案，您将采用什么样的架构来重构并简述原因？
* 4.针对上述第三个问题给出的情景，写出您认为最合理的架构和部署方式。

## 质量把控能力

* 1.写出您常用的php依赖管理功能。

* 2.写出您用来管理WEB前端依赖关系的工具名称，分别从Javascript和CSS说明。

* 3.注释到文档的映射工具，您使用过那些？

* 4.请尽可能多的写出PHPDOC注释规范中的关键字和作用，如：@class 声明类的描述

## 面试

### 1、关于学习的欲望

“你给自己设定的职业发展目标是什么？你准备如何去实现你的这个目标”，判断点，由面试官根据他的职业目标的判断，如何去实现通过什么样的途径我们客观看出一个人的学习欲望。

### 2、关于学习的时间

“首先要问一下现在的工作忙不忙（适用于有工作经验同时尚未离职者，对于其他的人需使用其他的技巧），以及加班多不多等”。判断点，根据他的答复我们需要判断他一天中上班的时间长短，之后我们还要看着他的简历提问“你的兴趣有那些？或者你喜欢打球吗？”我们来整体判断一天24小时内他多少时间用来学习，这是我们需要的答案。

### 3、对于理解和接受能力

这个我们可以在笔试中进行，要么使用逻辑题的方式，要么使用复杂案例的方式，这些都可以测试出来。

### 4、对于总结能力。

问题是“请问你大学/工作期间最成功/最失败的事情是什么呢？请给我们讲讲怎么回事”判断点：看是否能够通过事件来总结自己的不足或者成功之处。
