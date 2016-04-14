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