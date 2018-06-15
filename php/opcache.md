
## HugePage

HugePages是Linux内核的一个特性，使用hugepage可以用更大的内存页来取代传统的4K页面。使用HugePage主要带来以下好处：

1. HugePages 会在系统启动时，直接分配并保留对应大小的内存区域。
2. HugePages 在开机之后，如果没有管理员的介入，是不会释放和改变的。
3. 没有swap。
4. 大大提高了CPU cache中存放的page table所覆盖的内存大小，从而提高了TLB命中率。

进程的虚拟内存地址段先连接到page table然后再连接到物理内存。所以在访问内存时需要先访问page tables得到虚拟内存和物理内存的映射关系，然后再访问物理内存。
CPU cache中有一部分TLB用来存放部分page table以提高这种转换的速度。因为page size变大了，所以同样大小的TLB，所覆盖的内存大小也变大了。提高了TLB命中率，也提高了地址转换的速度。

5. 减轻page table的负载。

* 注意的地方

1. Hugepages是在分配后就会预留出来的，其大小一定要比服务器上所有实例的SGA总和要大，差一点都不行。

比如说Hugepages设置为8G，oracle SGA为9G，那么oracle在启动的时候就不会使用到这8G的Hugepages。这8G就浪费了。所以在设置Hugepages时要计算SGA的大小，后面会给出一个脚本来计算。

2. 其他进程无法使用Hugepages的内存，所以不要设置太大，稍稍比SGA大一点保证SGA可以使用到hugepages就好了。

3. 在meminfo中和Hugepage相关的有四项：

- HugePages_Total
为所分配的页面数目，和Hugepagesize相乘后得到所分配的内存大小。4611*2/1024大约为9GB
- HugePages_Free
为从来没有被使用过的Hugepages数目。即使oraclesga已经分配了这部分内存，但是如果没有实际写入，那么看到的还是Free的。这是很容易误解的地方。
- HugePages_Rsvd
为已经被分配预留但是还没有使用的page数目。在Oracle刚刚启动时，大部分内存应该都是Reserved并且Free的，随着oracle SGA的使用，Reserved和Free都会不断的降低。
- HugePages_Free-HugePages_Rsvd
这部分是没有被使用到的内存，如果没有其他的oracle instance，这部分内存也许永远都不会被使用到，也就是被浪费了。

4. HugePages和oracle AMM（自动内存管理）是互斥的，所以使用HugePages必须设置内存参数MEMORY_TARGET / MEMORY_MAX_TARGET 为0。

```conf
opcache.huge_code_pages=1
```

```shell
sudo sysctl vm.nr_hugepages=512
cat /proc/meminfo | grep Huge
```

* 修改内核参数 memlock

`/etc/security/limits.conf`增加以下两行内容：
```conf
*  soft  memlock    12582912
*  hard  memlock    12582912
```

* 修改系统参数 kernel.shmall

Kernel.shmall 是系统一次可以使用的最大共享内存大小。单位是page（4KB）。

修改 `/etc/sysctl.conf`：
```conf
kernel.shmall = 2359296

fs.file-max=65536
net.core.somaxconn = 65535
net.core.netdev_max_backlog = 65535
net.ipv4.tcp_syncookies = 1
net.ipv4.tcp_tw_reuse = 1
net.ipv4.tcp_tw_recycle = 1
net.ipv4.tcp_fin_timeout=30
vm.nr_hugepages = 512
```