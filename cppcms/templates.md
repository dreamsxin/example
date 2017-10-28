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