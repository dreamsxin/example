# CppCMS 安装

```shell
sudo apt-get install cmake libpcre3-dev zlib1g-dev libgcrypt11-dev libicu-dev python
git clone https://github.com/artyom-beilis/cppcms.git
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

## FastCGI

```json
{  
	"service" : {  
		"api" : "fastcgi",  
		"socket" : "/tmp/hello-fcgi-socket"   
	}  
}
```

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
