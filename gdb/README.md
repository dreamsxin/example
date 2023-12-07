## 原理
gdb 进程接管进程的所有信号，也就是说系统向test进程发送的所有信号，都被gdb进程接收到，这样一来，test进程的执行就被gdb控制了，从而达到调试的目的。

调用系统函数
- ptrace(PTRACE_TRACEME，[其他参数])
- ptrace(PTRACE_ATTACH,[其他参数])
### 参数
- PTRACE_TRACEME：用于将当前进程标记为被跟踪的目标。调用进程使用这个类型请求后，它的父进程可以使用 PTRACE_ATTACH 来附加到它，对其进行调试和跟踪。
- PTRACE_ATTACH：用于将一个进程附加到另一个进程上进行调试和跟踪。调试器进程可以使用这个类型请求，通过指定目标进程ID来附加到目标进程。
- PTRACE_DETACH：用于从一个已经被附加和调试的进程上分离调试器。这个请求会停止对目标进程的跟踪，并将其恢复为正常运行状态。
- PTRACE_PEEKDATA：用于从目标进程的内存中读取数据。可以使用该请求来读取目标进程的内存值，例如寄存器、栈帧等。
- PTRACE_POKEDATA：用于向目标进程的内存中写入数据。可以使用该请求来修改目标进程的内存值，例如修改寄存器、改变变量值等。
- PTRACE_GETREGS：用于获取目标进程的寄存器值。通过这个请求，可以获得目标进程的 CPU 寄存器的当前值，用于调试和跟踪。
- PTRACE_SETREGS：用于设置目标进程的寄存器值。通过这个请求，可以将特定的寄存器值设置为目标进程中的特定值。
- PTRACE_CONT：用于继续执行已附加的目标进程。调试器进程可以使用这个请求来继续目标进程的执行，直到下一个断点或者其他事件触发。

### 信号
- SIGSTOP
- 
## 前期准备

在程序编译的时候，函数名、变量名会被相应的内存地址所代替，但这些内存地址对于我们来说是不可读的。我们往往需要需要根据函数名、变量名来查找实际的地址，从而读取其值。

为了方便调试，`gcc`增加了一个`-g`参数，在编译的时候可以在程序集中添加相应的符号信息。

```shell
gcc -g hello.c -o hello
```

## 启动GDB

启动GDB的方法有以下几种：

- gdb <program>		用gdb关联程序，可以通过run命令启动关联上的程序进程。
- gdb <program> core    用gdb调试core文件，core是程序非法执行后core dump后产生的文件。
- gdb <program> <PID>	用gdb调试当前正在运行的进程，类似VS中的"附加到进程"方式的调试。

这些方式实际上也可以自由组合使用。例如，我们也可以先通过gdb <program>关联程序，然后通过attach <pid>的方式调试当前正在运行的进程。

## GDB环境简介

首先介绍下GDB环境，GDB也是一个Shell式的交互环境，在GDB中，可以进程看到其提示符：(gdb)

```shell
(gdb) break
```

在gdb中，也想VS那样有一定的智能提示功能，具体如下：

    输入命令时，可以不用打全命令，只用打命令的前几个字符就可以了。
    当输入前几个字符后，可以通过Tab键自动补全
    当前几个字符开头的命令较多时，Tab键无法补全，再次敲TAB键可以显示所有该字符开头的命令。
    可以通过回车键重复执行命令（单步跟踪的时候常用）
    可以通过上下键切换历史命令

例如，设置断点的命令是break，可以直接输入b；也可以输入b后敲两次Tab键以查询所有b开头的命令（只有一个的话可以直接补全）：

```shell
(gdb) b
backtrace bookmark break break-range bt
```

这个智能提示也可以使用于在设置断点时候自动联想函数名称，例如，我们要对main函数设置断点，当输入b mai后敲Tab键，会自动补全为main

```shell
(gdb) b main
```

另外，也可以通过help <command>命令查看不熟悉的命令的帮助信息，类似于shell下的man，非常方便。

退出GDB

退出gdb的方式比较简单，直接在Shell下输入q（quit）命令就可以了：

```shell
(gdb) q
```

## 设置断点

启动GDB后，首先就是要设置断点，程序中断后才能调试。在gdb中，断点通常有三种形式：

### 断点（BreakPoint）

在代码的指定位置中断，这个是我们用得最多的一种。设置断点的命令是break，它通常有如下方式：

