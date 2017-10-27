# 模版 templates

为了使用模板，我们需要将源代码分成三部分：

1. 内容 content
content 对象中填充了应该呈现的数据。

2. 应用 application
application 产生需要填充的内容。

3. 渲染内容

## 内容 content

在 `content.h` 文件中，声明 `content`：

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