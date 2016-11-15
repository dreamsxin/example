很久前就接触 autoconf、automake 等，然而一直没有静下心来看看怎么使用他们，特别是 autoconf。 希望通过这个阅读autoconf文档的笔记能把它们用起来。

1. 介绍

Autoconf is a tool for producing shell scripts that automatically configure software source code packageto adapt to many kinds of POSIX-like systems. The primary goal of Autoconf is making the user’s life easier; making the maintainer’s life easier is only a secondary goal. 换句话说就是 autoconf 的目的不是更容易生成 configure 脚本，而是使生成的 configure 脚本更易用、更可移植。

2. GNU 编译系统

2.1 Automake

make 的主要限制包括缺少自动依赖跟踪、子目录递归编译、可靠时间戳等。开发人员不得不为每一个项目重复繁琐的 工作 （例如为每个项目，每个目录实现 make install, make distclean, make uninstall 等target）。

Automake 的目的是简化这些反复琐碎的工作，它使用 Makefile.am 表达编译需求（语法比 Makefile 更灵活）， 自动根据生成 Makefile.in 并和 Autoconf 联合使用，并使得项目编译更简单。

例如一个简单的 Hello World 的只有2行：

bin_PROGRAMS = hello
hello_SOURCES = hello.c
2.2 Libtool

Libtool 帮助编译可移植的共享库，而不用关系不同平台上不兼容的工具、编译选项等。

3. 生成 configure 脚本

configure 脚本会生成以下文件：

一个或者多个 Makefile 文件，通常一个子目录一个。
一个 C 头文件，包含若干 #define 指令，名字可配置
一个名为 config.status 的脚本。运行时会重新创建上面提到的文件
一个名为 config.cache 的脚本（–config-cache) 保存运行的测试的结果。
一个名为 config.log 的文件，包含编译器生成的日志，用于调试错误。
Autoconf 需要输入文件 configure.ac (或者过时的 configure.in) 来创建 configure 脚本。 如果自己实现 某些 feature tests，则需要编辑类似 aclocal.m4 的文件。如果使用 C 头文件，则需要运行 autoheader。

3.1 编写 configure.ac 文件

这个文件包含 Autoconf 宏，这些宏用于测试项目需要的系统特性。

Autoconf 已经提供了大量的宏，用于测试非常多的系统特性
也可以使用 Autoconf 模板宏生成自定义的检查
也可以在 configure.ac 中使用 shell 脚本执行检查

your source files –> [autoscan*] –> [configure.scan] –> configure.ac

configure.ac –+ | +—-> autoconf* –> configure [aclocal.m4] –+—–+ | +—->[autoheader*] –> [config.h.in] [acsite.m4] –+

Makefile.in

如果同时使用 Autoconf 和 Automake，则还有以下处理：

[acinclude.m4] --+
|
[local macros] --+--> aclocal* --> aclocal.m4
|
configure.ac --+

configure.ac --+
|--> automake* --> Makefile.in
Makefile.am --+
配置时：

+---------------> [config.cache]
configure* --+---------+---------------> config.log
| |
[config.h.in] --+ V +-> [config.h] --+
|--> config.status* --+ +--> make*
Makefile.in --+ +-> Makefile --+
3.1.1 Shell 脚本编译器

autoconf 自身需要运行在各种系统上，因而只能用这些系统的一个公共子集。autoconf 需要一个类似编译器的东西，把 configure.ac 翻译成可移植的shell 脚本：configure。

autconf 通过扩展 sh 来实现这个编译器。因而通过宏扩展实现autoconf的扩展是一个自然而然的选择：autoconf编译器不断的对 输入 text 执行宏扩展，使用宏体替换宏调用，最终生成一个纯 sh 脚本。

autoconf 没有自定义自己的宏语言，而是使用了一个通用的宏语言：M4。

3.1.2 Autoconf 语言

Autoconf 语言和大多数计算机语言不同，读起来像自然语言。

首先需要区分literal strings和宏: 使用转义字符(quotation).

调用宏的时候需要使用括号，而且宏名字和括号之间不能有空格：AC_INIT([hello], [1.0])

参数使用转义字符’[’, ‘]’括起来。如果参数包含宏名字、逗号、括号、空格或者换行，则必须使用转义字符。

