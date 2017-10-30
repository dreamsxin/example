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