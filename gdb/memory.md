# Linux 查看进程的内存

## 内存映射

`proc/pid/maps` 文件显示进程映射的内存区域和权限。内核中进程的一段地址空间信息 `vm_area_struct` 结构体表示：
```c
struct vm_area_struct {
    struct mm_struct * vm_mm;    /* The address space we belong to. */
    unsigned long vm_start;        /* Our start address within vm_mm. */
    unsigned long vm_end;        /* The first byte after our end address within vm_mm. */

    /* linked list of VM areas per task, sorted by address */
    struct vm_area_struct *vm_next;

    pgprot_t vm_page_prot;        /* Access permissions of this VMA. */
    unsigned long vm_flags;        /* Flags, listed below. */

    struct rb_node vm_rb;

    /*
     * For areas with an address space and backing store,
     * linkage into the address_space->i_mmap prio tree, or
     * linkage to the list of like vmas hanging off its node, or
     * linkage of vma in the address_space->i_mmap_nonlinear list.
     */
    union {
        struct {
            struct list_head list;
            void *parent;    /* aligns with prio_tree_node parent */
            struct vm_area_struct *head;
        } vm_set;

        struct raw_prio_tree_node prio_tree_node;
    } shared;

    /*
     * A file's MAP_PRIVATE vma can be in both i_mmap tree and anon_vma
     * list, after a COW of one of the file pages.  A MAP_SHARED vma
     * can only be in the i_mmap tree.  An anonymous MAP_PRIVATE, stack
     * or brk vma (with NULL file) can only be in an anon_vma list.
     */
    struct list_head anon_vma_node;    /* Serialized by anon_vma->lock */
    struct anon_vma *anon_vma;    /* Serialized by page_table_lock */

    /* Function pointers to deal with this struct. */
    struct vm_operations_struct * vm_ops;

    /* Information about our backing store: */
    unsigned long vm_pgoff;        /* Offset (within vm_file) in PAGE_SIZE units, *not* PAGE_CACHE_SIZE */
    struct file * vm_file;        /* File we map to (can be NULL). */
    void * vm_private_data;        /* was vm_pte (shared mem) */
    unsigned long vm_truncate_count;/* truncate_count or restart_addr */

#ifndef CONFIG_MMU
    atomic_t vm_usage;        /* refcount (VMAs shared if !MMU) */
#endif
#ifdef CONFIG_NUMA
    struct mempolicy *vm_policy;    /* NUMA policy for the VMA */
#endif
};
```

```shell
cat /proc/pid/maps
```

虚拟地址区间
1000   = 4KB
100000 = 1MB

下面是 PHP FPM进程的 maps 内容。
```maps
5577acaa6000-5577ace79000 r-xp 00000000 08:03 3024822                    /usr/sbin/php-fpm7.0-zts
5577ad078000-5577ad0f1000 r--p 003d2000 08:03 3024822                    /usr/sbin/php-fpm7.0-zts
5577ad0f1000-5577ad0fe000 rw-p 0044b000 08:03 3024822                    /usr/sbin/php-fpm7.0-zts
5577ad0fe000-5577ad114000 rw-p 00000000 00:00 0 
5577ae59b000-5577ae75f000 rw-p 00000000 00:00 0                          [heap]
7f0d54878000-7f0d54883000 r-xp 00000000 08:03 5249044                    /lib/x86_64-linux-gnu/libnss_files-2.23.so
7f0d54883000-7f0d54a82000 ---p 0000b000 08:03 5249044                    /lib/x86_64-linux-gnu/libnss_files-2.23.so
7f0d54a82000-7f0d54a83000 r--p 0000a000 08:03 5249044                    /lib/x86_64-linux-gnu/libnss_files-2.23.so
7f0d54a83000-7f0d54a84000 rw-p 0000b000 08:03 5249044                    /lib/x86_64-linux-gnu/libnss_files-2.23.so
...
7f0d61cf9000-7f0d61cfa000 r--p 00025000 08:03 5247495                    /lib/x86_64-linux-gnu/ld-2.23.so
7f0d61cfa000-7f0d61cfb000 rw-p 00026000 08:03 5247495                    /lib/x86_64-linux-gnu/ld-2.23.so
7f0d61cfb000-7f0d61cfc000 rw-p 00000000 00:00 0 
7fff72f0b000-7fff72f2c000 rw-p 00000000 00:00 0                          [stack]
7fff72ff6000-7fff72ff8000 r--p 00000000 00:00 0                          [vvar]
7fff72ff8000-7fff72ffa000 r-xp 00000000 00:00 0                          [vdso]
ffffffffff600000-ffffffffff601000 r-xp 00000000 00:00 0                  [vsyscall]
```
第一列：虚拟地址空间范围
第二列：权限 r-读，w-写 x-可执行 p-私有，对应vm_flags。
第三列：指本段映射地址在文件中的偏移
第四列：所映射的文件所属设备的设备号，没有设备号为00:00，冒号前面的为主设备号fd，后面的为次设备号
第五列：文件的索引节点号，匿名映射为0。
第六列：所映射的文件名。对匿名映射来说，是此段内存在进程中的作用。`[stack]`表示本段内存作为栈来使用，`[heap]`作为堆来使用，其他情况则为无。

