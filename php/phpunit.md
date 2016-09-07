# phpunit

```shell
wget https://phar.phpunit.de/phpunit.phar
php phpunit.char unit-tests/
```

执行 phpunit 命令，后面可跟具体的目录或文件，也可不跟，如果没有则会对当前目录的所有文件执行单元测试，输出一个字符来指示进展：

- . 当测试成功时输出
- F 当测试断言失败时输出，例如一个失败的assertEquals()调用
- E 当测试产生一个错误时输出，错误是指意料之外的异常（exception）或者PHP错误
- R 当测试被标记有风险时输出
- S 当测试跳出时输出
- I 当测试被标记不完整或为实现时输出

## 常用参数


- --debug

    Output debug information such as the name of a test when its execution starts.
    输出调试信息如测试的名称及该测试什么时候开始执行

- --stop-on-error

    Stop execution upon first error.
    在遇到第一个错误时停止执行
    
- --stop-on-failure

    Stop execution upon first error or failure.
    在遇到第一个失败时停止执行
    
- --stop-on-skipped

    Stop execution upon first skipped test.
    在遇到第一个跳过的测试时停止执行


- --stop-on-incomplete

    Stop execution upon first incomplete test.
    在遇到第一个未完成的测试时停止执行


- --strict

    Mark a test as incomplete if no assertions are made.
    当一个测试没有定义任何断言时将其标记为未完成的测试


- --verbose

    Output more verbose information, for instance the names of tests that were incomplete or have been skipped.
    输出例如未完成的测试的名字，跳过的测试的名字