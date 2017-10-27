# 使用 Gdb 调试 Python

https://wiki.python.org/moin/DebuggingWithGdb

## 安装

```shell
sudo apt-get install gdb python2.7-dbg
sudo echo 0 > /proc/sys/kernel/yama/ptrace_scope
```

## 实例

假设一个服务器上运行了下面这样的 `test.py` 程序：
```python
import time

def do(x):
    time.sleep(10)

def main():
    for x in range(10000):
        do(x)

if __name__ == '__main__':
    main()
```

这个程序既没有日志也没有 `print` 输出，无法通过日志和查看标准输出确认程序状况的。

我们这里使用 gdb 来查看程序当前的运行状况。

* 运行 `test.py` 并

```shell
python test.py &
```

使用 `gdb` 利用上面返回的进程号来调试运行中的进程：
```shell
gdb python pid
```

* 生成 core file

为了不影响运行中的进程，可以通过生成 core file 的方式来保存进程的当前信息。
```text
(gdb) generate-core-file
(gdb) quit
```

读取 core file：
```shell
gdb python core.PID
```

### 可用的 python 相关的命令

可以通过输入 py 然后加 tab 键的方式来查看可用的命令：
```text
(gdb) py
py-bt               py-down             py-locals           py-up               python-interactive
py-bt-full          py-list             py-print            python
```
可以通过 help cmd 查看各个命令的说明:
```text
(gdb) help py-bt
Display the current python frame and all the frames within its call stack (if any)
```

### 当前执行位置的源码

```text
(gdb) py-list
   1    # -*- coding: utf-8 -*-
   2    import time
   3
   4
   5    def do(x):
  >6        time.sleep(10)
   7
   8
   9    def main():
  10        for x in range(10000):
  11            do(x)
(gdb)
```
可以看到当前正在执行 `time.sleep(10)`


### 当前位置的调用栈

```text
(gdb) py-bt
Traceback (most recent call first):
  <built-in function sleep>
  File "test.py", line 6, in do
    time.sleep(10)
  File "test.py", line 11, in main
    do(x)
  File "test.py", line 15, in <module>
    main()
(gdb)
```

可以看出来是 `main() -> do(x) -> time.sleep(10)`

### 查看变量的值

```text
(gdb) py-list
   1    # -*- coding: utf-8 -*-
   2    import time
   3
   4
   5    def do(x):
  >6        time.sleep(10)
   7
   8
   9    def main():
  10        for x in range(10000):
  11            do(x)
(gdb) py-print x
local 'x' = 12
(gdb)
(gdb) py-locals
x = 12
```

### 查看上层调用方的信息

```text
(gdb) py-up
#9 Frame 0xb74c0994, for file test.py, line 11, in main (x=12)
    do(x)
(gdb) py-list
   6        time.sleep(10)
   7
   8
   9    def main():
  10        for x in range(10000):
 >11            do(x)
  12
  13
  14    if __name__ == '__main__':
  15        main()
(gdb) py-print x
local 'x' = 12
```

### 可以通过 py-down 回去:

```text
(gdb) py-down
#6 Frame 0xb74926e4, for file test.py, line 6, in do (x=12)
    time.sleep(10)
(gdb) py-list
   1    # -*- coding: utf-8 -*-
   2    import time
   3
   4
   5    def do(x):
  >6        time.sleep(10)
   7
   8
   9    def main():
  10        for x in range(10000):
  11            do(x)
(gdb)
```

## 调试多线程程序

测试程序 `test2.py`：
```python
# -*- coding: utf-8 -*-
from threading import Thread
import time


def do(x):
    x = x * 3
    time.sleep(x * 60)


def main():
    threads = []
    for x in range(1, 3):
        t = Thread(target=do, args=(x,))
        t.start()
    for x in threads:
        x.join()


if __name__ == '__main__':
    main()
```

* 运行
```shell
$ python test2.py &
[2] 12281
```

### 查看所有线程

```text

$ gdb python core.12281

(gdb) info threads
  Id   Target Id         Frame
* 1    Thread 0xb74b9700 (LWP 11039) 0xb7711c31 in __kernel_vsyscall ()
  2    Thread 0xb73b8b40 (LWP 11040) 0xb7711c31 in __kernel_vsyscall ()
  3    Thread 0xb69ffb40 (LWP 11041) 0xb7711c31 in __kernel_vsyscall ()
(gdb)
```

可以看到这个程序当前有 3 个线程, 当前进入的是 1 号线程。

### 切换线程

```text
(gdb) thread 3
[Switching to thread 3 (Thread 0xb69ffb40 (LWP 11041))]
#0  0xb7711c31 in __kernel_vsyscall ()
(gdb) info threads
  Id   Target Id         Frame
  1    Thread 0xb74b9700 (LWP 11039) 0xb7711c31 in __kernel_vsyscall ()
  2    Thread 0xb73b8b40 (LWP 11040) 0xb7711c31 in __kernel_vsyscall ()
* 3    Thread 0xb69ffb40 (LWP 11041) 0xb7711c31 in __kernel_vsyscall ()
(gdb)
```
现在切换到了 3 号线程。


### 同时操作所有线程

`thread apply all CMD` 或 `t a a CMD`

```text
(gdb) thread apply all py-list

Thread 3 (Thread 0xb69ffb40 (LWP 11041)):
   3    import time
   4
   5
   6    def do(x):
   7        x = x * 3
  >8        time.sleep(x * 60)
   9
  10
  11    def main():
  12        threads = []
  13        for x in range(1, 3):

Thread 2 (Thread 0xb73b8b40 (LWP 11040)):
   3    import time
   4
   5
   6    def do(x):
   7        x = x * 3
  >8        time.sleep(x * 60)
   9
  10
  11    def main():
  12        threads = []
  13        for x in range(1, 3):

---Type <return> to continue, or q <return> to quit---
Thread 1 (Thread 0xb74b9700 (LWP 11039)):
 335            waiter.acquire()
 336            self.__waiters.append(waiter)
 337            saved_state = self._release_save()
 338            try:    # restore state no matter what (e.g., KeyboardInterrupt)
 339                if timeout is None:
>340                    waiter.acquire()
 341                    if __debug__:
 342                        self._note("%s.wait(): got it", self)
 343                else:
 344                    # Balancing act:  We can't afford a pure busy loop, so we
 345                    # have to sleep; but if we sleep the whole timeout time,
(gdb)
```
常用的 gdb python 相关的操作就是这些， 同时也不要忘记原来的 gdb 命令都是可以使用的哦。