AC_CHECK_HEADER([stdio.h],
[AC_DEFINE([HAVE_STDIO_H], [1],
[Define to 1 if you have .])],
[AC_MSG_ERROR([sorry, can’t do anything for you])])
可以写成： AC_CHECK_HEADER(stdio.h, AC_DEFINE(HAVE_STDIO_H, 1, Define to 1 if you have .), AC_MSG_ERROR([sorry, can’t do anything for you]))

3.1.3 标准的 configure.ac 布局

Autoconf requirements
AC_INIT(package, version, bug-report-address)
information on the package
checks for programs
checks for libraries
checks for header files
checks for types
checks for structures
checks for compiler characteristics
checks for library functions
checks for system services
AC_CONFIG_FILES([file...])
AC_OUTPUT
3.2 使用 autoscan 创建 configure.ac

autoscan 可以帮助生成 configure.ac，它会扫描目录下得文件，并根据常见的可移植性问题，创建 configure.scan。 根据这个文件可以生成 configure.ac. 有时候需要做某些修改。 autoscan.log 包含了问什么需要某个宏的详细信息。

3.3 使用 ifnames 列出条件

ifnames 扫描参数C文件，并打印出 C 预处理器会处理的条件.

3.4 使用 autoconf 创建 configure

autoconf 扫描 configure.ac 文件，使用 M4 宏处理器处理其中的autoconf宏，并生成 configure 文件。

Autoconf 宏定义通常在单独的文件中。Autoconf 自身包含了常用的宏定义，autoconf 会首先读入自定义的宏。然后查找一个 ‘acsite.m4’ 文件和 当前目录下得 aclocal.m4 文件。

4 初始化和输出文件

4.1 初始化 configure

configure 需要首先调用 AC_INIT 进行初始化。

AC_INIT(package, version, [bug-report], [tarname], [url])
处理命令行参数，执行初始化和验证。

设置package的名字和版本。
AC_INIT 定义下面的 M4 宏：

AC_PACKAGE_NAME,PACKAGE_NAME
AC_PACKAGE_TARNAME, PACKAGE_TARNAME
AC_PACKAGE_VERSION, PACKAGE_VERSION
AC_PACKAGE_STRING, PACKAGE_STRING
AC_PACKAGE_BUGREPORT, PACKAGE_BUGREPORT
AC_PACKAGE_URL, PACKAGE_URL
4.2 设定 Autoconf 版本

AC_PREREQ(version)：需求的 autoconf 的版本

AC_AUTOCONF_VERSION
4.3 输出文件

AC_OUTPUT： 生成 config.status 并执行它。

config.status 执行所有的配置操作：所有的输出文件(AC_CONFIG_FILES), 头文件（AC_CONFIG_HEADERS), 命令（AC_CONFIG_COMMANDS)，
链接（AC_CONFIG_LINKS)，子目录（AC_CONFIG_SUBDIRS)

AC_PROG_MAKE_SET
4.4 执行配置操作

看起来 configure 做了所有的事情，实际上它主要使用了 config.status。 configure 主要负责检测系统特性，而 config.status 负责根据 configure 检测的结果生成各种文件 （instantiate files）。

有四种主要的标准 instantiating 宏：

AC_CONFIG_FILES
AC_CONFIG_HEADERS
AC_CONFIG_COMMANDS
AC_CONFIG_LINKS
原型为：

AC_CONFIG_ITEMS(tag..., [commands], [init-cmds])

tag: 空格或者换行分割的 tags，通常是要创建的文件的名字。

AC_CONFIG_FILES，AC_CONFIG_HEADERS 可以使用类似 output:inputs:inputs 这样的tag，表示根据输入模板生成输出。
缺省的输入模板是 output.in

commands: 直接写入到 config.status 的shell命令。
4.5 创建配置文件

AC_CONFIG_FILES(file..., [cmds], [init-cmds])

告诉 AC_OUTPUT 通过拷贝输入文件（默认是file.in) ，替换输出变量而创建每个 file。
通常用来创建 Makefile。也能创建其他文件，例如 .gdbinit

AC_CONFIG_FILES([Makefile src/Makefile man/Makefile X/Imakefile])
AC_CONFIG_FILES([autoconf], [chmod +x autoconf])
4.6 Makefile 中的变量替换

