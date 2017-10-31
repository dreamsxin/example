# Form

## 创建 `content.h`

```cpp
#ifndef CONTENT_H
#define CONTENT_H

#include <cppcms/view.h>
#include <cppcms/form.h>

#include <iostream>


namespace content  {

// info_form 表单
struct info_form : public cppcms::form {
    cppcms::widgets::text name;
    cppcms::widgets::radio sex;
    cppcms::widgets::select marital;
    cppcms::widgets::numeric<double> age;
    cppcms::widgets::submit submit;

    info_form()
    {
		// 设置信息
        name.message("Your Name");
        sex.message("Sex");
        marital.message("Marital Status");
        age.message("Your Age");
        submit.value("Send");

		// 注册表单控件
        add(name);
        add(sex);
        add(marital);
        add(age);
        add(submit);

		// 设置控件值，第一个参数为显示文本
        sex.add("Male","male");
        sex.add("Female","female");
        marital.add("Single","single");
        marital.add("Married","married");
        marital.add("Divorced","divorced");

		// 定义字段的一些基本限制
        name.non_empty();
        age.range(0,120);
    }

	// 增加验证
    virtual bool validate()
    {
        if(!form::validate()) 
            return false;
        if(marital.selected_id()!="single" && age.value()<18) {
            marital.valid(false);
            return false;
        }
        return true;
    }
};

struct message : public cppcms::base_content {
    std::string name,state,sex;
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
    <% if not empty name %>
	    <h1>Hello <%= name %></h1>
	    <p>You are <%= sex %>, <%= state %></p>
	    <p>Your age is <%= age %></p>
    <% else %>
    	<h1>Input your details</h1>
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

## 应用控制器

```cpp
#include <cppcms/application.h>
#include <cppcms/applications_pool.h>
#include <cppcms/service.h>
#include <cppcms/http_response.h>
#include <iostream>
#include "content.h"

using namespace std;
class hello: public cppcms::application {
public:
    hello(cppcms::service &s) :
        cppcms::application(s) 
    {
    }

    void main(std::string /*unused*/)
    {
        content::message c;
        if(request().request_method()=="POST") {
            c.info.load(context());
            if(c.info.validate()) {
            	c.name=c.info.name.value();
            	c.sex=c.info.sex.selected_id();
            	c.state=c.info.marital.selected_id();
            	c.age=c.info.age.value();
            	c.info.clear();
            }
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

## 预防跨站点请求伪造（Security Considerations: Cross Site Request Forgery）

预防 CSRF，需要在每个 POST 表单包含一个隐藏字段，保存特定会话的 CSRF 令牌。
所以我们需要增加代码 `<% csrf %>` 在表单中生成 CSRF 隐藏字段。

```text
<form method="post" action="" ><% csrf %>
<% form as_p info %>
</form>
```

在配置中开启 CSRF 检查：
```json
"security" : {
   "csrf" : {
      "enable" : true
   }
}
```