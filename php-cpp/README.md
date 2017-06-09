# PHP-CPP 使用

官方文档 http://www.phpcpp.com/documentation/
实例：https://github.com/dreamsxin/cpphalcon

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

INI_DIR             =   /etc/php5/cli/conf.d
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

Php::Value sum(Php::Parameters &parameters)
{
    int result = 0;
    for (auto &param : parameters) result += param;
    return result;
}

extern "C" {
    PHPCPP_EXPORT void *get_module() {
        static Php::Extension extension("hello", "1.0");
        extension.add("sum", sum);
        return extension;
    }
}
```

## 类型

```cpp
enum class ClassType {
    Regular     =   0x00,
    Abstract    =   0x20,
    Final       =   0x04,
    Interface   =   0x40,
};
```

```cpp
enum class Type : unsigned char {
    Undefined       =   0,  // Variable is not set
    Null            =   1,  // Null will allow any type
    False           =   2,  // Boolean false
    True            =   3,  // Boolean true
    Numeric         =   4,  // Integer type
    Float           =   5,  // Floating point type
    String          =   6,  // A string obviously
    Array           =   7,  // An array of things
    Object          =   8,  // An object
    Resource        =   9,  // A resource
    Reference       =  10,  // Reference to another value (can be any type!)
    Constant        =  11,  // A constant value
    ConstantAST     =  12,  // I think an Abstract Syntax tree, not quite sure

    // "fake types", not quite sure what that means
    Bool            = 13,   // You will never get this back as a type
    Callable        = 14,   // I don't know why this is a "fake" type
};
```

## 接口与继承

* 创建接口

```cpp
// create static instance of the extension object
static Php::Extension myExtension("my_extension", "1.0");

Php::Interface interface("MyInterface");

interface.method("myMethod", { 
	Php::ByVal("value", Php::Type::String, true) 
});

myExtension.add(std::move(interface));
```

* 实现接口

```cpp
// register our own class
Php::Class<MyClass> myClass("MyClass");

// from PHP user space scripts, it must look like the myClass implements
// the MyInterface interface
myClass.implements(myInterface);

// the interface requires that the myMethod method is implemented
myClass.method<&MyClass::myMethod>("myMethod", {
	Php::ByVal("value", Php::Type::String, true) 
});

myExtension.add(myClass);
```

* 继承

```cpps
 // create a third class
Php::Class<DerivedClass> derivedClass("DerivedClass");

// in PHP scripts, it should look like DerivedClass has "MyClass" as its base
derivedClass.extends(myClass);
myExtension.add(derivedClass);
```

> 接口和继承有顺序要求，实现接口和继承的类必须位于接口和基类之后注册。

## 源码分析

* Php::Extension

他继承自 `Php::Namespace`，拥有`onStartup`、`onShutdown`、`onRequest`、`onIdle`、`onStartup`等成员方法。

* Php::Namespace

上面用到的 `add` 方法就来自这个类。
下面是函数、类、常量的注册源码，带命名空间的在没有命名空间之后注册，所以没带命名空间的类无法继承带命名空间的类：

```cpp
/**
 *  Apply a callback to each registered function
 * 
 *  The callback will be called with the name of the namespace, and
 *  a reference to the registered function.
 * 
 *  @param  callback
 */
void Namespace::functions(const std::function<void(const std::string &ns, NativeFunction &func)> &callback)
{
    // loop through the functions, and apply the callback
    for (auto &function : _functions) callback(_name, *function);
    
    // loop through the other namespaces
    for (auto &ns : _namespaces) ns->functions([this, callback](const std::string &ns, NativeFunction &func) {
        
        // if this is the root namespace, we don't have to change the prefix
        if (_name.size() == 0) return callback(ns, func);
        
        // construct a new prefix
        // @todo this could be slightly inefficient
        return callback(_name + "\\" + ns, func);
    });
}

/**
 *  Apply a callback to each registered class
 * 
 *  The callback will be called with the name of the namespace, and
 *  a reference to the registered class.
 * 
 *  @param  callback
 */