源代码路径下，每个需要执行编译操作的子目录都要有一个 Makefile.in 的文件， configure 根据这个文件创建 Makefile.

configure 从 Makefile.in 创建 Makefile 的主要操作时变量替换：即替换 Makefile.in 中类似 @variable@ 的变量。 这种变量称为 output variables。 通常它是 configure 中设置的 shell 脚本。

需要使用 AC_SUBST 告诉 configure 某个shell变量可以进行替换。

4.6.1 预定义的输出变量

某些输出变量是由 Autoconf 宏预先定义好的。 这些预定义的变量可以在 config.status 中直接使用，也可以用于 configure 测试。

CFLAGS： C 编译器参数。 如果环境变量没有设置，调用 AC_PROG_CC 时使用缺省值。

configure_input:

CPPFLAGS: 预处理器选项
CXXFLAGS: C++ 编译器选项

DEFS：传递给 C 编译器的 -D 选项。如果调用 AC_CONFIG_HEADERS ，则 configure 使用 -DHAVE_CONFIG_H 替换 @DEFS@

ECHO_C, ECHO_N, ECHO_T:

LDFLAGS: linker 的选项,紧使用 -s 或者 -L 这样影响linker行为的选项。 不要使用这个变量传递库名字（-l），使用 LIBS。
LIBS：linker的 -l 选项。

builddir: 通常为 .
abs_builddir: builddir的绝对路径
top_builddir： 编译链中的 toplevel 路径

srcdir: 包含Makefile处理的源代码的目录. Makefile 可能和 srcdir 不是同一个目录。特别是分开编译的时候。
top_srcdir: 顶级源代码目录.
4.6.2 安装目录相关的变量

这些变量仅仅在 makefile 中使用。

bindir
datadir
datarootdir
docdir
dvidir
exec_prefix
htmldir
includedir
infodir
libdir
libexecdir
localedir
prefix
sbindir
sharestatedir
sysconfdir
4.6.3 编译目录

autoconf支持同时编译多个平台，并使用不同的目录保存不同平台的 object 文件。 也就是说源代码和 object 文件可以在不同的目录中。 make 通过使用 VPATH 实现。

4.6.4 自动 remaking

4.7 配置头文件

如果需要多个 c 预处理器宏，使用编译器 -D 选项会变得很长。 造成2个问题：1）make的输出难以辨认错误；2）更严重的是可能超过OS的行长度限制 。另一个办法是创建一个包含 #define 指令的 C 头文件。 AC_CONFIG_HEADERS 用于此目的.

源代码需要使用 #include 包含配置头文件，并且在其他头文件之前，以防止声明的不一致。

如果使用 VPATH，则需要设定 -I. 编译选项，因为默认c预处理器仅仅搜索源代码目录而不会搜索 build 目录。（VPATH的时候 config.h 生成在 build 目录下）。

AC_CONFIG_HEADERS(header..., [cmds], [init-cmds])

AH_HEADER
4.7.1 配置头文件模板

需要模板文件生成 config.h. 模板文件通常包含注释、#undef语句。例如 configure.ac 包含下面调用：

AC_CONFIG_HEADERS([conf.h])
AC_CHECK_HEADERS([unistd.h])
则 conf.h.in 需要含有下面代码：

/* Define as 1 if you have unistd.h */
#undef HAVE_UNISTD_H
AC_DEFINE 修改 #undef 为适当的 #define。

4.7.2 使用 autoheader 创建 config.h.in

使用 autoheader 可以简化头文件模块的维护。 autoheader 搜索 configure 中的第一次调用 AC_CONFIG_HEADERS，并使用其第一个文件 作为生成的头文件名。

4.8 运行任意配置命令

可以在运行 config.status 之前、之中和之后运行任意命令。

AC_CONFIG_COMMANDS(tag..., [cmds], [init-cmds])

在 config.status 结束后运行的额外命令。通常 cmds 会创建文件，所以 tag 通常是文件的名字。
4.9 创建配置连接

AC_CONFIG_LINKS(dest:source..., [cmds], [init-cmds])

创建 AC_OUTPUT 符号链接. dest 和 source 名字相对于 top level 源代码路径或者编译路径.
例子：