- break <function>    在进入指定函数时停住
- break <linenum>    在指定行号停住。
- break +/-offset    在当前行号的前面或后面的offset行停住。offiset为自然数。
- break filename:linenum    在源文件filename的linenum行处停住。
- break ... if <condition>    ...可以是上述的参数，condition表示条件，在条件成立时停住。比如在循环境体中，可以设置break if i=100，表示当i为100时停住程序。

可以通过info breakpoints [n]命令查看当前断点信息。此外，还有如下几个配套的常用命令：

- delete    删除所有断点
- delete breakpoint [n]    删除某个断点
- disable breakpoint [n]    禁用某个断点
- enable breakpoint [n]    使能某个断点

### 查看所有断点
info` b#info break`

### 观察点（WatchPoint）

在变量读、写或变化时中断，这类方式常用来定位bug。

- watch <expr>    变量发生变化时中断
- rwatch <expr>    变量被读时中断
- awatch <expr>     变量值被读或被写时中断

可以通过info watchpoints [n]命令查看当前观察点信息

### 捕捉点（CatchPoint）

捕捉点用来补捉程序运行时的一些事件。如：载入共享库（动态链接库）、C++的异常等。通常也是用来定位bug。

捕捉点的命令格式是：

- catch <event>

event可以是下面的内容

- throw     C++抛出的异常时中断
- catch     C++捕捉到的异常时中断
- exec    调用系统调用exec时（只在某些操作系统下有用）
- fork    调用系统调用fork时（只在某些操作系统下有用）
- vfork    调用系统调用vfork时（只在某些操作系统下有用）
- load 或 load <libname>     载入共享库时（只在某些操作系统下有用）
- unload 或 unload <libname>    卸载共享库时（只在某些操作系统下有用）

另外，还有一个`tcatch <event>`，功能类似，不过他只设置一次捕捉点，当程序停住以后，应点被自动删除。

捕捉点信息的查看方式和代码断点的命令是一样的，这里就不多介绍了。

### 在特定线程中中断

你可以定义你的断点是否在所有的线程上，或是在某个特定的线程。GDB很容易帮你完成这一工作。

- break <linespec> thread <threadno>
- break <linespec> thread <threadno> if ...

`linespec` 指定了断点设置在的源程序的行号。threadno指定了线程的ID，注意，这个ID是GDB分配的，你可以通过"info threads"命令来查看正在运行程序中的线程信息。如果你不指定`thread <threadno>`则表示你的断点设在所有线程上面。你还可以为某线程指定断点条件。如：

```shell
(gdb) break frik.c:13 thread 28 if bartab > lim
```

当你的程序被GDB停住时，所有的运行线程都会被停住。这方便你你查看运行程序的总体情况。而在你恢复程序运行时，所有的线程也会被恢复运行。那怕是主进程在被单步调试时。

恢复程序运行和单步调试

在gdb中，和调试步进相关的命令主要有如下几条：

- continue    继续运行程序直到下一个断点（类似于VS里的F5）
- next        逐过程步进，不会进入子函数（类似VS里的F10）
- setp        逐语句步进，会进入子函数（类似VS里的F11）
- until        运行至当前语句块结束
- finish    运行至函数结束并跳出，并打印函数的返回值（类似VS的Shift+F11）

PS：这些命令大部分可以简写为第一个字母，在日常使用过程中，往往只会输入第一个字符即可执行该命令，我标红的即是通常的使用方式。这几条命令使用非常频繁，并且可以带一些附加参数以实现高级功能，需要熟练掌握。

## GDB 查看数据

### 查看栈信息

当程序被停住了，首先要确认的就是程序是在哪儿被断住的。这个一般是通过查看调用栈信息来看的。在gdb中，查看调用栈的命令是backtrace，可以简写为bt。

```shell
(gdb) bt
#0 pop () at stack.c:10
#1 0x080484a6 in main () at main.c:12
```

也可以通过info stack命令实现类似的功能（我更喜欢这个命令）：

```shell
(gdb) info stack
#0 pop () at stack.c:10
#1 0x080484a6 in main () at main.c:12
```

### 查看源程序

当程序断住是，gdb会显示当前断点的位置：

```shell
Breakpoint 1, pop () at stack.c:10
10 return stack[top--];
```

可以用list命令来查看当前断点附近的程序的源代码：

```shell
(gdb) list
5 int top = -1;
6
7
8 char pop(void)
9 {
10 return stack[top--];
11 }
12
13 void push(char c)
14 {
```

`list`命令后面还可以更一些参数，来显示更多功能：

