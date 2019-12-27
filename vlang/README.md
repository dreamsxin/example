# vlang

* 性能（Performance）

- As fast as C
- C interop without any costs
- Minimal amount of allocations
- Built-in serialization without runtime reflection
- Compiles to native binaries without any dependencies: a simple web server is only 65 KB 

* 安全（Safety）

- No null
- No global variables
- No undefined values
- No undefined behavior
- No variable shadowing
- Bounds checking
- Option/Result types
- Generics
- Immutable variables by default
- Pure functions by default
- Immutable structs by default 

## 编译安装（Installing V from source）

Linux, macOS, Windows, *BSD, Solaris, WSL, Android, Raspbian

```shell
sudo apt install build-essential
git clone https://github.com/vlang/v
cd v
make
# sudo ln -s /vpath/v /usr/local/bin/
sudo ./v symlink
```
在当前目录下生成可执行文件 `v`。

## 更新 v

```shell
v up
```

## 进入命令行模式

```shell
$ v
V 0.1.x
Use Ctrl-C or `exit` to exit

>>> println('hello world')
hello world
>>>
```

可以带上参数 `-g` 或 `-cg` 生成调试信息。
参数 `-cg` 与 `-g` 区别在于显示的行号是编译过程中间 `C` 代码文件 `.tmp.c` 的行号。
参数 `-keep_c` 将保留中间`C` 代码文件 `.tmp.c`

## 运行自带例子

```shell
sudo apt install libglfw3 libglfw3-dev libfreetype6-dev libssl-dev
cd examples
v hello_world.v && ./hello_world    # or simply
v run hello_world.v                 # this builds the program and runs it right away

v word_counter.v && ./word_counter cinderella.txt
v run news_fetcher.v
v run tetris/tetris.v
```

* 调试模式下运行

```shell
v -g run tetris/tetris.v
```

## Hello World

创建文件 `hello_world.v`：
```vlang
fn main() {
	println('hello world')
}
```

* 编译运行

```shell
v run hello_world.v
```

## 注释范例（Comments）

```txt
// This is a single line comment.

/* This is a multiline comment.
   /* It can be nested. */
*/
```

## 函数（Functions）

范例1：
```vlang
fn main() {
	println(add(77, 33))
	println(sub(100, 50))
}

fn add(x int, y int) int {
	return x + y
}

fn sub(x, y int) int {
	return x - y
}
```
参数名之后跟着类型，函数不能重载，可以在声明之前使用，不需要考虑文件和声明的顺序。

范例2：
```vlang
fn foo() (int, int) {
	return 2, 3
}

a, b := foo()
println(a) // 2
println(b) // 3
```

函数可以返回多个值。

* 作用域

函数Functions、常量consts 和 变量默认都是私有未导出的（ private (not exported)）。要允许其他模块能使用它们需要加上前置 `pub`。

范例3：
```vlang
pub fn public_function() {
}

fn private_function() {
}
```

## 变量 Variables

范例2：
```vlang
name := 'Bob'
age := 20
large_number := i64(9999999999)
println(name)
println(age)
println(large_number)
```

## 调用 C 函数（Calling C functions from V）

`call_c_functions.v` 代码如下：
```vlang
#flag -lsqlite3
#include "sqlite3.h"

struct C.sqlite3
struct C.sqlite3_stmt

fn C.sqlite3_column_text(voidptr, int) byteptr
fn C.sqlite3_column_int(voidptr, int) int
fn C.sqlite3_open()
fn C.sqlite3_step() int
fn C.sqlite3_prepare_v2()
fn C.sqlite3_finalize()

fn main() {
        path := 'users.db'
        db := &C.sqlite3(0) // a temporary hack meaning `sqlite3* db = 0`
        C.sqlite3_open(path.str, &db)

        query := 'select count(*) from users'
        stmt := &C.sqlite3_stmt(0)
        C.sqlite3_prepare_v2(db, query.str, - 1, &stmt, 0)
        C.sqlite3_step(stmt)
        nr_users := C.sqlite3_column_int(stmt, 0)
        C.sqlite3_finalize(stmt)
        println(nr_users)
}
```
在文件最上面增加 `#flag` 指令提供 `C` 编译指令，比如链接 `-l`、头文件路径 `-I`、编译时定义 `-D`等。

可以给不同目标环境设置不同指令（目前支持linux , darwin , and windows）：
```vlang
#flag linux -lsdl2
#flag linux -Ivig
#flag linux -DCIMGUI_DEFINE_ENUMS_AND_STRUCTS=1
#flag linux -DIMGUI_DISABLE_OBSOLETE_FUNCTIONS=1
#flag linux -DIMGUI_IMPL_API=
```

显示 C 编译命令：
```shell
v -show_c_cmd call_c_functions.v
```
out:
```txt
==========
cc  -std=gnu11 -Wall -Wextra -Wno-unused-variable -Wno-unused-parameter -Wno-unused-result -Wno-unused-function -Wno-missing-braces -Wno-unused-label -Werror=implicit-function-declaration -o "call_c_functions" "/tmp/v/call_c_functions.tmp.c"  -lsqlite3  -lm -lpthread   -ldl 
cc took 238 ms
=========
```

## 编译时 `if`（Compile time if）

类似 C 里面预定义宏的判断。

```vlang
$if windows {
	println('Windows')
}
$if linux {
	println('Linux')
}
$if mac {
	println('macOS')
}

$if debug {
	println('debugging')
}
```

## 将 C 代码转换成 V（Translating C/C++ to V）

TODO: translating C to V will be available in V 0.3. C++ to V will be available later this year. 

```shell
v translate test.cpp
```

`test.cpp` 内容如下：
```cpp
#include <vector>
#include <string>
#include <iostream>

int main() {
        std::vector<std::string> s;
        s.push_back("V is ");
        s.push_back("awesome");
        std::cout << s.size() << std::endl;
        return 0;
}
```
生成文件`test.v`：
```vlang
fn main {
        mut s := []
	s << 'V is '
	s << 'awesome'
	println(s.len)
}
```