AC_CONFIG_LINKS([host.h:config/$machine.h
object.h:config/$obj_format.h])

在当前目录下创建 host.h -> srcdir/config/$machine.h
4.10 子目录

通常 AC_OUTPUT 可以处理子目录 makefile 的生成，如果 configure 脚本需要控制多个独立的package，则可以使用 CONFIG_SUBDIRS.

4.11 缺省的 prefix

缺省的prefix 是 /usr/local. 可以使用 AC_PREFIX_DEFAULT 宏设置缺省的 prefix

AC_PREFIX_DEFAULT(/usr/local/pgsql)
5 Autoconf 自带的测试宏

5.1 通用行为

符号转换：

首先参数转换成全部大写
‘*’ 转换成字母 ‘P’
所有其他非字母数字字符转换成下划线，包括空格.

AC_CHECK_TYPES([struct $Expensive*])

如果检查成功，则定义符号：HAVE_STRUCT__EXPENSIVEP

缺省include

AC_INCLUDES_DEFAULT([include-directives])
5.2 可用工具程序

这些宏检查某个工具是否存在或者是否支持特定的行为。

AC_PROG_AWK: 依次检查 gawk, mawk, nawk, awk, 并设置 AWK 为第一个找到的程序。

AC_PROG_GREP: 设置 GREP 为 grep 或者 ggrep 的最佳实现。

AC_PROG_EGREP: 检查 $GREP -E 或者选择 egrep 或者 gegrep。设置输出变量 EGREP

AC_PROG_FGREP: 检查 $GREP -F, 或者 fgrep， gfgrep

AC_PROG_INSTALL: 设置输出变量 INSTALL 为 BSD 兼容的 install 程序。如果PATH里面找不到，则设置
INSTALL 为 dir/install-sh -c'. 也设置 INSTALL_PROGRAM 和 INSTALL_SCRIPT 为 ${INSTALL},
INSTALL_DATA 为 ${INSTALL} -m 644

AC_PROG_MKDIR_P: 设置输出变量 MKDIR_P， 如果可能使用 mkdir -p，如果不支持，则使用 install-sh -d。

AC_PROG_LEX：设置输出变量 LEX 为flex/lex, LEXLIB 为 -lfl/-ll。如果都不存在，则设置为 ':'.

AC_PROG_LN_S: ln -s

AC_PROG_RANLIB
AC_PROG_SED
AC_PROG_YACC
5.3 通用程序和文件检查

AC_CHECK_PROG(variable, prog-to-check-for, value-if-found, [value-if-not-found], [path=$PATH], [reject])

AC_CHECK_PROGS(variable, progs-to-check-for, [value-if-not-found], [path='$PATH'])
检查progs-to-check-for在 PATH 中是否存在，如果存在，则设置变量为程序的名字，否则检查list中得下一个。
如果一个也没找到，则设置输出变量为 value-if-not-found. 如果value-if-not-found 没有设置，则变量的值
不变。 需要对变量调用 AC_SUBST。

AC_CHECK_TOOL(variable, prog-to-check-for, [value-if-not-found], [path=$PATH])
和 AC_CHECK_PROG 类似，但是会先检查是否存在带有 --host 前缀的程序。
configure --build=x86_64-gnu --host=i386-gnu

则 AC_CHECK_TOOL([RANLIB], [ranlib], [:]): 则先查找 i386-gnu-ranlib 是否存在，然后是ranlib，
最后是 ：。

AC_CHECK_TOOLS(variable, progs-to-check-for, [value-if-not-found], [path=$PATH])

AC_PATH_PROG(variable, prog-to-check-for, [value-if-not-found], [path=$PATH])
和 AC_CHECK_PROG 类似，但是设置输出变量为程序的绝对路径名。

AC_PATH_PROGS(variable, progs-to-check-for, [value-if-not-found], [path=$PATH])
5.4 文件：有时候需要检查文件是否存在。交叉编译时不可用。考虑运行时检查是不是更合理。

AC_CHECK_FILE(file, [action-if-found], [action-if-not-found])
在本地系统上检查文件是否存在。

AC_CHECK_FILES
库文件： 检查 C、C++、Fortran或者 Go库是否存在

