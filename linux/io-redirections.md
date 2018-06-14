文件描述符： 0 通常是标准输入（STDIN），1 是标准输出（STDOUT），2 是标准错误输出（STDERR）。

- `>` 表示覆盖
- `>>` 表示追加
- `>&` 合并

错误命令（2）重定向中 `>` 或和 `>>` 左右两边不能有空格

```shell
if [[ $(ls -l /test 2>/dev/null) > /dev/null ]]; then
    echo OK
else
    echo FAIL
fi
```

```shell
ret=$(ls /test > /dev/null 2>&1)
if [ $? -eq 0 ]; then
    echo OK
else
    echo FAIL
fi
```