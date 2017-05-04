# readelf命令

readelf命令用来显示一个或者多个elf格式的目标文件的信息，可以通过它的选项来控制显示哪些信息。
这里的elf-file(s)就表示那些被检查的文件。可以支持32位，64位的elf格式文件，也支持包含elf文件的文档（这里一般指的是使用ar命令将一些elf文件打包之后生成的例如lib*.a之类的“静态库”文件）。

这个程序和objdump提供的功能类似，但是它显示的信息更为具体，并且它不依赖BFD库(BFD库是一个GNU项目，它的目标就是希望通过一种统一的接口来处理不同的目标文件)，所以即使BFD库有什么bug存在的话也不会影响到readelf程序。

## ELF文件类型

- 可重定位文件：用户和其他目标文件一起创建可执行文件或者共享目标文件,例如lib*.a文件。
- 可执行文件：用于生成进程映像，载入内存执行,例如编译好的可执行文件a.out。
- 共享目标文件：用于和其他共享目标文件或者可重定位文件一起生成elf目标文件或者和执行文件一起创建进程映像，例如lib*.so文件。 


- .text section 里装载了可执行代码；
- .data section 里面装载了被初始化的数据；
- .bss section 里面装载了未被初始化的数据；
- 以 .rec 打头的 sections 里面装载了重定位条目；
- .symtab 或者 .dynsym section 里面装载了符号信息；
- .strtab 或者 .dynstr section 里面装载了字符串信息；

## ELF文件作用

ELF文件参与程序的连接(建立一个程序)和程序的执行(运行一个程序)，所以可以从不同的角度来看待elf格式的文件：

如果用于编译和链接（可重定位文件），则编译器和链接器将把elf文件看作是节头表描述的节的集合,程序头表可选。
如果用于加载执行（可执行文件），则加载器则将把elf文件看作是程序头表描述的段的集合，一个段可能包含多个节，节头表可选。
如果是共享文件，则两者都含有。

## ELF文件总体组成

elf文件头描述elf文件的总体信息。

系统相关：elf文件标识的魔术数，以及硬件和平台等相关信息，增加了elf文件的移植性,使交叉编译成为可能。
类型相关：就是前面说的那个类型。
加载相关：包括程序头表相关信息。 
链接相关：节头表相关信息。



参数说明

- -a --all 全部 Equivalent to: -h -l -S -s -r -d -V -A -I
- -h --file-header 文件头 Display the ELF file header
- -l --program-headers 程序 Display the program headers
- --segments An alias for --program-headers
- -S --section-headers 段头 Display the sections' header
- --sections An alias for --section-headers
- -e --headers 全部头 Equivalent to: -h -l -S
- -s --syms 符号表 Display the symbol table
- --symbols An alias for --syms
- -n --notes 内核注释 Display the core notes (if present)
- -r --relocs 重定位 Display the relocations (if present)
- -u --unwind Display the unwind info (if present)
- -d --dynamic 动态段 Display the dynamic segment (if present)
- -V --version-info 版本 Display the version sections (if present)
- -A --arch-specific CPU构架 Display architecture specific information (if any).
- -D --use-dynamic 动态段 Use the dynamic section info when displaying symbols
- -x --hex-dump= 显示 段内内容Dump the contents of section
- -w[liaprmfFso] or
- -I --histogram Display histogram of bucket list lengths
- -W --wide 宽行输出 Allow output width to exceed 80 characters
- -H --help Display this information
- -v --version Display the version number of readelf

## 示例

想知道一个应用程序的可运行的架构平台

```shell
readelf -h main| grep Machine
```

一个编译好的应用程序，想知道其编译时是否使用了-g选项（加入调试信息）


```shell
readelf -S main| grep debug
```

readelf输出的完整内容

```shell
readelf -all a.out
```

查看依赖库

```shell
readelf -d x | grep NEED
```