# SystemTap

https://github.com/myaut/dtrace-stap-book
https://github.com/tfiedor/perun

调试信息主要有下面几种：stabs，COFF，PE-COFF，OMF，IEEE-695和DWARF。其中DWARF在Linux中被普遍使用，我们主要分析它。
DWARF的全称是"Debugging With Attributed Record Formats"，遵从GNU FDL授权。现在已经有dwarf1，dwarf2，dwarf3三个版本。

查看内容的工具常用的有四种：
1. readelf
GNU提供的二进制工具，功能很多，并不限于读dwarf信息
2. gdb
3. drawfdump
是一个被打包在libdwarf内的程序
4. libdwarf
是一个封装好的C库API，用来读取dwarf信息

使用`gcc –g hello.c –o hello`编译生成hello文件。
`Hello`文件是`elf`格式的，由多个节(section)组成。调试信息被包含在某几个节中，如果是用`dwarf2`格式编译的，这些节的名字一般是以`.debug`开头，如`.debug_info`，`.debug_line`，`.debug_frame`等，如果是用`dwarf1`格式编译的，这些节的名字一般是`.debug`，`.line`等。
现在的编译器默认大多数是`dwarf2`格式编译，当然可以通过`gcc`的编译选项`-gdwarf-version`改变。

使用`readelf –S hello`命令查看hello文件调试信息。

## 安装

```shell
sudo apt-get install systemtap  
```

* 检测是否支持
```shell
sudo stap -ve 'probe begin { log("hello world") exit() }'
```

## 使用

`stap -l` 可以查看某个函数在哪个文件的哪一行定义的，可以是内核代码也可以是用户态代码。
```shell
stap -l 'process("/usr/lib64/libjemalloc.so.2").function("malloc")'
```
输出
`process("/usr/lib64/libjemalloc.so.2").function("malloc@/usr/include/stdlib.h:465")`

`stap -L` 可以看函数中的哪些变量可以被看到，后面写脚本时可以直接引用这些变量：
```shell
stap -L 'process("/usr/lib64/libjemalloc.so.2").function("malloc")'
```
复杂一点的打点、统计之类的可以写stap脚本来完成，语法类似c语言，也比较简答，这里贴一个做内存分析的脚本。
```stp
probe begin {
    printf("=============begin============\n")
}

//记录每次内存分配的size
global g_mem_ref_tbl
//记录内存分配的堆栈
global g_mem_bt_tbl
global cnt
global m_cnt
global str
probe process("/usr/lib64/libjemalloc.so.2").function("malloc").return {
    if (target() == pid()) {
      cnt++
      if ($size > 1024*1024) { // 1M
        g_mem_ref_tbl[$return] = $size
        g_mem_bt_tbl[$return] = sprint_ubacktrace()
        //g_mem_bt_tbl[$return] = sprint_ubacktrace_brief()
        //str = sprint_ubacktrace()
        //g_mem_bt_tbl[$return] = sprint_ustack(ubacktrace)
        m_cnt++
      }
    }
}

probe process("/usr/local/lib/libjemalloc.so.2").function("free").call {
    if (target() == pid()) {
      //内存释放就将这两个变量删除
      g_mem_ref_tbl[$ptr] = 0
      delete g_mem_ref_tbl[$ptr]
      delete g_mem_bt_tbl[$ptr]
    }
}

probe end {
    //最后输出未释放内存的分配堆栈
    total_size=0
    printf("=============end============\n")
    printf("total alloc count %d, mcnt=%d\n", cnt, m_cnt)
    foreach(mem in g_mem_bt_tbl) {
      if (g_mem_ref_tbl[mem] > 1024 * 1024) {
        total_size+=g_mem_ref_tbl[mem]
        printf("---------%d--------\n", g_mem_ref_tbl[mem])
        printf("%s\n", g_mem_bt_tbl[mem])
        printf("--------end--------\n")
      } else {
        printf("error %d, %s\n", g_mem_ref_tbl[mem], g_mem_bt_tbl[mem])
      }
    }
    printf("total size = %d\n", total_size)
}

//程序计时功能，60s后退出
probe timer.ms(60000)
{
    exit()
}
```

## SystemTap实用实例

