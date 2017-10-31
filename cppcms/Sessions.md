# Sessions

http://cppcms.com/wikipp/en/page/cppcms_1x_sessions

In this example we would save the information about user using session_interface object that can be accessed by session() member function of cppcms::application.
我们将通过 `session_interface` 对象保存信息，可以通过 `cppcms::application::session` 函数访问这些信息。

## 创建 `content.h`

```cpp
#ifndef CONTENT_H
#define CONTENT_H

#include <cppcms/view.h>
#include <cppcms/form.h>

#include <iostream>

namespace content  {
 
struct info_form : public cppcms::form {
    cppcms::widgets::text name;
    cppcms::widgets::radio sex;
    cppcms::widgets::select martial;
    cppcms::widgets::numeric<double> age;
    cppcms::widgets::submit submit;
    info_form()
    {
        name.message("Your Name");
        sex.message("Sex");
        martial.message("Martial State");
        age.message("Your Age");
        submit.value("Send");

        add(name);
        add(sex);
        add(martial);
        add(age);
        add(submit);

        sex.add("Male","m");
        sex.add("Female","f");
        martial.add("Single","s");
        martial.add("Married","m");
        martial.add("Divorced","d");

        name.non_empty();
        age.range(0,120);
    }
    virtual bool validate()
    {
        if(!form::validate()) 
            return false;
        if(martial.selected_id()!="s" && age.value()<18) {
            martial.valid(false);
            return false;
        }
        return true;
    }
};

struct message : public cppcms::base_content {
    std::string name,who;
    double age;
    info_form info;
};

} // content

#endif
```

## 创建模版 `view.tmpl

```text
<% c++ #include "content.h" %>
<% skin view %>
<% view message uses content::message %>
<% template render() %>
<html>
<body>
    <h1>Hello <%= who %> <%= name %></h1>
    <% if (content.age != -1.0) %>
        <p>Your age is <%= age %></p>
        <h2>Change details</h2>
    <% else %>
        <h2>Input your details</h2>
    <% end %>
    <form method="post" action="" >
    <% form as_p info %>
    </form>
  </body>
<html>
<% end template %>
<% end view %>
<% end skin %>
```

## 应用

```cpp
#include <cppcms/application.h>
#include <cppcms/applications_pool.h>
#include <cppcms/service.h>
#include <cppcms/http_response.h>
#include <cppcms/url_dispatcher.h>
#include <cppcms/session_interface.h>
#include <iostream>

#include "content.h"

using namespace std;

class hello: public cppcms::application {
public:
    hello(cppcms::service &s) :
        cppcms::application(s) 
    {
    }
    void main(std::string /* unused */)
    {
        content::message c;
        if(request().request_method()=="POST") {
            c.info.load(context());
            if(c.info.validate()) {
				// 保存数据
                session()["name"]=c.info.name.value(); // // !session()["name"].empty()
                session()["sex"]=c.info.sex.selected_id();
                session()["state"]=c.info.martial.selected_id();
				// 保存非 string 类型数据
                session().set("age",c.info.age.value());
                c.info.clear();
            }
        }

		// 读取数据
        if(session().is_set("name")) {
            c.name=session()["name"];
            if(session()["sex"]=="m") {
                c.who="Mr";
            }
            else {
                if(session()["state"]=="s") {
                    c.who="Miss";
                }
                else {
                    c.who="Mrs";
                }
            }
            c.age=session().get<double>("age");
        }
        else {
            c.name="Visitor";
            c.age=-1;
        }
        render("message",c);
    }
};

int main(int argc,char ** argv)
{
    try {
        cppcms::service app(argc,argv);
        app.applications_pool().mount(cppcms::applications_factory<hello>());
        app.run();
    }
    catch(std::exception const &e) {
        cerr<<e.what()<<endl;
    }
}
```

## 配置后端存储

默认情况下使用 `cookies` 来存储，不能通过 URL（/page/?sid=2e7f60c43b88d4b554a） 传递 `Session ID` 。

存储信息有以下几个方法：
- 将所有信息加密之后存在 cookies 中。
- 将所有信息存在服务端，并通过 Session ID 检索信息。
- 结合上面两者方式: 如果数据量过大不能存储在cookie中，那么它将被存储在服务器端。

例子：
```json
"session" : {
    "expire" : "renew",
    "timeout" : 604800,
    "location" : "client",
    "client" :      {
        "hmac" :        "sha1",
        "hmac_key" :    "3891bbf7f845fd4277008a63d72640fc13bb9a31"
    }    
}
```

* 选项说明

- expire
存活周期设置，可以设置为 `browser` 直到浏览器关闭、`renew` 基于更新的时间（每次刷新都会更新过期时间）、`fixed` 首次创建后的硬限制时间。

- timeout
每种类型都有超时时间，默认的是24小时。

- location
信息存储方式，可以设置为 `client`、`server`、`both`。
此处设置为 `client`，如果设置为 `server`，需要修改配置：

```json
"session" : {
    "expire" : "renew",
    "timeout" : 604800,
    "location" : "server",
    "server" :      {
        "storage" : "memory",
    }    
}
```

- client
设置客户端存储时，使用的加密方式和密钥。