AC_CHECK_LIB(library, function, [action-if-found], [action-if-not-found], [other-libraries])
检测library是否存在，检查方法为使用该library链接调用给定函数的一个测试程序。 通常 function 是库提供的
函数之一。

action-if-found: 是一系列shell命令。缺省动作为添加 -llibrary 到 LIBS 中，并定义 HAVE_LIBlibrary.

建议使用 AC_SEARCH_LIBS。

AC_SEARCH_LIBS(function, search-libs, [action-if-found], [action-if-not-found], [other-libs])
依次测试链接 search-libs，判断是否能调用给定函数，如果成功，则将 -llibrary 加到 LIBS 头部。
库函数：检查特定的 c 库函数

1. c函数可移植性问题

exit
free： C 标准说 free(NULL) 不做任何操作，而老的系统不支持。
isinf/isnan: C99 说这些是宏. C99 的isinf可以支持 long double，然后 pre-C99 只能使用 double 参数。
最好的办法是使用 gnulib 模块中得isinf和isnan，一个简单的方法：

#include #ifndef isnan
#define isnan(x) \
(sizeof(x) == sizeof(long double) ? isnan_ld(x) \
: sizeof(x) == sizeof(double) ? isnan_d(x) \
: isnan_f(x))

static inline int isnan_f(float x) { return x != x; }
static inline int isnan_d(double x) { return x != x; }
static inline int isnan_ld(long double x) { return x != x };
#endif

malloc: C 标准诶有定义 malloc(0) 的行为

putenv
realloc： realloc(NULL, size)
signal handler:

snprintf:
sprintf
sscanf
strerror_r
strnlen
sysconf
unlink
unsetenv

va_copy
va_list

Signed >>:
Integer /

2. 特殊函数检查

AC_FUNC_ALLOCA： alloca
AC_FUNC_CHOWN
AC_FUNC_CLOSEDIR_VOID

AC_FUNC_ERROR_AT_LINE
AC_FUNC_FNMATCH
AC_FUNC_FNMATCH_GNU

AC_FUNC_FORK: 检查 fork 和vfork
AC_FUNC_FSEEKO
AC_FUNC_GETGROUPS
AC_FUNC_GETLOADAVG
AC_FUNC_GETMNTENT
AC_FUNC_GETPGRP
AC_FUNC_MALLOC
AC_FUNC_MBRTOWC
AC_FUNC_MEMCMP:
AC_FUNC_MKTIME
AC_FUNC_MMAP
...
AC_FUNC_STAT
AC_FUNC_LSTAT
AC_FUNC_STRFTIME
AC_FUNC_STRTOD
AC_FUNC_STRTOLD
...

3. 普通函数检查：通常先使用 AC_CHECK_LIB 检查库是否存在

AC_CHECK_FUNC(function, [action-if-found], [action-if-not-found])

AC_CHECK_FUNS(function..., [action-if-found], [action-if-not-found])
依次检查每个函数，如果存在，则定义 HAVE_function。 如果后面参数存在，则额外执行。

AC_CHECK_FUNCS_ONE(function...)

AC_LIBOBJ(function)
要求 function.c 必须包含在可执行文件中，以替换 function 的问题实现。

AC_LIBSOURCE(file)：编译本项目，必须该文件。
AC_LIBSOURCES(files)
AC_CONFIG_LIBOBJ_DIR(directory)

AC_REPLACE_FUNCS(function...)
和 AC_CHECK_FUNCS 类似，但是使用 AC_LIBOBJ(function) 作为 action-if-not-found.
5.5 头文件

下面的宏检查某些头文件是否存在。

headers 的可移植性问题

limits.h: C99 要求 limits.h 定义 LLONG_MIN， LLONG_MAX，ULLONG_MAX，但是非 c99 没有定义。

inttypes.h vs. stdint.h
linux/irda.h
linux/random.h
net/if.h
netinet/if_ehter.h
stdint.h
stdlib.h
sys/mount.h
sys/ptem.h
sys/socket.h

AC_HEADER_STDC: 检查 stdlib.h, stdarg.h, string.h, float.h
AC_HEADER_TIME

通用 header checks：

AC_CHECK_HEADER(header-file, [action-if-found], [action-if-not-found], [includes])
AC_CHECK_HEADERS(header-file..., [action-if-found], [action-if-not-found], [includes])
遍历给定头文件，如果存在，则定义 HAVE_header-file