void Namespace::classes(const std::function<void(const std::string &ns, ClassBase &clss)> &callback)
{
    // loop through the classes, and apply the callback
    for (auto &c : _classes) callback(_name, *c);
    
    // loop through the other namespaces
    for (auto &ns : _namespaces) ns->classes([this, callback](const std::string &ns, ClassBase &clss) {
        
        // if this is the root namespace, we don't have to change the prefix
        if (_name.size() == 0) return callback(ns, clss);
        
        // construct a new prefix
        // @todo this could be slightly inefficient
        return callback(_name + "\\" + ns, clss);
    });
}

/**
 *  Apply a callback to each registered constant
 * 
 *  The callback will be called with the name of the namespace, and
 *  a reference to the registered constant
 * 
 *  @param  callback
 */
void Namespace::constants(const std::function<void(const std::string &ns, Constant &constant)> &callback)
{
    // loop through the constants, and apply the callback
    for (auto &c : _constants) callback(_name, *c);
    
    // loop through the other namespaces
    for (auto &ns : _namespaces) ns->constants([this, callback](const std::string &ns, Constant &constant) {
        
        // if this is the root namespace, we don't have to change the prefix
        if (_name.size() == 0) return callback(ns, constant);
        
        // construct a new prefix
        // @todo this could be slightly inefficient
        return callback(_name + "\\" + ns, constant);
    });
}
```

* Php::ExtensionImpl

```cpp
/**
 *  Constructor
 *  @param  data        Pointer to the extension object created by the extension programmer
 *  @param  name        Name of the extension
 *  @param  version     Version number
 *  @param  apiversion  API version number
 */
ExtensionImpl::ExtensionImpl(Extension *data, const char *name, const char *version, int apiversion) :
    ExtensionBase(data)
{
    // keep extension pointer based on the name
    name2extension[name] = this;

    // assign all members (apart from the globals)
    _entry.size = sizeof(zend_module_entry);                       // size of the data
    _entry.zend_api = ZEND_MODULE_API_NO;                          // api number
    _entry.zend_debug = ZEND_DEBUG;                                // debug mode enabled?
    _entry.zts = USING_ZTS;                                        // is thread safety enabled?
    _entry.ini_entry = NULL;                                       // the php.ini record, will be filled by Zend engine
    _entry.deps = NULL;                                            // dependencies on other modules
    _entry.name = name;                                            // extension name
    _entry.functions = NULL;                                       // functions supported by this module (none for now)
    _entry.module_startup_func = &ExtensionImpl::processStartup;   // startup function for the whole extension
    _entry.module_shutdown_func = &ExtensionImpl::processShutdown; // shutdown function for the whole extension
    _entry.request_startup_func = &ExtensionImpl::processRequest;  // startup function per request
    _entry.request_shutdown_func = &ExtensionImpl::processIdle;    // shutdown function per request
    _entry.info_func = NULL;                                       // information for retrieving info
    _entry.version = version;                                      // version string
    _entry.globals_size = 0;                                       // size of the global variables
    _entry.globals_ctor = NULL;                                    // constructor for global variables
    _entry.globals_dtor = NULL;                                    // destructor for global variables
    _entry.post_deactivate_func = NULL;                            // unknown function
    _entry.module_started = 0;                                     // module is not yet started
    _entry.type = 0;                                               // temporary or persistent module, will be filled by Zend engine
    _entry.handle = NULL;                                          // dlopen() handle, will be filled by Zend engine
    _entry.module_number = 0;                                      // module number will be filled in by Zend engine
    _entry.build_id = (char *)ZEND_MODULE_BUILD_ID;                // check if extension and zend engine are compatible

    // things that only need to be initialized
#ifdef ZTS
    _entry.globals_id_ptr = NULL;
#else
    _entry.globals_ptr = NULL;
#endif

    // everything is ok if the api versions match
    if (apiversion == PHPCPP_API_VERSION) return;

    // mismatch between api versions, the extension is invalid, we use a
    // different startup function to report to the user
    _entry.module_startup_func = &ExtensionImpl::processMismatch;

    // the other callback functions are no longer necessary
    _entry.module_shutdown_func = nullptr;
    _entry.request_startup_func = nullptr;
    _entry.request_shutdown_func = nullptr;
}
```