* 监控某个进程函数的性能
```stp
#show exec time of function(@2) and the tid who execute it
global t, call_time
probe process(@1).function(@2).call
{
    t = gettimeofday_ns();
} 
probe process(@1).function(@2).return
{
    call_time <<< gettimeofday_ns()-t;
} 
probe timer.s(1)
{
    if(@count(call_time)>0)
    {
        printf("avg time %d total :%d/n", @avg(call_time), @count(call_time));
        delete call_time;
    }
     else
    {
        println("no call");
    }
} 
probe begin
{
     println("Ready");
}
```

该脚本用于测试由参数传递的函数运行时所占用的时间，即每秒打印一次平均执行时间，以及调用次数。通过这样一个脚本可以大概的确定我们要probe的函数的性能以及它相对于线程的使用时间。“<<<”表示数组操作,把上面的function改为statement，可probe行范围。@avg(call_time)计算数组内所有元素和的平均值，count(call_time)计算数组元素个数。

* 监控所有的系统调用
```stp
global syscalllist
probe begin {
    printf("System Call Monitoring Started (10 seconds).../n")
} 
probe syscall.*
{
    syscalllist[pid(), execname()]++
} 
probe timer.ms(10000) {
    foreach ( [pid, procname] in syscalllist ) {
        printf("%s[%d] = %d/n", procname, pid, syscalllist[pid, procname] )
    }
    exit()
}
```

统计10s内的所有系统调用及次数。
 
* 监控某个程序的所有系统调用
```stp
global syscalllist
probe begin {
    printf("Syslog Monitoring Started (10 seconds).../n")
} 
probe syscall.*
{
    if (execname() == @1) {
        syscalllist[name]++
    }
} 
probe timer.ms(10000) {
    foreach ( name in syscalllist ) {
        printf("%s = %d/n", name, syscalllist[name] )
    }
    printf("------------------------/n");
}
```

* 收集网络包

```stp
global recv, xmit
 
probe begin {
    printf("Starting network capture (Ctl-C to end)/n")
} 
probe netdev.receive {
    recv[dev_name, pid(), execname()] <<< length
} 
probe netdev.transmit {
    xmit[dev_name, pid(), execname()] <<< length
} 
probe end {
    printf("/nEnd Capture/n/n") 
    printf("Iface Process........ PID.. RcvPktCnt XmtPktCnt/n") 
    foreach ([dev, pid, name] in recv) {
        recvcount = @count(recv[dev, pid, name])
        xmitcount = @count(xmit[dev, pid, name])
    printf( "%5s %-15s %-5d %9d %9d/n", dev, name, pid, recvcount, xmitcount )
    } 
    delete recv
    delete xmit
}
```

count计算某个进程接收和发送的报文数，通过avg可计算收到（发送）的报文平均长度，sum可计算收到（发送）的报文的总长度。该收集的数据与dstat有点相似，但它又能够指出具体的进程使用情况，比dstat要详细些。

* 监听某个signal
```stap
probe signal.send{
    if (sig_name == "SIGKILL")
        printf("%s was send to %s(pid:%d) by %s uid:%d", sig_name, pid_name, sig_pid, execname(),uid())
}
```
```shell
stap -L signal.send
sudo stap sig_send.stp
```


* 监测某个函数的某个区间执行情况（行
对于行经常出现行地址错误，所以多调整一下用`-L`单独测试
```stp
probe process(@1).statement(@2)
{
  num++;
  t = gettimeofday_ns();
}
probe process(@1).statement(@3)
{
  call_time = gettimeofday_ns()-t;
  printf("\n%s exec time:%d", @3,call_time);
}
```
```shell
sudo stap insline.stp "libexec/mysqld" "*@/home/xxx/mysql-5.1.48/sql/sql_select.cc:11388" "*@/home/xxx/mysql-5.1.48/sql/sql_select.cc:11391" 
```
 

通过使用`-L`可以测试某条语句是否是正确的：
```shell
sudo stap -L 'process("/usr/local/mysql/libexec/mysqld").function("apply_event")'
sudo stap -L 'process("libexec/mysqld").statement("*@/home/xxx/mysql-5.1.48/sql/sql_select.cc:11388")'
```