AC_CHECK_HEADERs([foo.h])
AC_CHECK_HEADERS([bar.h], [], [],
[#ifdef HAVE_FOO_H
#include
#endif
])
5.6 声明

下面的宏检查变量和函数的声明。

AC_CHECK_DECL(symbol, [action-if-found], [action-if-not-found], [includes='AC_INCLUDES_DEFAULT'])
如果符号（函数、变量或者常数）没有被 includes 声明，则执行 action-if-not-found. 否则执行 action-if-found.
includes 是头文件包含指令，缺省为 AC_INCLUDES_DEFAULT.

检查结果缓存在变量 `ac_cv_have_decl_symbol` 变量中。

AC_CHECK_DECLS(symbols, [action-if-found], [action-if-not-found], [includes='AC_INCLUDES_DEFAULT'])
对于每个符号，如果声明了，则定义 HAVE_DECL_symbol 为1，否则为0.

#if defined HAVE_DECL_MALLOC && !HAVE_DECL_MALLOC
void *malloc (size_t *s);
#endif
5.7 结构体

下面的宏检查C结构体是否包含特定的域。

特定的结构体检查

AC_STRUCT_DIRENT_D_INFO: 检查 struct dirent 是否包含 d_ino。
AC_STRUCT_DIRENT_D_TYPE：检查 struct dirent 是否包含 d_type.
AC_STRUCT_ST_BLOCKS
AC_STRUCT_TM
AC_STRUCT_TIMEZONE: struct tm 包含 tm_zone?

通用结构体检查

