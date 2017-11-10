## C++11 的 lambda 表达式规范

有以下几种形式：
(1) [ capture ] ( params ) mutable exception attribute -> ret { body }
(2) [ capture ] ( params ) -> ret { body }
(3) [ capture ] ( params ) { body }
(4) [ capture ] { body }

其中：

(1) 是完整的 `lambda` 表达式形式，
(2) `const` 类型的 `lambda` 表达式，该类型的表达式不能改捕获（capture）列表中的值。
(3) 省略了返回值类型的 `lambda` 表达式，但是该 lambda 表达式的返回类型可以按照下列规则推演出来：
	如果 `lambda` 代码块中包含了 return 语句，则该 lambda 表达式的返回类型由 return 语句的返回类型确定。
	如果没有 `return` 语句，则类似 `void f(...)` 函数。
	省略了参数列表，类似于无参函数 `f()`。

- mutable
修饰符说明 lambda 表达式体内的代码可以修改被捕获的变量，并且可以访问被捕获对象的 non-const 方法。

- exception
说明 lambda 表达式是否抛出异常(noexcept)，以及抛出何种异常，类似于void f() throw(X, Y)。

- attribute
用来声明属性。

另外，capture 指定了在可见域范围内 lambda 表达式的代码内可见得外部变量的列表，具体解释如下：

- [a,&b] a变量以值的方式呗捕获，b以引用的方式被捕获。
- [this] 以值的方式捕获 this 指针。
- [&] 以引用的方式捕获所有的外部自动变量。
- [=] 以值的方式捕获所有的外部自动变量。
- [] 不捕获外部的任何变量。

此外，params 指定 lambda 表达式的参数。