- <linenum>    行号。
- <+> [offset]    当前行号的正偏移量。
- <-> [offset]     当前行号的负偏移量。
- <filename:linenum> 文件的中的行行。
- <function>             函数的代码
- <filename:function>     文件中的函数。
- <*address> 程序运行时的语句在内存中的地址。

不过，就算有这些信息，查看代码仍然不大方便。现在新版的gdb都带一个tui的功能，可以通过`focus`命令开启、也可以启动 gdb 时，加 `-tui`选项。

### 查看运行时数据

gdb中查看变量的命令是print，一般用它的简写形式p。它的语法如下：

- print [</format>] <expr>

其中参数expr可以是一个变量，也可以是表达式。format表示输出格式，例如，可以用/x来将结果按16进制输出。如下是几个基本的例子：

```shell
(gdb) p top
$16 = 1
(gdb) p &top
$17 = (int *) 0x804a014 <top>
(gdb) p 3+2*5
$18 = 13
(gdb) p /x 3+2*5
$19 = 0xd
```

format的取值范围有如下几种：

- x 按十六进制格式显示变量。
- d 按十进制格式显示变量。
- u 按十六进制格式显示无符号整型。
- o 按八进制格式显示变量。
- t 按二进制格式显示变量。
- a 按十六进制格式显示变量。
- c 按字符格式显示变量。
- f 按浮点数格式显示变量。

### 查看函数返回值

查看函数返回值是在调试的过程中经常遇到的需求。例如，对于如下函数

```c
int foo()
{
	return 100;
}
```

我们可以以如下方式获取函数的返回值：
1. 通过finish命令运行至函数结束，此时会打印函数返回值。
```shell
(gdb) finish
Run till exit from #0 foo () at main.c:9
main () at main.c:15
15 }
Value returned is $2 = 100
```
2. 返回值会存储在eax寄存器中，通过查看信息可以获取返回值。
```shell
(gdb) p $eax
$3 = 100

(gdb) info registers
eax 0x64 100 
```

### 查看连续内存

可以使用GDB的"@"操作符查看连续内存，"@"的左边是第一个内存的地址的值，"@"的右边则你你想查看内存的长度。

例如，对于如下代码：`int arr[] = {2, 4, 6, 8, 10};`，可以通过如下命令查看arr前三个单元的数据。
```shell
(gdb) p *arr@3
$2 = {2, 4, 6} 
```

### 查看内存

可以使用examine命令（简写为x）来查看内存地址中的值。x命令的语法如下所示：

`x /<n/f/u> <addr>`

- n 表示显示内存的长度，也就是说从当前地址向后显示几个地址的内容。
- f 表示显示的格式，如果是字符串，则用s，如果是数字，则可以用i。
- u 表示从当前地址往后请求的字节数，默认是4个bytes。（b单字节，h双字节，w四字节，g八字节）
- <addr> 表示一个内存地址。

例如：以两字节为单位显示前面的那个数组的地址后32字节内存信息如下.
```shell
(gdb) x /16uh arr
0xbffff4cc: 2 0 4 0 6 0 8 0
0xbffff4dc: 10 0 34032 2052 0 0 0 0 
```

### 自动显示

在VisualStudio中，可以通过监视窗口动态查看变量的值。在gdb中，也提供了类似的命令display，它的语法是：

- display <expr>
- display /<fmt> <expr>
- display /<fmt> <addr>

expr是一个表达式，fmt表示显示的格式，addr表示内存地址。当你用display设定好了一个或多个表达式后，只要你的程序被停下来（单步跟踪时），GDB会自动显示你所设置的这些表达式的值。

几个相关的命令如下：

- undisplay <dnums...>        不显示dispaly
- delete display [dnums]    删除自动显示，不带dnums参数则删除所有自动显示，也支持范围删除，如： delete display 1,3-5
- disable display <dnums...>    使display失效
- enable display <dnums...>    恢复display
- info display        查看display信息

## 常用命令

### run命令
在默认情况下，gdb+filename只是attach到一个调试文件，并没有启动这个程序，我们需要输入run命令启动这个程序（run命令被简写成r）。如果程序已经启动，则再次输入 run 命令就会重启程序。

### continue命令
在程序触发断点或者使用 Ctrl+C 组合键中断后，如果我们想让程序继续运行，则只需输入 continue 命令即可（简写成 c）。当然，如果通过 continue 命令让程序在继续运行的过程中触发设置的程序断点，则程序会在断点处中断。

### break命令
break 命令即我们添加断点的命令，可以将其简写成 b。