AC_CHECK_MEMBER(aggrgate.member, [action-if-found], [action-if-not-found], [includes='AC_INCLUDES_DEFAULT'])
检查 aggregate 是否含有 member。
AC_CHECK_MEMBER([struct passwd.pw_gecos], []
[AC_MSG_ERROR([we need 'passwd.pw_gecos'])],
[[#include ]])
检查结果保存在 ac_cv_member_aggregate_member 变量中。

AC_CHECK_MEMBERS(members, [action-if-found], [action-if-not-found], [includes='AC_INCLUDES_DEFAULT'])
检查每个 aggregate.member，如果存在则定义 HAVE_aggregate_member.
AC_CHECK_MEMBERS([struct stat.st_rdev, struct stat.st_blksize])
5.8 类型

下面的宏检查c类型。

特定类型检查：下面的宏检查 sys/types.h, stdlib.h, stdint.h, inttypes.h 中得c类型。
Gnulib 的 stdint 模块定义了更好的办法。如果使用C99，建议使用 stdint。

AC_TYPE_GETGROUPS

AC_TYPE_INT8_T: 如果 stdint.h 或者 inttypes.h 没有定义 int8_t, 则定义 int8_t 。
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_INTMAX_T: 检查 inttypes.h 是否定义了类型 intmax_t 并定义 HAVE_INTMAX_T
AC_TYPE_INTPTR_T: intptr_t 和 HAVE_INTPTR_T
AC_TYPE_LONG_DOUBLE
AC_TYPE_LONG_DOUBLE_WIDER
AC_TYPE_LONG_LONG_INT
AC_TYPE_MBSTATE_T
AC_TYPE_MODE_T
AC_TYPE_OFF_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UID_T
AC_TYPE_UINT8_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINTMAX_T
AC_TYPE_UINTPTR_T
AC_TYPE_UNSIGED_LONG_LONG_INT

通用类型检查

AC_CHECK_TYPE(type, [action-if-found], [action-if-not-found], [includs='AC_INCLUDES_DEFAULT'])
检查type是否定义了。结果保存在 ac_cv_type_type 变量中。

AC_CHECK_TYPES(types, [action-if-found], [action-if-not-found], [includes='AC_INCLUDES_DEFAULT'])
对每个定义的types 类型，定义宏 HAVE_type.
AC_CHECK_TYPES([unisgned long long int, uintmax_t])
5.9 编译器和预处理器

编译器测试宏 (AC_PROG_CC, AC_PROG_CXX, AC_PROG_F77) 根据编译器的输出定义输出变量 EXEEXT， OBJEXT 等。

AC_CHECK_SIZEOF
AC_CHECK_ALIGNOFF
AC_COMPUTE_INT
AC_LANG_WERROR
AC_OPENNMP
c 编译器特性检查

AC_PROG_CC([compiler-search-list])
确定使用哪个 c 编译器, 如果 CC 没有设置，则检查 gcc 和 cc，然后是其他编译器。设置输出变量 CC 为找到的编译器。
AC_PROG_CC([gcc cl cc])

AC_PROG_CC_C_O
AC_PROG_CC_STDC
AC_PROG_CC_C89
AC_PROG_CC_C99
AC_C_BACKSLASH_A
AC_C_BIGENDIAN
AC_C_CONST
AC_C_RESTRICT
AC_C_VOLATILE
AC_C_INLINE
AC_C_CHAR_UNSIGNED
AC_C_STRINGIZE
AC_C_FLEXIBLE_ARRAY_MEMBER
AC_C_VARARRAYS
AC_C_TYPEOF
AC_C_PROTOTYPES
AC_PROG_GCC_TRADITONAL
Go 编译器特性

AC_PROG_GO
AC_PATH_X
AC_PATH_XTRA
6. 实现 Autoconf 测试

若已有的特性测试满足不了要求，则可以自定义测试宏。

6.1 语言选择

自动生成的 configure 脚本默认检查 c 编译器和特性。如果使用其他语言，则用下面的宏

AC_LANG(language)
6.2 实现测试宏

Autoconf 的测试遵循一个规则：传递某些输入给某些程序；大多数情况下，使用编译器编译某些源文件。

测试程序不要使用标准输出，如果测试成功，返回退出状态0，否则返回1. 不要直接使用 exit。

测试程序可以使用宏定义：#if, #ifdef 等。

Autoconf 提供了一组宏来生成测试源代码。

AC_LANG_CONFTEST(source)
保存 source 代码到当前测试源文件 conftest.extension 中。 extension 依赖于当前语言。

source 内容仅被执行一次，类似于其他Autoconf宏参数，因而 1）you may pass a macro invocation; 2) if not, be sure double quote if needed.

通常不会直接使用这个宏，而是使用 AC_COMPILE_IFELSE, AC_RUN_IFELSE.

AC_LANG_DEFINES_PROVIDED

AC_LANG_SOURCE(source)
AC_LANG_PROGRAM(prologue, body)
AC_LANG_CALL(Prologue, function)
AC_LANG_FUNC_LINK_TRY
6.3 运行预处理器

AC_PREPROC_IFELSE (input, [action-if-true], [action-if-false])

AC_INIT([Hello], [1.0], [bug-hello@example.org])
AC_DEFINE([HELLO_WORLD], ["Hello, World\n"],
[Greetings string.])
AC_PREPROC_IFELSE(
[AC_LANG_PROGRAM([[const char hw[] = "Hello, World\n";]],
[[fputs (hw, stdout);]])],
[AC_MSG_RESULT([OK])],
[AC_MSG_FAILURE([unexpected preprocessor failure])])
6.4 运行编译器

检查当前语言编译器的语法特征，例如是否识别某个关键字等。

AC_COMPILE_IFELSE(input, [action-if-true], [action-if-false])
根据当前的语言的编译器和编译选项，编译给地的 input，如果成功，运行shell命令：action-if-true, 否则执行 action-if-false.

input 通常使用 AC_LANG_PROGRAM 实现。
6.5 运行连接器

需要检查库、函数和全局变量是， configure 脚本试图编译并链接使用他们的一个小程序。

AC_LINK_IFELSE(input, [action-if-true], [action-if-false])
用于编译测试函数，并测试函数和全局变量。 AC_CHECK_LIB 常使用它来检查库是否存在。
6.6 检查运行时行为

有时候需要检查系统运行时的情况，例如某个函数是否具备某种行为或者含有某个bug。这种问题建议运行时检查，而不是配置时。 如果需要配置时检查，则使用 AC_RUN_IFELSE.

6.7 条件语句

Autoconf 本身不支持条件语句，但是可以使用 shell 来实现。

AC_MSG_CHECKING([how to get file system type])

fstype=no
# The order of these tests is important.
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include
#include ]])],
[AC_DEFINE([FSTYPE_STATVFS], [1], [Define if statvfs exists.])
fstype=SVR4])
if test $fstype = no; then
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include
#include ]])],
[AC_DEFINE([FSTYPE_USG_STATFS], [1], [Define if USG statfs.])
fstype=SVR3])
fi
if test $fstype = no; then
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include
#include ]])]),
[AC_DEFINE([FSTYPE_AIX_STATFS], [1], [Define if AIX statfs.])
fstype=AIX])
fi
# (more cases omitted here)
AC_MSG_RESULT([$fstype])
7. 测试结果

