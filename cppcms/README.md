# CppCMS 安装
```shell
sudo apt-get install cmake libpcre3-dev zlib1g-dev libgcrypt11-dev libicu-dev python
svn co https://svn.code.sf.net/p/cppcms/code/framework/trunk cppcms
cd cppcms && mkdir build && cd build && cmake .. && make && make test && make install  
```

# 创建 Hello World

## 新建文件 `hello.cpp`
```cpp
#include <cppcms/application.h>
#include <cppcms/applications_pool.h>
#include <cppcms/service.h>
#include <cppcms/http_response.h>
#include <iostream>

class hello : public cppcms::application {
public:
    hello(cppcms::service &srv) :
        cppcms::application(srv)
    {
    }
    virtual void main(std::string url);
};

void hello::main(std::string /*url*/)
{
    response().out() <<
        "<html>\n"
        "<body>\n"
        "  <h1>Hello World</h1>\n"
        "</body>\n"
        "</html>\n";
}


int main(int argc,char ** argv)
{
    try {
        cppcms::service srv(argc,argv);

        srv.applications_pool().mount(
          cppcms::applications_factory<hello>()
        );

        srv.run();
    } catch(std::exception const &e) {
        std::cerr << e.what() << std::endl;
    }
}
```

## 编译
```shell
c++ -Wl,-rpath=/usr/local/lib -lcppcms hello.cpp -lcppcms -o hello
```

## 创建配置文件config.js
```json
{
    "service" : {
        "api" : "http",
        "port" : 8080
    },
    "http" : {
        "script_names" : [ "/hello" ]
    }
}
```

## 运行
```shell
 ./hello -c config.js
 ```