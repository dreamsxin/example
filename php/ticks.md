# declare(ticks=N) 的作用

## 作用一：Zend 引擎每执行N条低级语句就去执行一次 `register_tick_function` 注册的函数：

```php
declare(ticks=1);

$time_start = time();
function check_timeout(){
    // 开始时间
    global $time_start;
    if(time()-$time_start > 5){
        exit("超时 {$timeout} 秒\n");
    }
}

register_tick_function('check_timeout');

// 模拟一段耗时的业务逻辑，虽然是死循环，但是执行时间不会超过$timeout=5秒
while(1){
   $num = 1;
}
```

## 作用二：每执行N条低级语句会检查一次该进程是否有未处理过的信号：

```php
// 使用 CTL + C 或者 运行命令`kill -SIGINT PID`会去运行`pcntl_signal`注册的函数
declare(ticks=1);
pcntl_signal(SIGINT, function(){
   exit("Get signal SIGINT and exit\n");
});

echo "Ctl + c or run cmd : kill -SIGINT " . posix_getpid(). "\n" ;

while (1) {
  $num = 1;
}
```

为函数执行设置超时：
```php
// 为进程设置一个 alarm 闹钟信号，接收处理闹钟信号`SIGALRM`。
declare(ticks = 1);
function sig() {
    throw new Exception;
}

function a() {
    sleep(10);
    echo "a finishi\n";
}

function b() {
    echo "Stop\n";
}

function c() {
    usleep(100000);
}

try {
	// 1秒后发送一个`SIGALRM`信号。
    pcntl_alarm(1);
    pcntl_signal(SIGALRM, "sig");
    a();
    pcntl_alarm(0);
} catch(Exception $e) {
    echo "timeout\n";
}

b();
a();
b();
```