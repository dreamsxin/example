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