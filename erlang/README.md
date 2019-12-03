# erlang

https://cpie-cn.readthedocs.io/en/latest/part-i_chapter-1.html

## 编译安装

```shell
sudo apt-get install erlang
# or
sudo apt-get install build-essential
sudo apt-get install libncurses5-dev m4 libssl-dev
sudo apt-get install unixodbc unixodbc-dev libc6

wget http://erlang.org/download/otp_src_20.3.tar.gz
tar -zxvf otp_src_20.3.tar.gz
cd otp_src_20.3
./configure --without-javac
make
sudo make install
```

## 查看版本

```shell
erl --version
```

ErLang 语法一些约定：
- 大写字母开头的名字（比如Address），表示一个变量，包括参数、局部变量等
- 小写字母开头的单词（比如ok），表示一个常量，叫做atom（原子的意思），包括常量名、函数名、模块名等
- 注释用`%`开头
- 下划线`_`表示任意变量
- 采用 -> 定义函数
- 函数定义结束用一个点号`.`
- {} 表示 Tuple（元组）
- [] 表示 List（列表）
- `=` 匹配原语
特殊符号
顺序执行语句使用逗号","分割

分支语句使用分号";"分割

函数/语句结束使用句点"."分割

注释以百分号"%"开头到该行结尾结束

Erlang中等号"="起的不是简单的赋值的作用,是用来做模式匹配,如果等号左边的变量未赋值的情况下不管等号右边的结果是什么都会合等号左边的变量绑定,这时候才起到赋值的作用

下划线"_"作为通配符

下划线"_"开头的变量即时不使用也不会编译警告

双冒号"::"指定参数或类型取值范围/限定类型

美元符号"$"表示整型变量值(ASCII)或者在erlang:words/2使用时作为分隔符标识

## 类型

下划线 `_` 代表特殊的匿名变量或无所谓变量。在语法要求需要一个变量但又不关心变量的取值时，它可用作占位符。

常量数据类型——无法再被分割为更多原始类型的类型，包括 数值、Atom

* 数值
如：123、-789、3.14159、7.8e12、-1.2e-45。数值可进一步分为整数和浮点数。

* 原子类型 Atom
原子类型是 Erlang 语言中另一种数据类型。所有原子类型都以小写字母开头，如函数名，未使用引号的字符串，如：abc、'An atom with spaces'、monday、green、hello_word。它们都只是一些命名常量。
```erl
-module(math3).
-export([area/1]).

area({square, Side}) ->
    Side * Side;
area({rectangle, X, Y}) ->
    X * Y;
area({circle, Radius}) ->
    3.14159 * Radius * Radius;
area({triangle, A, B, C}) ->
    S = (A + B + C)/2,
    math:sqrt(S*(S-A)*(S-B)*(S-C)).
```
Erlang 中的选择函数的时候是通过模式匹配完成的，上面例子中，如我们所期望的，调用 `math3:area({triangle, 3, 4, 5})`得到6.0000而`math3:area({square, 5})`得到 25 。

复合数据类型——用于组合其他数据类型。

* 元组 Tuple

Erlang 提供了将某些元素分成组并用以更易于理解的方式表示的机制，它就是元组。一个元组由花括号括起来的，如：{a, 12, b}、{}、{1, 2, 3}、{a, b, c, d, e}。
元组用于存储固定数量的元素，并被写作以花括号包围的元素序列。

```erl
Thing = {triangle, 6, 7, 8}.
math3:area(Thing).
```
此处Thing被绑定到`{triangle, 6, 7, 8}`——我们将 Thing 称为尺寸为4的一个元组——它包含 4 个元素。第一个元素是原子式`triangle`，其余三个元素分别是整数6、7和8。

* 列表 list
Erlang 中的列表由方括号括起来表示，如：[]、[a, b, 12]、[22]、[a, 'hello friend']。列表用于存储可变数量的元素，并被写作以方括号包围的元素序列。

* `|` 使用

```shell
2> [First | TheRest]=[1,2,3,4].
[1,2,3,4]
%每间隔一个| 对应一个元素
3> First.
1
4> TheRest.
[2,3,4]
5> 
```

```shell
5> [E1, E2 | R ] = [1,2,3,4,5,6,7]. 
[1,2,3,4,5,6,7]
6> E1.
1
7> E2.
2
8> R.
[3,4,5,6,7]
9> 
```

