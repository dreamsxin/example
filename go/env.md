go env 用于查看和设置 Go 环境变量。

默认情况下 go env 输出格式为 Shell 脚本格式（如 Windows 上是 batch 文件格式）。如果指定变量名称，则只输出变量的值。

2.格式

go env [-json] [-u] [-w] [var ...]

其中 -json 表示以 JSON 格式输出而不是 Shell 脚本格式。

-u 表示 unset，取消环境变量。

-w 表示 write，设置环境变量。