第一行：有执行权限，是应用程序代码段（Text段），存放着二进制可执行的机器指令，由kernel把该库ELF文件的代码段map到虚存空间；
第二行：只读权限，是应用程序只读数据段（Data段）
第三行：应用程序读写私有数据段（Data段），存放着程序执行所需的全局变量，由kernel把ELF文件的数据段map到虚存空间；

BSS 段的范围[5577ad0f1000-5577ad0fe000]

- BSS段：
　　BSS段（Block Started by Symbol）通常是指用来存放程序中未初始化的全局变量的一块内存区域，属于静态内存分配。 
  
- Data段：
　　数据段（data segment）通常是指用来存放程序中已初始化的全局变量的一块内存区域。
　　数据段属于静态内存分配。 
  
- Text段：

　　代码段（code segment/text segment）通常是指用来存放程序执行代码的一块内存区域。
　　这部分区域的大小在程序运行前就已经确定，并且内存区域通常属于只读(某些架构也允许代码段为可写，即允许修改程序)。
　　在代码段中，也有可能包含一些只读的常数变量，例如字符串常量等。 
  
- 堆（heap）：
　　堆是用于存放进程运行中被动态分配的内存段，它的大小并不固定，可动态扩张或缩减。
  
- 栈(stack)：
　　栈又称堆栈，是用户存放程序临时创建的局部变量，

* 将第一列空间范围换成 size

```shell
sudo apt-get install gawk
# 32 位
cat /proc/pid/maps | sed -e "s/\([0-9a-f]\{8\}\)-\([0-9a-f]\{8\}\)/0x\1 0x\2/" | awk '{printf("\033[0;33m[%8d Page]\033[0m \033[0;35m[%8d KB]\033[0m %s\n", (strtonum($2) - strtonum($1))/4096, (strtonum($2) - strtonum($1))/1024, $0)}'
# 64 位使用
cat /proc/pid/maps | sed -e "s/\([0-9a-f]\{12\}\)-\([0-9a-f]\{12\}\)/0x\1 0x\2/" | awk '{printf("\033[0;33m[%8d Page]\033[0m \033[0;35m[%8d KB]\033[0m %s\n", (strtonum($2) - strtonum($1))/4096, (strtonum($2) - strtonum($1))/1024, $0)}'
```

## 导出进程内存内容

将进程 `attach` 到 `gdb`上，然后导出指定地址范围的内存：
```shell
gdb attach pid
(gdb) dump memory /home/xxx/heap.dump 0x5577ae59b000 0x5577ae75f000
(gdb) dump memory /home/xxx/memory.dump 0x5577ad0fe000 0x5577ad114000
```

```shell
strings -n 10 /home/xxx/heap.dump
```

## other

- strace
- ptrace
