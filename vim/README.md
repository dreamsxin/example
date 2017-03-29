# VI 常用命令

## 移动

- shift + 0 是行首
- shift + $ 是行尾

## 二进制方式打开文件

有些文件字符不可见，使用二进制方式打开可以看到16进制的值。

```shell
vi -b filepath
```

## 转换为16进制编辑

在命令模式下输入如下命令，将会显示 hex 字符串：
```shell
:%!xxd
```

编辑完之后，需要转换回来
```shell
:%!xxd -r
```

## 去掉最后的行终止符(line terminator)

```shell
:set noendofline binary
:w
```
