## 内置函数和自定义函数关于点"."和作用域
在写 template 的时候，会经常用到"."。比如`{{.}}`、`{{len .}}`、`{{.Name}}`、`{{$x.Name}}`等等。

在 template 中，点"."代表当前作用域的当前对象。它类似于`java/c++`的`this`关键字，类似于`perl/python的self`。如果了解 perl，它更可以简单地理解为默认变量 `$_`。

## 去除空白
template 引擎在进行替换的时候，是完全按照文本格式进行替换的。除了需要评估和替换的地方，所有的行分隔符、空格等等空白都原样保留。所以，对于要解析的内容，不要随意缩进、随意换行。

可以在`{{`符号的后面加上短横线并保留一个或多个空格"- "来去除它前面的空白(包括换行符、制表符、空格等)，即`{{- xxxx`。

在`}}`的前面加上一个或多个空格以及一个短横线"-"来去除它后面的空白，即`xxxx -}}`。

## 注释
注释方式：`{{/* a comment */}}`。

注释后的内容不会被引擎进行替换。但需要注意，注释行在替换的时候也会占用行，所以应该去除前缀和后缀空白，否则会多一空行。

## 管道pipeline
pipeline 是指产生数据的操作。比如`{{.}}`、`{{.Name}}`、`funcname args`等。

可以使用管道符号|链接多个命令，用法和unix下的管道类似：`|`前面的命令将运算结果(或返回值)传递给后一个命令的最后一个位置。
```tmpl
{{`"output"`}}
{{printf "%q" "output"}}
{{"output" | printf "%q"}}
{{printf "%q" (print "out" "put")}}
{{"put" | printf "%s%s" "out" | printf "%q"}}
{{"output" | printf "%s" | printf "%q"}}
```

## 变量

变量有作用域，只要出现 `end`，则当前层次的作用域结束。内层可以访问外层变量，但外层不能访问内层变量。
有一个特殊变量$，它代表模板的最顶级作用域对象(通俗地理解，是以模板为全局作用域的全局变量)，在 `Execute()` 执行的时候进行赋值，且一直不变。
变量不可在模板之间继承。

可以在 template 中定义变量：
```tmpl
// 未定义过的变量
$var := pipeline

// 已定义过的变量
$var = pipeline

{{- $how_long :=(len "output")}}
{{- println $how_long}}   // 输出6
```
**变量的传递**
```tmpl
func main() {
    t1 := template.New("test1")
    tmpl, _ := t1.Parse(
`
{{- define "T1"}}ONE {{println .}}{{end}}
{{- define "T2"}}{{template "T1" $}}{{end}}
{{- template "T2" . -}}
`)
    _ = tmpl.Execute(os.Stdout, "hello world")
}

上面使用define额外定义了T1和T2两个模板，T2中嵌套了T1。`{{template "T2" .}}` 的点代表顶级作用域的"hello world"对象。
在 T2 中使用了特殊变量`$`，这个`$`的范围是`T2`的，执行 T2 的时候，传递的是 "."，所以这里的 $ 的值仍然是"hello world"。

实际上，template可以看作是一个函数，它的执行过程是`template("T2", .)`。如果把上面的 `$` 换成"."，结果是一样的。如果换成`{{template "T2"}}`，则 `$=nil`

## with...end
with 用来设置"."的值。两种格式：
```tmpl
{{with pipeline}} T1 {{end}}
{{with pipeline}} T1 {{else}} T0 {{end}}
```
对于第一种格式，当pipeline不为0值的时候，点"."设置为pipeline运算的值，否则跳过。对于第二种格式，当pipeline为0值时，执行else语句块，否则"."设置为pipeline运算的值，并执行T1。

例如：
```tmpl
{{with "xx"}}{{println .}}{{end}}
```
上面将输出xx，因为"."已经设置为"xx"。

## 内置函数

- and
    返回第一个为空的参数或最后一个参数。可以有任意多个参数。
    and x y等价于if x then y else x

- not
    布尔取反。只能一个参数。

- or
    返回第一个不为空的参数或最后一个参数。可以有任意多个参数。
    "or x y"等价于"if x then x else y"。

- print
- printf
- println
    分别等价于fmt包中的Sprint、Sprintf、Sprintln

- len
    返回参数的length。

- index
    对可索引对象进行索引取值。第一个参数是索引对象，后面的参数是索引位。
    "index x 1 2 3"代表的是x[][][]。
    可索引对象包括map、slice、array。

- call
    显式调用函数。第一个参数必须是函数类型，且不是template中的函数，而是外部函数。
    例如一个struct中的某个字段是func类型的。
    "call .X.Y 1 2"表示调用dot.X.Y(, )，Y必须是func类型，函数参数是和。
    函数必须只能有一个或个返回值，如果有第二个返回值，则必须为error类型。

- eq arg1 arg2：
    arg1 == arg2时为true
  支持多个参数：`eq arg1 arg2 arg3 arg4...` 它等价于：arg1==arg2 || arg1==arg3 || arg1==arg4 
- ne arg1 arg2：
    arg1 != arg2时为true
- lt arg1 arg2：
    arg1 < arg2时为true
- le arg1 arg2：
    arg1 <= arg2时为true
- gt arg1 arg2：
    arg1 > arg2时为true
- ge arg1 arg2：
    arg1 >= arg2时为true

## define 和 template
define 可以直接在待解析内容中定义一个模板，这个模板会加入到 common 结构组中，并关联到关联名称上。

```tmpl
{{template "name"}} //直接执行名为name的template，点设置为nil
{{template "name" pipeline}} //点"."设置为pipeline的值，并执行名为name的template。
```

## block
block 的第一个动作是找到名为 name 的模板，如果不存在，则在此处自动定义这个模板，并执行这个临时定义的模板。换句话说，block可以认为是设置一个默认模板。
例如：
```tmpl
{{block "T1" .}} one {{end}}
```

它首先表示 `{{template "T1" .}}`，也就是说先找到T1模板，如果 T1 存在，则执行找到的 T1，如果没找到 T1，则临时定义一个`{{define "T1"}} one {{end}}`，并执行它。
