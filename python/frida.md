#

- https://github.com/frida/frida

Frida 允许在运行时对即将执行的操作进行操作。
比如，当我们从一个简单的 C++ 程序开始，该程序使用一个函数将两个值相加并返回结果。我们想要操作的函数声明为 `Add(int,int)`。首先，我们将更改其中一个 `int` 参数，然后，更改返回的结果。

