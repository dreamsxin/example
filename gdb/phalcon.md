# 调试 phalcon7 扩展

## 通过关键字查找符号表

```shell
nm /usr/lib/php/20160303/phalcon.so |  grep handle
```

得到 `zim_Phalcon_Mvc_Application_handle`

## 启动 gdb

```shell
gdb php -tui
```

## 设置断点

```shell
break zim_Phalcon_Mvc_Application_handle  
```

## 运行程序

```shell
run test.php
```

此时会在方法 `Phalcon\Mvc\Application::handle` 方法处停下，我们可以输入`n`或`next`回车往下执行一行，以后直接回车就是继续上一次输入的操作，所以想继续执行下一行，只要直接回车即可。

```shell
n
```

## 单步执行

仅执行到代码的下一行后再次暂停。

- `step（s）`：单步进入（stepping into）
- `next（n）`：单步越过（stepping over）

next和step都可以采用一个可选的数值参数，来表示要使用next或step执行的额外行数。

```shell
n 5
```

## 打印变量`uri`

```shell
p uri
p (*uri).value.str->len
p (*uri).value.str->val
p (char*)((*uri).value.str->val)
p (*uri).value.str->val[0]
p /c (*uri).value.str->val
p (*uri).value.str->val@6
```

## 打印变量`controller_name`

```shell
p (char*)(controller_name.value.str->val)
```


## 设置新断点

```shell
clear zim_Phalcon_Mvc_Application_handle
break zim_Phalcon_Dispatcher_dispatch
```

## 打印变量`handler_name`

```shell
p (char*)(handler_name.value.str->val)
```