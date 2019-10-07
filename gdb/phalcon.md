# 调试 phalcon7 扩展

## 通过关键字查找符号表

```shell
nm /usr/lib/php/20160303/phalcon.so |  grep handle
```

得到 `zim_Phalcon_Mvc_Application_handle`

## 禁用Zend MM

```shell
ulimit -c unlimited
export USE_ZEND_ALLOC=0
export ZEND_DONT_UNLOAD_MODULES=1
```
## 启动 gdb

```shell
gdb php -tui
```

### 执行

```shell
gdb --args php -d extension=phalcon.so manual.php <<EOF
run
bt full
quit
EOF
```

## 设置断点

```shell
break zim_Phalcon_Mvc_Application_handle
break zim_Phalcon_Mvc_Router_handle
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
p (char*)((real_uri).value.str->val)

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

## 相关命令

与调试控制相关的命令

- continue	继续运行程序直到下一个断点（类似于VS里的F5）
- next		逐过程步进，不会进入子函数（类似VS里的F10）
- step		逐语句步进，会进入子函数（类似VS里的F11）
- until		运行至当前语句块结束
- finish	运行至函数结束并跳出，并打印函数的返回值（类似VS的Shift+F11）

- s:		step in
- fin:		step out 跳出函数
- until 行号:	可用于跳出循环，加快了调试速度。

在 php 官方源码根目录中，存在`gdb`脚本文件 `.gdbinit`，我可以在`gdb`启动后引入

```shell
source .gdbinit
```

`gdb`在启动的时候，会在当前目录下查找 ".gdbinit" 文件，所以也可以把文件拷贝到当前目录。

里面定义的快捷的命令有：

- zbacktrace
- print_ht
- print_htptr
- print_htstr
- print_ft
- print_inh
- printzn
- printzops
- lookup_root
- zmemcheck

## 调试 core

```shell
gdb php core

# 查看堆栈
gdb> bt
# 跳到指定的 frame
gdb> f 1
# 打印当前 state
gdb> p execute_data->function_state.function->common->function_name
gdb> p execute_data->function_state.function->op_array->filename
gdb> p execute_data->function_state.function->op_array->line_start
# 或者
gdb> zbacktrace
gdb> print execute_data.This
