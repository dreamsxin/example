## 二进制文件

```shell
grep -a bin.log
```

## 正则
搜索指定格式数据 `[100.00ms] xxxx`
```shell
grep -P "\[[0-9]{3,}\.[0-9]+ms\].*" sql.log
```

### 只输出匹配部分

```shell
grep -oP '(?<=FROM )\w+' sql.log
```
