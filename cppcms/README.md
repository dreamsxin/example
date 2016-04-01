# CppCMS 安装
```shell
sudo apt-get install cmake libpcre3-dev zlib1g-dev libgcrypt11-dev libicu-dev python
svn co https://svn.code.sf.net/p/cppcms/code/framework/trunk cppcms
cd cppcms && mkdir build && cd build && cmake .. && make && make test && sudo make install  
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
c++ -Wl,-rpath=/usr/local/lib hello.cpp -lcppcms -o hello
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

## 访问 http://localhost:8080/hello

## 编译 CppDB `http://cppcms.com/sql/cppdb/`
```shell
svn co http://cppcms.svn.sourceforge.net/svnroot/cppcms/cppdb/trunk cppdb
cd cppdb && mkdir build && cd build && cmake .. && make && sudo make install
```

## 更新代码
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

## 编译
```shell
c++ -Wl,-rpath=/usr/local/lib hello.cpp -lcppcms -lcppdb -o hello
```