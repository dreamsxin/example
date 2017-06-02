# Windows下使用 VS2010 编译 PHP 扩展

官方教程：https://wiki.php.net/internals/windows/stepbystepbuild

需要说明的是，官方教程所用的是VS2008 Express版。
这里使用的是 vs2010（VS2012也是支持的），已经包含了windows sdk，所以也就不需要下载windows SDK了。

## 配置 PHP-SDK

1、首先到 http://windows.php.net/downloads/php-sdk/ 下载PHP-SDK，这些是编译PHP过程中需要的一些辅助工具，如词法、语法解析等。下载最新的php-sdk-binary-tools-20110915.zip，解压到c:\php-sdk。注意，解压后，这个目录下应该有bin和script、share三个子目录。

2、打开CMD，切换到到c:\php-sdk目录下，运行 vcvarsall.bat (位置根据VS2010的安装位置而定) 来设置好编译环境变量。

```cmd
X:\vs2010\VC\vcvarsall.bat
bin\phpsdk_setvars.bat
bin\phpsdk_buildtree.bat php54dev
```
这样，在c:\php-sdk目录下又多了一个php54dev目录

3、接着继续在php-sdk目录下载deps-5.4-vc9-x86.7z，并解压到 c:\php-sdk\php54dev\vc9\x86\目录下。

4、下载PHP5.4 最新源代码。到 http://snaps.php.net/ 下载 php-5.4-dev (tar.xz) ，解压目录下所有文件到c:\php-sdk\php54dev\vc9\x86\deps\php-5.4目录（此处php-5.4为我们新建的目录）。


## 编译 PHP

进入PHP源码目录，c:\php-sdk\php54dev\vc9\x86\deps\php-5.4，运行buildconf，之后我们就可以用configure --help 命令查看构建选项了。
可以先试着编译php了。最开始，一般编译尽可能少的模块，先让核心能正常编译出来。

```cmd
configure --disable-all --enable-cli
nmake
```

稍等一会，php5.4最核心部分及一个命令行工具就编译出来了（这中间的警告信息可以不用顾及）。

```cmd
cd Release_TS
php -m
```

就可以看到
```output
[PHP Modules]
Core
date
ereg
pcre
Reflection
SPL
standard
[Zend Modules]
```

然后我们也可以试着加入更多的模块，比如：

```cmd
configure --disable-all --enable-cli --with-sqlite3 --with-mcrypt=static --with-mysqlnd --with-mysql=mysqlnd --with-mysqli=mysqlnd --with-gd --with-mhash --with-curl --with-gettext --with-iconv --enable-mbstring --enable-session
```

然后运行nmake重新编译。

一些扩展的编译需要依赖第三方库，可以到 https://wiki.php.net/internals/windows/libs 下载。
如果修改了源码等需要再次编译，可运行 make clean，然后重新 buildconf, configure.

## 编译 php 扩展 phalcon

把 cphalcon\build\32bits 目录拷贝到 c:\php-sdk\php54dev\vc9\x86\pecl\phalcon 或者 c:\php-sdk\php54dev\vc9\x86\php-5.4\ext\phalcon

```cmd
cd c:\php-sdk\php54dev\vc9\x86\php-5.4
buildconf
```

然后可以查看一下可配置的选项：

```cmd
configure --help
```

可以发现，多了一个参数选项：
```cmd
 --enable-phalcon              enable phalcon framework
```

再次配置并编译：
```cmd
configure --disable-all --enable-cli --enable-phalcon
nmake
```
或者
```cmd
configure --disable-all --enable-cli --enable-phalcon=shared
nmake
```

如果本地 PHP 加载 phalcon 扩展出错出现类似错误：
```error
PHP Warning:  PHP Startup: phalcon: Unable to initialize module
Module compiled with build ID=API20100525,TS,VC10
PHP    compiled with build ID=API20100525,TS,VC9
These options need to match
 in Unknown on line 0

Warning: PHP Startup: phalcon: Unable to initialize module
Module compiled with build ID=API20100525,TS,VC10
PHP    compiled with build ID=API20100525,TS,VC9
```
请根据本地 PHP 版本修改 PHP 源码 main/config.w32.h 中 PHP_COMPILER_ID
```c
#define PHP_COMPILER_ID "VC10"
```
为
```c
#define PHP_COMPILER_ID "VC9"
```