有四种方式处理 configure 的测试结果：

定义一个 C 预处理符号
设置输出变量
保存到 cache 文件中
打印一条消息
7.1 定义 c 预处理器符号

最常见的方式是使用 AC_DEFINE、AC_DEFINE_UNQUOTED 保存测试结果。

AC_DEFINE 的常用方式是和 AC_CONFIG_HEADERS 合用，如果测试成功，则使用 #define 替换调用模板文件中的响应语句，最终 AC_OUTPUT 回生成适当的头文件。

AC_DEFINE(variable, value, [description])
AC_DEFINE(variable)
定义变量为 value（字面量）

AC_DEFIINE_UNQUOTED(varialbe,value, [description])
AC_DEFIINE_UNQUOTED(variable)
类似 AC_DEFINE，但是会对变量和值执行3种shell扩展：变量扩展($), 命令替换(`), 和转义（\)。 单引号和双引号没有特殊含义。
如果变量或者value是个shell变量，则使用这种方式。

AC_DEFINE_UNQUOTED([config_machfile], ["$machfile"], [Configuration machine file.])
AC_DEFINE_UNQUOTED([GETGROUPS_T], [$ac_cv_type_getgroups], [getgroups return type.])
AC_DEFINE_UNQUOTED([$ac_tr_hdr], [1], [Translated header name.])

和别的宏连用
AC_CHECK_HEADER([elf.h],
[AC_DEFINE([SVR4], [1], [System V Release 4]) LIBS="-lelf $LIBS"])
7.2 设置输出变量

输出变量是 shell 变量，在configure输出文件时，会进行替换。

AC_SUBST(variable, [value])
根据shell变量创建输出变量。 AC_OUTPUT 会对输出文件（通常是 Makeifles）中的变量进行替换。也就是 AC_OUTPUT 被调用时，它会
替换输入文件中的所有 @variable@ 为 variable 的值。

AC_SUBST_FILE(variable)
AC_OUTPUT 将 @variable@ 替换为文件的内容

AC_ARG_VAR(variable, description)
7.3 输出变量中得特殊符号

很多输出变量会被 make 和 shell 处理，他们在不同的上下文会执行不同欧冠的替换，所以为了避免混淆，尽量不使用一些特殊字符。

" # $ & ’ ( ) * ; < > ? [ \ ^ ‘ |
7.4 缓存结果

configure 可以保存check结果到缓存文件中，避免重复的检查某些特性。如果启用了缓存，并且缓存文件存在，则configure会从里面读检查的 结果，而不是重复运行检查。

AC_CACHE_VAL(cache-id, commands-to-set-it)
AC_CACHE_CHECK(message, cache-id, commands-to-set-it)
7.5 打印消息

下面的宏可以展示消息给用户，这些参数都是用shell的双引号括起来，以便 shell 可以执行变量替换和命令替换。

AC_MSG_CHECKING(feature-description)
AC_MSG_RESULT(result-description)
这两个要合用。

AC_MSG_NOTICE
AC_MSG_ERROR
AS_MSG_FAILURE
AC_MSG_WARN
8. M4 编程

Autoconf 构建与2个层次之上：M4sugar, 提供宏方便纯 M4 编程； M4sh， 提供宏用于shell脚本生成。

8.1 M4 quotation

Autoconf 的特殊字符：

: 注释，不会执行宏扩展

,: 分割参数
[]: 本身是转义字符
()
$
每次发生宏扩展，也会执行转义（quotation expansion），例如一个级别的quotes被去掉

int tab[10]
=> int tab10
[int tab[10]]
=> int tab[10]
8.2 使用 autom4te

8.3 使用 M4sugar 编程

8.4 通过 autom4te 调试

9. M4sh 编程

10. 实现 Autoconf 宏