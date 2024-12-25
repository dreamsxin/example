## go语言中的Scan函数简介

> 变体：`Scan`、`Scanln`、`Scanf`、`Sscanf`

在 Go 语言中，从标准输入（默认是键盘）读取数据可以使用 fmt 包中的 Scan 和 Scanln 函数。

+   `Scan()` ：注意这里是 **`&num`**，如果输入的数据格式不正确，将返回错误。
    
    ```go
    var num int
    fmt.Scan(&num)
    ```
    
+   `Scanln()` ：类似于 Scan，但是只能读取一行数据（将换行符视为终止）
    
    ```go
    var name string
    fmt.Scanln(&name)
    ```
    
+   `Scanf()` ：按照指定格式对输入进行解析。示例程序从标准输入中读取一个整数和一个字符串，并将它们分别存储在变量 num 和 name 中：
    
    ```go
    var num int
    var name string
    fmt.Scanf("%d %s", &num, &name)
    ```
    
+   `Sscanf()` ：String scan，从字符串中读取数据。Sscanf 的用法与 Scanf 类似，只是会从字符串中读取数据而不是标准输入。
    
    ```go
     var num int
     var name string
     fmt.Sscanf("10 foo", "%d %s", &num, &name)
    ```
    

* * *

**注意**：请勿混淆各种输入方式的用法

```go
fmt.Scan("%d", &num)	// 错误的写法
fmt.Scan(&num)				// 正确
```

第一种错误，其使用 `Scan` 获取输入，但是却采用了 `Scanf` 的写法。
