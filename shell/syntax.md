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