```shell
10> [A, B| C]= [1, 2].
[1,2]
11> A.
1
12> B.
2
13> C.
[]
%取不到值时会返回一个空的列表
14> 
```
通过 `|` 获得一个列表的长度：
```erl
-module(tut4).
-export([list_length/1]).

list_length([]) ->
    0;
list_length([First | Rest]) ->
    1 + list_length(Rest).
```

* 映射 (Map)

键和值可以是任意的Erlang数据类型。
- 映射组用 `#{}` 表示
- `=>` 用于创建或更新一个映射
- `:=` 只能用来更新映射

```shell
13> Me = #{name =>"zyf",age =>26}.
#{age => 26,name => "zyf"}
14> NewMe = Me#{age => 20}.
#{age => 20,name => "zyf"}
15> Me.
#{age => 26,name => "zyf"}
16> 
16> #{name =>"zyf",age =>26}#{age =>27}.
#{age => 27,name => "zyf"}
17> 
```
只取出一个参数的映射
```shell
1> Me = #{age=> 19,name=>"zyfa"}.
#{age => 19,name => "zyfa"}
2> #{age := NewAge}=Me.
%重新匹配
#{age => 19,name => "zyfa"}
3> NewAge .
19
```

可以使用 maps 模块，查询映射的所有键、所有值、数量等

## 语法

* if / case
```erl
-module(tut9).
-export([test_if/2]).

test_if(A, B) ->
    if
        A == 5 ->
            io:format("A == 5~n",[]),
            a_equals_5;
        B == 6 ->
            io:format("B == 6~n",[]),
            b_equals_6;
        A == 2, B == 3 ->
            io:format("A == 2, B == 3~n",[]),
            a_equals_2_and_b_equals_3;
        A == 1; B == 7 ->
            io:format("A == 1; B== 7~n",[]),
            a_equals_1_or_b_equals_7    
    end.
```
>> 注意，在 end 之前没有 `;`。条件（Condidtion）的工作方式与 guard 一样，即测试并返回成功或者失败。

得到指定某月的天数：
```erl
-module(tut11).
-export([month_length/2]).

month_length(Year,Month) ->
    %% 被 400 整除的为闰年。
    %% 被 100 整除但不能被 400 整除的不是闰年。
    %% 被 4 整除但不能被 100 整除的为闰年。
    Leap = if 
        trunc(Year / 400) * 400 == Year ->
        %Year取整除400 再乘400 等于Year，说明能被400整除
            leap;
        trunc(Year / 100) * 100 == Year ->
            not_leap;
        trunc(Year / 4) * 4 == Year ->
            leap;
        true ->
            not_leap
    end,

    case Month of
        apr -> 30;%四月
        jun -> 30;%六月
        sept -> 30;%九月
        nov -> 30;%十一月
        feb when Leap == leap -> 29;%闰年二月
        feb -> 28;%平年二月
        jan -> 31;%一月
        mar -> 31;%三月
        may -> 31;%五月
        jul -> 31;%七月
        aug -> 31;%八月
        oct -> 31;
        %十月
        dec -> 31
        %十二月
    end.
```

## 模块和函数

1、Erlang 里代码是用 Module 组织的。一个 Module 包含了一组功能相近的函数。
调用函数的方法：`Module:Function(arg1, arg2)`或者先 `import` 某个 Module 里的函数，然后 `Function(arg1, arg2)` 即可。

2、Module 可也提供代码管理的作用，加载一个 Module 到 Erlang VM就加载了那个 Module 里的所有代码，然后你想热更新代码的话，直接更新这个 Module 就行了。

调用 erlang 自带的Module（BIP），例如 `erlang:element(2,{a,b,c}).` 或 `element(2,{a,b,c}).`

## 定义模块和函数

创建 `math1.erl`
```erl
-module(math1).
-export([factorial/1]).

factorial(0) -> 1;
factorial(N) -> N * factorial(N - 1).
```
`-module(math1)`定义了该模块的名称，`-export([factorial/1])`表示本模块向外部导出具备一个参数的函数`factorial`。

进入 `erl` shell 进行编译使用
```shell
% 编译 math1.erl 会生成 math1.beam 文件
c(math1).
math1:factorial(6).
```

我再创建文件 `math2.erl`
```erl
-module(math2).
-export([double/1]).

double(X) ->
    times(X, 2).

times(X, N) ->
    X * N.
```
此模块只向外导出了具备一个参数的函数`double`，如果要导出`times`，只要修改 `export`：
```erl
-export([double/1, times/2]).
```