在functionname函数的入口处添加断点
`break functionname`
##在当前文件行号为LineNo的地方添加一个断点
break LineNo
##在filename 文件行号为LineNo的地方添加一个断点
break filename:LineNo
tbreak命令
break 命令用于添加一个永久断点；tbreak 命令用于添加一个临时临时断点。

### backtrace与frame命令
backtrace 可简写成 bt，用于查看当前所在线程的调用堆栈。如果我们想切换到其他堆栈处，则可以使用frame命令。frame命令可被简写成f。

frame 堆栈编号
### info break、enable、disable、delete命令
在程序中加了很多断点以后，若想查看加了哪些断点，则可以使用info break命令

如果我们想禁用某个断点，则使用 disable 断点编号就可以禁用这个断点了，被禁用的断点不会再被触发；被禁用的断点可以使用enable断点编号并重新开启。

如果disable和enable命令不加断点编号，则分别表示禁用和启用所有断点

使用delete编号可以删除某个断点。如果输入delete时不加命令号，则表示删除所有断点。

### list命令
list命令用于查看当前断点附近的代码，可以简写成l。

list+命令（即list加号）可以从当前代码位置向下显示10行代码（向文件末尾方向）。list-命令（即list 减号）可以从当前代码位置向上显示10行代码（往文件开始方向）。

### print与ptype命令
print命令可以被简写成 p。通过 print命令可以在调试过程中方便地查看变量的值，也可以修改当前内存中的变量值。通过 print命令不仅可以输出变量的值，也可以输出特定表达式的计算结果，甚至可以输出一些函数的执行结果。

通过 print命令不仅可以输出表达式的结果，还可以修改变量的值

### info与thread命令
info命令是一个复合指令，可以用来查看当前进程所有线程的运行情况。

还可以用来查看当前函数的参数值，组合命令是info args。

### next命令
可被简写为n，next命令的调试术语叫“单步步过（step over）”，即遇到函数调用时不进入函数体内部，而是直接跳过。

### step命令
就是“单步步入（step into）”，顾名思义，就是遇到函数调用时进入函数内部。step可被简写为 s。

### return命令
用于结束执行当前函数，同时指定该函数的返回值

### finish命令
用于执行完整的函数体，然后正常返回到上层调用中

### until 命令
可被简写为u，我们使用这个命令让程序运行到指定的行停下来

### jump命令

jump <location>

location可以是程序的行号或者函数的地址，jump会让程序执行流跳转到指定的位置执行，其行为也是不可控的

在gdb命令行界面直接按下回车键，默认是将最近一条命令重新执行一遍

### set args与show args命令
很多程序都需要我们传递命令行参数。在gdb调试中使用gdb filename args这种形式给被调试的程序传递命令行参数是行不通的。正确的做法是在用gdb attach程序后，使用run 命令之前，使用 set args 命令行参数来指定被调试程序的命令行参数。可以通过show args查看命令行参数是否设置成功。

如果在单个命令行参数之间有空格，则可以使用引号将参数包裹起来。

如果想清除已经设置好的命令行参数，则使用set args不加任何参数即可。

### watch命令
        watch是一个强大的命令，可以用来监视一个变量或者一段内存，当这个变量或者该内存处的值发生变化时，gdb就会中断。监视某个变量或者某个内存地址会产生一个观察点（watch point）。

需要注意的是：当设置的观察点是一个局部变量时，局部变量失效后，观察点也会失效。

### display命令
display 命令用于监视变量或者内存的值，每次 gdb 中断，都会自动输出这些被监视变量或内存的值。

display命令的使用格式是display 变量名/内存地址/寄存器名。

我们可以使用info display查看当前已经监视了哪些值，使用delete display清除全部被监视的变量，使用delete display 编号移除对指定变量的监视。

### dir命令
可执行程序崩溃，我们用gdb调试core文件时，gdb可能会提示“No such file or directory”。

gcc/g++编译出来的可执行程序并不包含完整的源码，-g 只是加了一个可执行程序与源码之间的位置映射关系，我们可以通过dir命令重新定位这种关系。

加一个源文件路径到当前路径的前面，指定多个路径，可以使用“：”

`dir SourcePath:SourcePath2:SourcePath3`
SourcePath1、SourcePath2、SourcePath3
指的就是需要设置的源码目录，gdb 会依次到这些目录下搜索相应的源文件。

如果要查看当前设置了哪些源码搜索路径，则可以使用show dir命令。

dir命令不加参数时，表示清空当前已设置的源码搜索路径。
