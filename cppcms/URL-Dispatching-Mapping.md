# URL Dispatching and Mapping

http://cppcms.com/wikipp/en/page/cppcms_1x_tut_url_mapping

我们将学习如何轻松地将不同 URL 连接到它们的相关函数。

当客户端向服务端发送一个特定的 URL 请求时，被分为若干部分（CGI variables），例如 `/foo/bar.php/test?x=10`：

- SCRIPT_NAME = /foo/bar.php
- PATH_INFO = /test
- QUERY_STRING = x=10

CppCMS 应用中没有分为多个脚本，但是相当于单一的 FastCGI 应用。例如 `/myapp/page/10`，其中 `script_name = MyApp` 保持不变，`path_info = /page/10` 是根据用户的需要改变。

所以 URL 和 应用成员函数之间的映射是通过正则表达式匹配 `URL` 中的 `path_info` 部分完成的，这种行为可以通过使用 `mount_point` 类修改。

这项工作是由 `cppcms::url_dispatcher` 类完成。

## Mapping

重写 `hello`，加入头文件 `url_dispatcher.h` 和 `url_mapper.h`：
```cpp
#include <cppcms/application.h>
#include <cppcms/service.h>
#include <cppcms/http_response.h>
#include <cppcms/url_dispatcher.h>
#include <cppcms/url_mapper.h>
#include <cppcms/applications_pool.h>
#include <iostream>
#include <stdlib.h>

class hello : public cppcms::application {
public:
	hello(cppcms::service &srv) :
		cppcms::application(srv)
	{
		dispatcher().assign("/number/(\\d+)",&hello::number,this,1);
		mapper().assign("number","/number/{1}");

		dispatcher().assign("/smile",&hello::smile,this);
		mapper().assign("smile","/smile");

		dispatcher().assign("",&hello::welcome,this);
		mapper().assign("");

		mapper().root("/hello");
	}

	void number(std::string num)
	{
		int no = atoi(num.c_str());
		response().out() << "The number is " << no << "<br/>\n";
		response().out() << "<a href='" << url("/") << "'>Go back</a>";
	}

	void smile()
	{
		response().out() << ":-) <br/>\n";
		response().out() << "<a href='" << url("/") << "'>Go back</a>";
	}

	void welcome()
	{
		response().out() <<
			"<h1> Welcome To Page with links </h1>\n"
			"<a href='" << url("/number",1)  << "'>1</a><br>\n"
			"<a href='" << url("/number",15) << "'>15</a><br>\n"
			"<a href='" << url("/smile") << "' >:-)</a><br>\n";
	}

    virtual void main(std::string url);
};
```


## Root URL

配置文件中的 `http.script_names` 指定名称与`mapper().root(URL)` 相同时，`Root URL` 才有效。不相同时，配置文件中的有效，`mapper().root(URL)`指定的无效，所有 `url()` 函数生成的地址将是错误的。代码与配置文件都不指定，将无法访问根地址。入口 application 默认呼叫方法的正则设置为 `/?` 可以是没有指定 `Root URL` 时，单纯的 `/` 成为根地址。
 
设定 `mapper().root(URL)`，主要用于函数 `url` 生成 URL。例如指定 `/root`，那么 `url("/")` 返回 `/root`，不设置 `root URL`，第二层以下子路径调用url("/")，路径将变成路径最后多出一个/，地址出现错误。
 
## 成员方法指定 URL

dispatcher().assign(regx, fun, this);给application的成员方法指定URL，第一个参数是正则表达式。`mapper().assign(name, url);`是将URL以键值存入mapper，通过`url()`函数取出。两方法没有直接关系，但是成对出现可以方便自己取成员方法关联的URL。
 
## 默认呼叫的成员方法

默认方法关联的地址为字符串空，例如：
```cpp
dispatcher().assign("", fun, this);
mapper().assign("", url);
```
 
## 入口 application

最好不要覆盖 `virtual void main(std::string path)`方法，就算覆盖了也要在其中呼叫父类的`void main(std::string path)`方法，不然就不能访问挂在其下面的子application，而且 main 内用url也获得不到正确的值。因为最好是实现一个普通方法通过 dispatcher 和 mapper 去指定默认调用。
 
## 关联子 application

通过`attach(new numbers(srv), "numbers", "/numbers{1}", "/numbers(/(.*))?", 1);`
该子 `application` 不要覆盖 `void main(path)` 方法，就算覆盖也要呼叫父类的 `void main()` 方法，不然通过 `url` 方法将无法获得该子 `application` 的 URL。

## 子 application URL

普通成员方法的 URL 地址时需带上父 application 中为其指定 URL。

父类构造函数中指定：

`attach(new numbers(srv), "numbers", "/numbers{1}", "/numbers(/(.*))?", 1);`

子 application 的普通成员方法指定的 URL 是：`/fun`，那么通过 `url` 函数取就是 `url("/numbers/fun")`。
 
## 一点正则知识

`attach(new OtherApp(srv), "other","/other{1}", "/other(/(.*))?", 1);`

其中 URL `/other{1}`， 中的 `{1}` 是指从正则 `/other(/(.*))?` 中分组得到的第一个值。必须这样写正确，否则attach子app程序将异常退出。