# jq

一个轻量级的命令行工具，专门用于处理和查询JSON数据。它具有简洁、灵活和强大的特性，使得你可以轻松地从JSON数据中提取、转换和格式化信息。

## 基本语法

`jq [options] filter [file...]`

## 选项（Options）

jq命令可以接受一些可选参数（Options），以改变其行为和输出结果的格式。以下是一些常用的选项：

- -c：将输出结果以紧凑的格式输出，取消掉换行。
- -r：将输出结果以原始格式输出，取消掉JSON字符串的引号。
- -s：将输入合并成一个数组，用于处理包含多个JSON对象的文件。
- -M：禁用内存优化模式，适用于处理较大的JSON文件。

## 过滤器

过滤器的语法非常灵活，可以根据需求编写复杂的脚本。以下是一些常用的过滤器操作符：

- .：表示当前对象。可以使用.来访问对象的属性。
- []：表示索引或迭代。可以使用[]来访问数组元素或迭代JSON对象属性。
- |：表示管道操作。可以使用|将多个过滤器连接在一起，实现逐步处理数据。
- select()：用于根据条件筛选元素。
- map()：用于对数组中的每个元素应用同一个转换。