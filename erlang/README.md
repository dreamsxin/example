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

## 声明新类型

使用 `type` 和 `opaque` 关键字，opaque的类型只能在定义的模块内部使用。

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

## 类型及其定义语法

基本的类型语法为一个原子（atom）紧随一对圆括号。

数据类型由一系列Erlang terms组成，其有各种基本数据类型组成(如 integer() , atom() , pid() ）。Erlang预定义数据类型代表属于此类型的所有数据，比如 atom() 代表所有的atom类型的数据。
数据类型，由基本数据类型及其他自定义数据类型组成，其范围为对应数据类型的合集。 比如:

`atom() | 'bar' | integer() | 42` 与 `atom() | integer()` 具有相同的含义。

各种类型之间具有一定的层级关系，其中最顶层的 `any()` 可以代表任何Erlang类型，而最底层的 `none()` 表示空的数据类型。

类型定义不可重名，编译器可以进行检测。

* 预定义的类型及语法

```erl
Type :: any()           %% 最顶层类型，表示任意的Erlang term
     | none()           %% 最底层类型，不包含任何term
     | pid()
     | port()
     | ref()
     | []               %% nil
     | Atom
     | Binary
     | float()
     | Fun
     | Integer
     | List
     | Tuple
     | Union
     | UserDefined      %% described in Section 2

Union :: Type1 | Type2

Atom :: atom()
     | Erlang_Atom      %% 'foo', 'bar', ...

Binary :: binary()                        %% &lt;&lt;_:_ * 8&gt;&gt;
       | &lt;&lt;&gt;&gt;
       | &lt;&lt;_:Erlang_Integer&gt;&gt;            %% Base size
       | &lt;&lt;_:_*Erlang_Integer&gt;&gt;          %% Unit size
       | &lt;&lt;_:Erlang_Integer, _:_*Erlang_Integer&gt;&gt;

Fun :: fun()                             %% 任意函数
    | fun((...) -&gt; Type)                 %% 任意arity, 只定义返回类型
    | fun(() -&gt; Type)
    | fun((TList) -&gt; Type)

Integer :: integer()
        | Erlang_Integer                 %% ..., -1, 0, 1, ... 42 ...
        | Erlang_Integer..Erlang_Integer %% 定义一个整数区间

List :: list(Type)                       %% 格式规范的list (以[]结尾)
     | improper_list(Type1, Type2)       %% Type1=contents, Type2=termination
     | maybe_improper_list(Type1, Type2) %% Type1 and Type2 as above

Tuple :: tuple()                          %% 表示包含任意元素的tuple
      | {}
      | {TList}

TList :: Type
      | Type, TList
```

由于 `lists` 经常使用，我们可以将 `list(T)` 简写为 `[T] `，而 `[T, ...]` 表示一个非空的元素类型为T的规范列表。
`[T]` 可能为空，而 `[T, ...]` 至少包含一个元素。

请注意, `list()`表示任意类型的`list`，其等同于` [_]`或`[any()]`, 而 `[]` 仅仅表示一个单独的类型即空列表。

![内建类型列表](https://github.com/dreamsxin/example/blob/master/erlang/img/types.png?raw=true)

我们也可以使用`record`标记法来表示数据类型:
```erl
Record :: #Erlang_Atom{}
        | #Erlang_Atom{Fields}
```

* 自定义类型定义

`type` 关键字使用如下:
```erl
-type my_type() :: Type.
```
`my_type` 为自定义的类型名称，其必须为`atom`，Type 为已有类型，包括预定义类型。

类型定义也可以参数化，我们可以在括号中包含类型，如同变量定义，参数必须以大写字母开头:
```erl
-type orddict(Key, Val) :: [{Key, Val}].
```

* 伪数据类型 Record

在 Erlang 内部只有两种混合的数据类型：List和Tuple，而这两种都不支持命名访问（可以理解为不能自定义 key）。
为此，Erlang 虚拟机提供了一个伪数据类型，称为 `Record`。

`Record` 使用 `-record` 指令来声明，第一个参数是名称，第二个参数是一个 Tuple，包含了`field`和默认值。
在这里我们定义`server_opts`这个Record，它有三个field：端口、IP和最大连接数：
```erl
-module(my_server). 
 
-record(server_opts, 
  {port, 
  ip="127.0.0.1", 
  max_connections=10}). 
 
% The rest of your code goes here.  
```

* 创建 Record

定义之后，我们来创建 Record，通过`#`符号来创建 ：
```erl
Opts1 = #server_opts{port=80}. 
```
这段代码创建了一个 Record，`port`设置为80，其他`field`使用默认值。

* 访问 Record

如果我想访问`port`这个`field`，我可以这样做：
```erl
Opts = #server_opts{port=80, ip="192.168.0.1"},
Opts#server_opts.port
```
上面的例子可以看出，访问时加上了 `Record` 的名称，为什么要这样？因为`Record`不是真正的内部数据类型，它只是编译器的小把戏。
在内部，`Record` 只是 `Tuple`，上面定义的 `Record` 编译器将名称映射到 `Tuple`里面：
```erl
{server_opts, 80, "127.0.0.1", 10}
```

Erlang 虚拟机记录了`Record`的定义，而编译器将所有的Record逻辑翻译为Tuple逻辑。

* 更新 Record

更新Record和创建Record很类似：
```erl
Opts = #server_opts{port=80, ip="192.168.0.1"}, 
NewOpts = Opts#server_opts{port=7000}. 
```

* 匹配 Record 和 Guard 语句

```erl
handle(Opts=#server_opts{port=8000}) -> 
  % do special port 8080 stuff 
handle(Opts=#server_opts{} -> 
  % default stuff  
```
绑定小于 1024 的端口通常需要 root 权限：
```erl
handle(Opts) when Opts#server_opts.port <= 1024 -> 
  % requires root access 
handle(Opts=#server_opts{}) -> 
  % Doesn't require root access  
```

## 模块和函数

1、Erlang 里代码是用 Module 组织的。一个 Module 包含了一组功能相近的函数。
调用函数的方法：`Module:Function(arg1, arg2)`或者先 `import` 某个 Module 里的函数，然后 `Function(arg1, arg2)` 即可。

2、Module 可也提供代码管理的作用，加载一个 Module 到 Erlang VM就加载了那个 Module 里的所有代码，然后你想热更新代码的话，直接更新这个 Module 就行了。

调用 erlang 自带的Module（BIP），例如 `erlang:element(2,{a,b,c}).` 或 `element(2,{a,b,c}).`

## 函数规范定义

函数规范可以通过新引入的关键字 'spec' 来定义（摒弃了旧的 @spec 声明)。 其语法如下:
```erl
-spec Module:Function(ArgType1, ..., ArgTypeN) -> ReturnType.
```

在同一个module内部，可以简化为:

-spec Function(ArgType1, ..., ArgTypeN) -> ReturnType.

同时，为了便于我们生成文档，我们可以指明参数的名称:
```erl
-spec Function(ArgName1 :: Type1, ..., ArgNameN :: TypeN) -> RT.
```
函数的spec声明可以重载。通过 ';' 来实现:
```erl
-spec foo(pos_integer()) -> pos_integer()
           ; (integer()) -> integer().
```
我们可以通过spec指明函数的输入和输出的某些关系:

-spec id(X) -> X.

但是，对于上面的spec，其对输入输出没有任何限定。我们可以对返回值增加一些类似guard的限定:
```erl
-spec id(X) -> X when is_subtype(X, tuple()).
```
其表示X为一个tuple类型。目前仅仅支持 is_subtype 是唯一支持的guard。

某些情况下，有些函数是server的主循环，或者忽略返回值，仅仅抛出某个异常，我们可以使用 no_return() 作为返回值类型:
```erl
-spec my_error(term()) -> no_return().
my_error(Err) -> erlang:throw({error, Err}).
```

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

## 使用dialyzer进行静态分析

我们定义了type及spec，我们可以使用 dialyzer 对代码进行静态分析，在运行之前发现 很多低级或者隐藏的错误。

* 生成plt

为了分析我们的app或者module，我们可以生成一个plt文件（Persistent Lookup Table）， 其目的是为了加速我们的代码分析过程，plt内部很多类型及函数信息。

首先我们生成一个常用的plt文件, 其包含了以下lib：erts, kernel, stdlib, mnesia, crypto, sasl， ERL_TOP为erlang的安装目录，各个lib因为erlang版本不同会有所差别，我当前使用R13B(erl 5.7.1):

```shell
dialyzer --build_plt -r $ERL_TOP/lib/erts-5.7.1/ebin \
           $ERL_TOP/lib/kernel-2.13.1/ebin \
           $ERL_TOP/lib/stdlib-1.16.1/ebin \
           $ERL_TOP/lib/mnesia-4.4.9/ebin \
           $ERL_TOP/lib/crypto-1.6/ebin \
           $ERL_TOP/lib/sasl-2.1.6/ebin
```
经过十几分钟的的等待，生成了一个~/.dialyzer_plt文件，在生成plt时，可以通过--output_plt 指定生成的plt的名称。

我们也可以随时通过: `dialyzer --add_to_plt --plt ~/.dialyzer_plt -c path_to_app`添加应用到既有plt中，
也可以通过:
`dialyzer --remove_from_plt --plt ~/.dialyzer_plt -c path_to_app` 从已有plt中删除某个应用。

* 分析

生成plt后，就可以对我们书写的应用进行静态检查了。

假设我们书写一个简单的module（spec/spec.erl):

```erl
-module(spec).
-compile([export_all]).
-vsn('0.1').

-spec index(any(), pos_integer(), [any()]) -> non_neg_integer().
index(Key, N, TupleList) ->
   index4(Key, N, TupleList, 0).

index4(_Key, _N, [], _Index) -> 0;
index4(Key, N, [H | _R], Index) when element(N, H) =:= Key -> Index;
index4(Key, N, [_H | R], Index) -> index4(Key, N, R, Index + 1).

% correct:
%-spec fa( non_neg_integer() ) -> pos_integer().
% invalid:
-spec fa( N :: atom() ) -> pos_integer().
fa(0) -> 1;
fa(1) -> 1;
fa(N) -> fa(N-1) + fa(N-2).

-spec some_fun() -> any().
some_fun() ->
   L = [{bar, 23}, {foo, 33}],
   lists:keydelete(1, foo, L).
```

编译spec.erl:
```shell
erlc +debug_info spec.erl
```

使用dialyzer进行分析:
```shell
dialyzer -r ./spec
```
