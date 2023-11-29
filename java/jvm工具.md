# jvm

## GC

** 新生代的Copying算法 **

目前大部分垃圾收集器对于新生代都采取Copying算法，因为新生代中每次垃圾回收都要回收大部分对象，也就是说需要复制的操作次数较少，但是实际中并不是按照1：1的比例来划分新生代的空间的，一般来说是将新生代划分为一块较大的Eden空间和两块较小的Survivor空间，每次使用Eden空间和其中的一块Survivor空间，当进行回收时，将Eden和Survivor中还存活的对象复制到另一块Survivor空间中，然后清理掉Eden和刚才使用过的Survivor空间。

** 老年代的Mark-Compact算法：**

而由于老年代的特点是每次回收都只回收少量对象，一般使用的是Mark-Compact或者是 Mark-Sweep算法。但“标记-清除”或“标记-整理”算法会比复制算法慢10倍以上。

注意，在堆区之外还有一个代就是永久代（Permanet Generation），它用来存储class类、常量、方法描述等。对永久代的回收主要回收两部分内容：废弃常量和无用的类。

JVM 把年轻代分为了三部分：1个Eden区和2个Survivor区（分别叫from和to），Eden、From、To的比例是8:1:1。
一般情况下，新创建的对象都会被分配到Eden区(一些大对象特殊处理),这些对象经过第一次Minor GC后，如果仍然存活，将会被移到Survivor区。对象在Survivor区中每熬过一次Minor GC，年龄就会增加1岁，当它的年龄增加到一定程度时，就会被移动到年老代中。
在GC开始的时候，对象只会存在于Eden区和名为“From”的Survivor区，Survivor区“To”是空的。紧接着进行GC，Eden区中所有存活的对象都会被复制到“To”，而在“From”区中，仍存活的对象会根据他们的年龄值来决定去向。年龄达到一定值(年龄阈值，可以通过-XX:MaxTenuringThreshold来设置)的对象会被移动到年老代中，没有达到阈值的对象会被复制到“To”区域。经过这次GC后，Eden区和From区已经被清空。这个时候，“From”和“To”会交换他们的角色，也就是新的“To”就是上次GC前的“From”，新的“From”就是上次GC前的“To”。不管怎样，都会保证名为To的Survivor区域是空的。Minor GC会一直重复这样的过程，直到“To”区被填满，“To”区被填满之后，会将所有对象移动到年老代中。
因为年轻代中的对象基本都是朝生夕死的(80%以上)，所以在年轻代的垃圾回收算法使用的是复制算法，复制算法的基本思想就是将内存分为两块，每次只用其中一块，当这一块内存用完，就将还活着的对象复制到另外一块上面。复制算法不会产生内存碎片。

** 有关年轻代的JVM参数 **

1) -XX:NewSize和-XX:MaxNewSize
用于设置年轻代的大小，建议设为整个堆大小的1/3或者1/4,两个值设为一样大。

2) -XX:SurvivorRatio
用于设置Eden和其中一个Survivor的比值，这个值也比较重要。

3) -XX:+PrintTenuringDistribution
这个参数用于显示每次 Minor GC 时 Survivor 区中各个年龄段的对象的大小。

4) -XX:InitialTenuringThreshol和-XX:MaxTenuringThreshold
用于设置晋升到老年代的对象年龄的最小值和最大值，每个对象在坚持过一次Minor GC之后，年龄就加1。

## JPS：虚拟机进程状况工具

jvm process status tool

```shell
jps
jps -l
```
输出两列信息：LVMID 和 name。

- -q：仅输出VM标识符，不包括 classname, jar name, arguments in main method 
- -m：输出main method的参数 
- -l：输出完全的包名，应用主类名，jar的完全路径名 
- -v：输出jvm参数 
- -V：输出通过flag文件传递到JVM中的参数(.hotspotrc文件或-XX:Flags=所指定的文件 
- -Joption：传递参数到vm,例如:-J-Xms512m

## jstat：虚拟机统计信息监视工具

运行期定位虚拟机性能问题首选工具
```shell
jstat -gc pid
#后面加毫秒后缀、打印次数，来实现实时监控，如下图中的 1000代表1000毫秒，即一秒，100代表打印100次。
jstat -gc pid 1000 100
```
可以显示gc的信息，gc次数，及时间。
其中，年轻代（S0C、S1C、EC）、老年代（OC）在不同的时间点看到的容量可能会不一样。

各属性代表的含义：

末尾为C，代表该区Capacity（容量）

末尾为U，代表Used（已使用的容量）

前缀代表区域。
```txt
区域	含义
S0C	Survive0 第一个幸存区的容量大小
S1C	Survive1 第二个幸存区的容量大小
S0U	Survive0 第一个幸存区已使用的内存大小
S1U	Survive1 第二个幸存区已使用的内存大小
EC	Eden 伊甸园区的容量大小
EU	Eden 伊甸园区已使用的内存大小
OC	Old 老年代的容量大小
OU	Old 老年代已使用的内存大小
MC	Meta 方法区的容量大小
MU	Meta 方法区已使用的内存大小
YGC	Young GC发生的次数
YGCT	Young GC的总耗时
FGC	Full GC发生的次数
FGCT	Full GC的总耗时
```
其原因是：容量大小会因为JVM参数（Xmx、Xms）配置的大小不一致，而导致各区容量会在内存紧张时变为最大，内存空闲时变为最小。这是因为JVM的参数配置，而导致的内存容量的收缩与扩张。
在某种程度上来讲，可以将【Xmx、Xms】、【XX:MaxNewSize、XX:NewSize】配置为一样大小，减少在内存紧张时，对象在各内存区流转而引起的内存扩张、收缩，进而充分利用各区内存资源。

查询当前进程下，各区已使用的内存占比情况
```shell
jstat -gcutil
```
`jstat -gcnew` 与 `jstat -gcold` 是用来排查年轻代和年老代的内存占用情况

stack命令用于打印指定Java进程、核心文件或远程调试服务器的Java线程的Java堆栈跟踪信息。
