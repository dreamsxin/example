# 模版 templates

为了使用模板，我们需要将源代码分成三部分：

1. 内容 content
content 对象中填充了应该呈现的数据。

2. 应用 application
application 产生需要填充的内容。

3. 渲染内容

## 创建模版

* 创建文件 `content.h`：

```cpp
#include <cppcms/view.h>
#include <string>

namespace content  {
	// 每个 content 对象都继承自 cppcms::base_content
    struct message : public cppcms::base_content {
        std::string text;
    };
}
```

* 创建模版文件 `my_skin.tmpl`：
```text
<% c++ #include "content.h" %>
<% skin my_skin %>
<% view message uses content::message %>
<% template render() %>
<html>
  <body>
    <h1><%= text %> World!</h1>
  </body>
</html>
<% end template %>
<% end view %>
<% end skin %>
```
`view message uses content::message` 定义视图名称为 `message`。

* 生成模版文件 `my_skin.cpp`

```shell
cppcms_tmpl_cc my_skin.tmpl -o my_skin.cpp
```

* 应用中渲染使用模版

```cpp
#include "content.h"

class hello : public cppcms::application {
public:
    hello(cppcms::service &srv) :
        cppcms::application(srv)
    {
    }
	
	virtual void main(std::string /*url*/)
	{
		content::message c;
		c.text=">>>Hello<<<";
		// 使用视图 `message`
		render("message",c);
		# render("my_skin", "message", c);
	}

};
```

## 静态连接编译

```shell
g++ hello.cpp my_skin.cpp -o hello -lcppcms -lbooster 
```

## 动态连接编译

* 动态库
```shell
g++ -shared -fPIC my_skin.cpp -o libmy_skin.so -lcppcms -lbooster
# OSX
g++ -shared -fPIC my_skin.cpp -o libmy_skin.dylib -lcppcms -lbooster
```

通过 `-rdynamic` 用来通知链接器将所有符号都添加到动态符号表（`.dynsym`）里，以便那些通过 `dlopen` 或 `backtrace` 等函数使用。
```shell
g++ -rdynamic hello.cpp -o hello -lcppcms -lbooster
# OSX
g++ -dynamic hello.cpp -o hello -lcppcms -lbooster
```

配置`config.js`：
```text
"views" : {
    "paths" : [ "." ],
    "skins" : [ "my_skin" ]
}
```

## 视图继承

CppCMS 视图实际上是一个类，因此我们可以使用类的方法来实现模版的覆盖。
这给开发人员提供了一个非常强大的工具来设计 HTML 模板。
例如，我们创建一个主视图，包括基本的 HTML 和 CSS 样式，并定义了占位符用来加载各种不同类型的内容。然后我们推导出不同视图实际需要渲染的内容。
当然，派生视图的内容也应该来自内容类的父类。

视图示例的层次结构：
```text
         master  
       /    |   \  
    news  page  intro  
```

* 定义内容类 Content

Let's create our `content.h` for the sample hierarchy above.
让我们根据上面的视图示例层次结构创建内容 `content.h`：
```cpp

#include <cppcms/view.h>
#include <string>

namespace content  {

	struct master : public cppcms::base_content {
		std::string title;
	};

	struct news : public master {
		std::list<std::string> news_list;
	};

	struct page : public master {
		std::string page_title, page_content;
	};
}
```

* 定义视图

`master.tmpl`
```text
<% c++ #include "content.h" %>
<% skin myskin %>
<% view master uses content::master %>

<% template title() %><%= title %><% end %>
<% template page_content() %>Override  Me<% end %>
<% template render() %>
<html>
<head>
    <title><% include title() %></title>
</head>
<body>
<h1><% include title() %></h1>
<div id="content">
<% include page_content() %>
</div>
</body>
</html>
<% end template %> 
<% end view %>
<% end skin %>
```

`news.tmpl`
```text
<% skin myskin %>
<% view news uses content::news extends master %>
<% template title() %><% include master::title() %> :: News<% end %>
<% template page_content() %>
<% foreach message in news_list %>
    <ul>
    <% item %>
        <li><%= message %></li>
    <% end %>
    </ul>
<% end foreach %>
<% end template %> 
<% end view %>
<% end skin %>
```

* 定义控制器

```cpp
#include <cppcms/application.h>
#include <cppcms/applications_pool.h>
#include <cppcms/service.h>
#include <cppcms/http_response.h>
#include <cppcms/url_dispatcher.h>
#include <iostream>

#include "content.h"

class myapp  : public cppcms::application {
public:
    myapp(cppcms::service &s) :
       cppcms::application(s)
    {
        dispatcher().assign("",&myapp::intro,this);
        mapper().assign("");

        dispatcher().assign("/news",&myapp::news,this);
        mapper().assign("news","/news");

        dispatcher().assign("/page",&myapp::page,this);
        mapper().assign("page","/page");

        mapper().root("/myapp");
    }
    void ini(content::master &c)
    {
        c.title = "My Web Site";
    }
    void intro()
    {
        content::master c;
        ini(c);
        render("intro",c);
    }
    void page()
    {
        content::page c;
        ini(c);
        c.page_title = "About";
        c.page_content = "<p>A page about this web site</p>";
        render("page",c);
    }
    void news()
    {
        content::news c;
        ini(c);
        c.news_list.push_back("This is the latest message!");
        c.news_list.push_back("This is the next message.");
        c.news_list.push_back("This is the last message!");
        render("news",c);
    }
};

int main(int argc,char ** argv)
{
    try {
        cppcms::service srv(argc,argv);
        srv.applications_pool().mount(cppcms::applications_factory<myapp>());
        srv.run();
    }
    catch(std::exception const &e) {
        std::cerr<<e.what()<<std::endl;
    }
}
```

* 编译

```shell
cppcms_tmpl_cc master.tmpl page.tmpl news.tmpl intro.tmpl -o my_skin.cpp
```