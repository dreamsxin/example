# PHP-CPP 使用

## 下载安装
```shell
git clone https://github.com/CopernicaMarketingSoftware/PHP-CPP.git
cd PHP-CPP && make && sudo make install
```

## 新建扩展 hello
```shell
mkdir hello
vi hello\Makefile
vi hello\hello.ini
vi Main.cpp
```
`Makefile` 内容
```Makefile
NAME                =   hello

INI_DIR             =   /etc/php5/conf.d
EXTENSION_DIR       =   $(shell php-config --extension-dir)
EXTENSION           =   ${NAME}.so
INI                 =   ${NAME}.ini

COMPILER            =   g++
LINKER              =   g++

COMPILER_FLAGS      =   -Wall -c -O2 -std=c++11 -fpic -o
LINKER_FLAGS        =   -shared
LINKER_DEPENDENCIES =   -lphpcpp

RM                  =   rm -f
CP                  =   cp -f
MKDIR               =   mkdir -p

SOURCES             =   $(wildcard *.cpp)
OBJECTS             =   $(SOURCES:%.cpp=%.o)

all:                    ${OBJECTS} ${EXTENSION}

${EXTENSION}:           ${OBJECTS}
                        ${LINKER} ${LINKER_FLAGS} -o $@ ${OBJECTS} ${LINKER_DEPENDENCIES}

${OBJECTS}:
                        ${COMPILER} ${COMPILER_FLAGS} $@ ${@:%.o=%.cpp}

install:        
                        ${CP} ${EXTENSION} ${EXTENSION_DIR}
                        ${CP} ${INI} ${INI_DIR}

clean:
                        ${RM} ${EXTENSION} ${OBJECTS}
```

`hello.ini` 内容
```ini
extension=hello.so
```

`Main.cpp` 内容
```cpp
#include <phpcpp.h>

/**
 *  tell the compiler that the get_module is a pure C function
 */
extern "C" {

    PHPCPP_EXPORT void *get_module() 
    {
        static Php::Extension extension("hello", "1.0");

        return extension;
    }
}
```

## 编译安装
```shell
make && sudo make install
```

## 测试扩展是否安装成功
```shell
php --ri hello
```

## 编写第一个函数
```cpp
#include <phpcpp.h>
#include <iostream>

void helloWorld()
{
    // the C++ equivalent of the echo() function
    Php::out << "hello world" << std::endl;

    // generate output without a newline, and ensure that it is flushed
    Php::out << "hello world" << std::flush;

    // or call the flush() method
    Php::out << "hello world";
    Php::out.flush();

    // just like all PHP functions, you can call the echo() function 
    // from C++ code as well
    Php::echo("hello world\n");

    // generate a PHP notice
    Php::notice << "this is a notice" << std::flush;

    // generate a PHP warning
    Php::warning << "this is a warning" << std::flush;

    // inform the user that a call to a deprecated function was made
    Php::deprecated << "this method is deprecated" << std::flush;

    // generate a fatal error
    Php::error << "fatal error" << std::flush;

    // this code will no longer be called
    Php::out << "regular output" << std::endl;
}

extern "C" {
    PHPCPP_EXPORT void *get_module() {
        static Php::Extension extension("hello", "1.0");
        extension.add("helloworld", helloWorld);
        return extension;
    }
}
```

## 编写带返回值的函数
```cpp

#include <phpcpp.h>
#include <iostream>

void helloWorld()
{
    // the C++ equivalent of the echo() function
    Php::out << "hello world" << std::endl;

    // generate output without a newline, and ensure that it is flushed
    Php::out << "hello world" << std::flush;

    // or call the flush() method
    Php::out << "hello world";
    Php::out.flush();

    // just like all PHP functions, you can call the echo() function 
    // from C++ code as well
    Php::echo("hello world\n");

    // generate a PHP notice
    Php::notice << "this is a notice" << std::flush;

    // generate a PHP warning
    Php::warning << "this is a warning" << std::flush;

    // inform the user that a call to a deprecated function was made
    Php::deprecated << "this method is deprecated" << std::flush;

    // generate a fatal error
    Php::error << "fatal error" << std::flush;

    // this code will no longer be called
    Php::out << "regular output" << std::endl;
}

Php::Value sum(Php::Parameters &parameters)
{
    int result = 0;
    for (auto &param : parameters) result += param;
    return result;
}

extern "C" {
    PHPCPP_EXPORT void *get_module() {
        static Php::Extension extension("hello", "1.0");
        extension.add("helloworld", helloWorld);
	extension.add("sum", sum);
        return extension;
    }
}
```