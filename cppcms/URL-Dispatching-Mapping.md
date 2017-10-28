# URL Dispatching and Mapping

我们将学习如何轻松地将不同 URL 连接到它们的相关函数。

当客户端向服务端发送一个特定的 URL 请求时，被分为若干部分（CGI variables），例如 `/foo/bar.php/test?x=10`：

- SCRIPT_NAME = /foo/bar.php
- PATH_INFO = /test
- QUERY_STRING = x=10

CppCMS 应用中没有分为多个脚本，但是相当于单一的 FastCGI 应用。例如 `/myapp/page/10`，其中 `script_name = MyApp` 保持不变，`path_info = /page/10` 是根据用户的需要改变。

所以 URL 和 应用成员函数之间的映射是通过正则表达式匹配 `URL` 中的 `path_info` 部分完成的，这种行为可以通过使用 `mount_point` 类修改。

这项工作是由 `cppcms::url_dispatcher` 类完成。