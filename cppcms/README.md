# CppCMS 安装

```shell
sudo apt-get install cmake libpcre3-dev zlib1g-dev libgcrypt11-dev libicu-dev python
svn co https://svn.code.sf.net/p/cppcms/code/framework/trunk cppcms
cd cppcms && mkdir build && cd build && cmake .. && make && make test && sudo make install  
```

## 创建 Hello World

- `cppcms/application.h`
the class application that every CppCMS application is derived from.
每个应用都继承自这个 `application` 类。 

- `cppcms/applications_pool.h`
class applications_pool - the class that manages user applications - creates new per HTTP request and caches them for future use.
作为管理用户应用程序的 `applications_pool` 类，创建新的 HTTP 请求并将其缓存以供将来使用。

- `cppcms/service.h`
the service - central event loop of the network service.
`service` 是整个网络服务中的中央事件循环服务。

- `cppcms/http_response.h`
the class we write our response to the client with.
使用该类发送我们的响应给对应的客户端连接。


* 新建文件 `hello.cpp`

```cpp
#include <cppcms/application.h>
#include <cppcms/applications_pool.h>
#include <cppcms/service.h>
#include <cppcms/http_response.h>
#include <iostream>

// 定义第一个应用类 hello
class hello : public cppcms::application {
public:
    hello(cppcms::service &srv) :
        cppcms::application(srv)
    {
    }
    virtual void main(std::string url);
};

// 覆盖成员函数 cppcms::application::main，返回响应数据
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
		// 创建服务类，解析命令行参数并搜索配置选项
        cppcms::service srv(argc,argv);

		// 告知 applications_pool 类使用应用类 hello
        srv.applications_pool().mount(
          cppcms::applications_factory<hello>()
        );

		// 运行事件循环
        srv.run();
    } catch(std::exception const &e) {
        std::cerr << e.what() << std::endl;
    }
}
```

* 编译
```shell
c++ -Wl,-rpath=/usr/local/lib hello.cpp -lcppcms -o hello
```

* 创建配置文件 config.js

配置服务监听端口为 `8080`，使用 `HTTP` 协议。
提供 `script_names` 参数，定义的虚拟 CGI 脚本路径或简单一句话，我们的应用程序需要通过这个路径才能访问。

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

* 运行

```shell
 ./hello -c config.js
```

打开浏览器访问 `http://localhost:8080/hello` 看输出结果

## 使用 CppDB

`http://cppcms.com/sql/cppdb/`

* 编译 CppDB 

```shell
svn co http://cppcms.svn.sourceforge.net/svnroot/cppcms/cppdb/trunk cppdb
cd cppdb && mkdir build && cd build && cmake .. && make && sudo make install
```

* Sqlite3 读写

```shell
        try {
                cppdb::session sql("sqlite3:db=db.db");
                
                sql << "DROP TABLE IF EXISTS test" << cppdb::exec;

                sql<<   "CREATE TABLE test ( "
                        "   id   INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
                        "   n    INTEGER,"
                        "   f    REAL, "
                        "   t    TIMESTAMP,"
                        "   name TEXT "
                        ")  " << cppdb::exec;
                
                std::time_t now_time = std::time(0);
                
                std::tm now = *std::localtime(&now_time);

                cppdb::statement stat;
                
                stat = sql << 
                        "INSERT INTO test(n,f,t,name) "
                        "VALUES(?,?,?,?)"
                        << 10 << 3.1415926565 << now <<"Hello 'World'";

                stat.exec();
                std::cout<<"ID: "<<stat.last_insert_id() << std::endl;
                std::cout<<"Affected rows "<<stat.affected()<<std::endl;
                
                stat.reset();

                stat.bind(20);
                stat.bind_null();
                stat.bind(now);
                stat.bind("Hello 'World'");
                stat.exec();

                cppdb::result res = sql << "SELECT id,n,f,t,name FROM test";

                while(res.next()) {
                        double f=-1;
                        int id,k;
                        std::tm atime;
                        std::string name;
                        res >> id >> k >> f >> atime >> name;
                        std::cout <<id << ' '<<k <<' '<<f<<' '<<name<<' '<<asctime(&atime)<< std::endl;
                }

                res = sql << "SELECT n,f FROM test WHERE id=?" << 1 << cppdb::row;
                if(!res.empty()) {
                        int n = res.get<int>("n");
                        double f=res.get<double>(1);
                        std::cout << "The values are " << n <<" " << f << std::endl;
                }
        } catch(std::exception const &e) {
                std::cerr << "ERROR: " << e.what() << std::endl;
                return 1;
        }
```

* 编译

```shell
c++ -Wl,-rpath=/usr/local/lib hello.cpp -lcppcms -lcppdb -o hello
```

## CPPCMS URL指派

1、root URL

配置文件中的 `http.script_names` 指定名称与`mapper().root(URL)`相同时，root URL才有效。不相同时，配置文件中的有效，`mapper().root(URL)`指定的无效，所有`url()`函数生成的地址将是错误的。代码与配置文件都不指定，将无法访问根地址。入口application默认呼叫方法的正则设置为`/?`可以是没有指定root URL时，单纯的/成为根地址。
 
设定`mapper().root(URL)`，例如指定/root，那么`url("/")`返回`/root`，不设置root URL，第二层以下子路径调用url("/")，路径将变成路径最后多出一个/，地址出现错误。
 
2、成员方法指定URL，dispatcher().assign(regx, fun, this);给application的成员方法指定URL，第一个参数是正则表达式。`mapper().assign(name, url);`是将URL以键值存入mapper，通过`url()`函数取出。两方法没有直接关系，但是成对出现可以方便自己取成员方法关联的URL。
 
3、application 最好实现一个默认呼叫的成员方法，作为索引页。默认方法关联的地址为字符串空，例如：
```cpp
dispatcher().assign("", fun, this);
mapper().assign("", url);
```
 
4、入口 application 最好不要覆盖`virtual void main(std::string path)`方法，就算覆盖了也要在其中呼叫父类的`void main(std::string path)`方法，不然就不能访问挂在其下面的子application，而且 main 内用url也获得不到正确的值。因为最好是实现一个普通方法通过dispatcher和mapper去指定默认调用。
 
5、子application中通过url取实现的普通成员方法的URL地址时需带上父application中为其指定url。

例如：父类构造函数中通过`attach(子application实例, 名字, URL"/xxxx", 正则, 1);`,子application的普通成员方法指定的URL是：`/fun`，那么通过url()函数取就是`url("/xxxx/fun")`;
 
6、关联另一个application，通过`attach(子application实例, 名字, URL, 正则, 1);`,该子application不要覆盖void main(path）方法，就算覆盖也要呼叫父类的void main(）方法，不然通过url()方法将无法获得该子application的URL。
 
7、一点正则知识，`attach(new OtherApp(srv), "other","/other{1}", "/other(/(.*))?", 1);`
其中URL `/other{1}`,中的`{1}`是指从正则 `/other(/(.*))?`中分组得到的值。必须这样写正确，否则attach子app程序将异常退出。