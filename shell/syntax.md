# 变量说明

- `$$`
Shell本身的PID（ProcessID）

- `$!`
Shell最后运行的后台Process的PID

- `$?`
最后运行的命令的结束代码（返回值）

- `$-`
使用Set命令设定的Flag一览

- `$*`
所有参数列表。如"$*"用「"」括起来的情况、以"$1 $2 … $n"的形式输出所有参数。

- `$@`
所有参数列表。如"$@"用「"」括起来的情况、以"$1" "$2" … "$n" 的形式输出所有参数。

- `$#`
添加到Shell的参数个数

- `$0`
Shell本身的文件名

- `$1～$n`
添加到Shell的各参数值。$1是第1参数、$2是第2参数…。

# 模式匹配替换

* 第一种模式：${variable%pattern}。shell在variable中查找，看它是否一给的模式pattern结尾，如果是，把variable去掉右边最短的匹配模式
* 第二种模式： ${variable%%pattern}，这种模式时，shell在variable中查找，看它是否一给的模式pattern结尾，如果是，把variable中去掉右边最长的匹配模式
* 第三种模式：${variable#pattern} 这种模式时，shell在variable中查找，看它是否一给的模式pattern开始，如果是，把variable中去掉左边最短的匹配模式
* 第四种模式： ${variable##pattern} 这种模式时，shell在variable中查找，看它是否一给的模式pattern结尾，如果是，把variable中去掉左边最长的匹配模式

这四种模式中都不会改变variable的值，其中，只有在pattern中使用了*匹配符号时，%和%%，#和##才有区别。结构中的pattern支持通配符，*表示零个或多个任意字符，?表示仅与一个任意字符匹配，[...]表示匹配中括号里面的字符，[!...]表示不匹配中括号里面的字符。

# 字符串提取和替换

* 第一种模式：${var:num}，shell在var中提取第num个字符到末尾的所有字符。若num为正数，从左边0处开始；若num为负数，从右边开始提取字串，但必须使用在冒号后面加空格或一个数字或整个num加上括号，如${var: -2}、${var:1-3}或${var:(-2)}。         
* 第二种模式：${var:num1:num2}，num1是位置，num2是长度。表示从$var字符串的第$num1个位置开始提取长度为$num2的子串。不能为负数。
* 第三种模式：${var/pattern/pattern}表示将var字符串的第一个匹配的pattern替换为另一个pattern。
* 第四种模式：${var//pattern/pattern}表示将var字符串中的所有能匹配的pattern替换为另一个pattern。

# 井号使用

`${num#remove}`

用以删除匹配到的字符。

在`#`前面的数字代表着第几个参数（0号参数是脚本本身的名字），`#`后面的匹配表达式。

当用两个 `##` 属于贪婪匹配。

`${var#pattern}`

从最左边开始匹配。

`${var##pattern}`

最长匹配

# 问号使用

`${num?match}`

跟井号使用相反，用以获取匹配到的字符。

当用两个 `??` 属于贪婪匹配。

# 百分号使用

`${var%pattern}`

从最右边(即结尾)匹配。

`${var%%pattern}`

最长匹配

# set -- 使用

默认使用空格分割参数，也可以自定义分隔符

```shell
oIFS=$IFS	// 保存系统默认分隔符
IFS=**		// 设置分割符以便set -- 处理字符串使用
IFS=$oFIS	// 还原系统默认分隔符
```

重新设置参数位置，在原来的参数列表中加入 `my` 这个参数：

```shell
 set -- my "$@"
 ```

# 来自 manaphp 关于 bash_completion 例子
```shell
_manacli(){
   COMPREPLY=( $(./manacli.php bash_completion complete $COMP_CWORD "${COMP_WORDS[@]}") )
   return 0;
}
complete -F _manacli manacli
```

# cut

cut 命令可以从一个文本文件或者文本流中提取文本列。


选项与参数：
- -d  ：后面接分隔字符。与 -f 一起使用；
- -f  ：依据 -d 的分隔字符将一段信息分割成为数段，用 -f 取出第几段的意思；
- -c  ：以字符 (characters) 的单位取出固定字符区间；

将 PATH 变量取出，我要找出第三到最后一个路径。
```shell
echo $PATH | cut -d ':' -f 3-
```

# 截取
```shell
echo 'a_b_c'|cut -d '_' -f 2-		// b_c
var='a_b_c' &&  echo ${var#*_}		// b_c
var='a_b_c' &&  echo ${var%%_*}		// a
```