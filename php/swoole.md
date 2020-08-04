# Swoole

## swoole_runtime.cc

```c
bool PHPCoroutine::enable_hook(int flags);

static php_stream *socket_create(
    const char *proto, size_t protolen, const char *resourcename, size_t resourcenamelen,
    const char *persistent_id, int options, int flags, struct timeval *timeout, php_stream_context *context
    STREAMS_DC
)

static inline int socket_connect(php_stream *stream, Socket *sock, php_stream_xport_param *xparam);
```

## 使用了虚拟机中断的方法来切换一直占用CPU的协程

好的，我们现在来感受一下虚拟机中断的用法。

首先，我们实现一个扩展函数`start_interrupt`，它的作用是开启对虚拟机中断的使用：
```c
PHP_FUNCTION(start_interrupt) {
  init();
  create_scheduler_thread();
};
```
init的实现如下：
```c
void init()
{
  orig_interrupt_function = zend_interrupt_function;
  zend_interrupt_function = new_interrupt_function;
}
```
这里的核心是

`zend_interrupt_function = new_interrupt_function;`

也就是说，我们需要实现zend_interrupt_function。zend_interrupt_function的作用是在虚拟机中断发生的时候，会去执行的函数。并且，zend_interrupt_function是PHP内核定义的一个函数指针。

new_interrupt_function的定义如下：
```c
static void new_interrupt_function(zend_execute_data *execute_data)
{
    php_printf("yield coroutine\n");
    if (orig_interrupt_function)
    {
        orig_interrupt_function(execute_data);
    }
}
```
可以看出，我们的这个函数'模拟'了yield协程的过程。OK，我们接着看create_scheduler_thread：
```c
static void create_scheduler_thread()
{
    pthread_t pidt;
​
    if (pthread_create(&pidt, NULL, (void * (*)(void *)) schedule, NULL) < 0)
    {
        php_printf("pthread_create[PHPCoroutine Scheduler] failed");
    }
}
```
这个函数的作用是创建一个线程，这个线程的执行体是schedule函数：
```c
void schedule()
{
    while (1)
    {
        EG(vm_interrupt) = 1;
        usleep(5000);
    }
}
``
而这个schedule线程我们可以认为它是一个负责调用的一个线程。它设置EG(vm_interrupt)的值为1。设置完之后，当虚拟机检查到这个值为1的时候，就会去执行new_interrupt_function函数，从而实现了yield协程。

每次触发完虚拟机中断后，虚拟机会把EG(vm_interrupt)设置为0。因此，这里需要循环的去设置EG(vm_interrupt)的值为1。为什么这里需要使用usleep呢？因为中断不是每分每秒都在进行的，所以可以挂起这个线程，让其他线程跑。
