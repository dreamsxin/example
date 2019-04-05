# Boost.Context

https://github.com/boostorg/context

* stackfull 协程

每个协程有完整的私有堆栈，里面的核心就是上下文切换(context)，可以从嵌套的 `stackframe` 中暂停执行，在恢复的时候可以在其暂停的地方继续执行。

例如：boost, libtask, libmill, libco（微信）, libgo（魅族）、libcopp

上下文切换方法：

- 使用 `ucontext` 系列接口，例如：libtask
- 使用 `setjmp/longjmp`接口，例如：libmill
- 使用 `boost.context`，纯汇编实现，内部实现机制跟ucontext完全不同，效率非常高，后面会细讲，tbox最后也是基于此实现
- 使用 windows 的`GetThreadContext/SetThreadContext` 接口
- 使用 windows 的` CreateFiber/ConvertThreadToFiber/SwitchToFiber` 接口

* stackless 协程

调用栈复用，栈内信息保存在堆上，只有顶层的函数(top-level routine)可以被暂停，所有顶层函数调用的函数都不允许被暂停，也就是不允许嵌套使用携程。

例如：protothreads

* stackfull 与 stackless 比较

stackless 优点性能好
stackfull 优点易用，缺点空间大，通过预分配一定大小的栈，检查是否超出来扩容，采取一定的收缩策略。

## Boost.Coroutine

在 `C++` 中有函数对象的概念后，只要类提供`operator()`的接口，那么类对象就可以当作函数调用，同时类的其他成员可以保存相关的状态信息。
`stackless` 就是通过 `coroutine` 这个类本身来实现当前协程的状态保护的，其内部就是用的一个`int`来保留下次resume的行号的，同时提供`is_child()`、`is_parent()`、`is_complete()`三个函数来辅助控制协程的行为。
要支持协程的函数类必须是可拷贝构造和赋值构造的，其既可以作为实现类的基类派生，也可以作为实现类的一个成员变量，甚至是`lambda`、`bind`的参数。

* Stackless Coroutines

```cpp
struct session : boost::asio::coroutine
{
  boost::shared_ptr<tcp::socket> socket_;
  boost::shared_ptr<std::vector<char> > buffer_;

  session(boost::shared_ptr<tcp::socket> socket)
    : socket_(socket),
      buffer_(new std::vector<char>(1024))
  {
  }

  void operator()(boost::system::error_code ec = boost::system::error_code(), std::size_t n = 0)
  {
    if (!ec) reenter (this)
    {
      for (;;)
      {
        yield socket_->async_read_some(boost::asio::buffer(*buffer_), *this);
        yield boost::asio::async_write(*socket_, boost::asio::buffer(*buffer_, n), *this);
      }
    }
  }
};
```

其定义了几个C++标准之外的伪关键字方便使用：

* reenter
* yield
- yield ;
- yield break ;
- yield statement
- yield return expression ;
- fork statement

## Boost.Corountine2

使用了 `Boost.Context` 进行上下文的切换，因此要使用 `Boost.Corountine2`，必须先编译 `Boost.Context`。
每个协程都有自己的 `stack` 和 `control-block(boost::contexts::fcontext_t)`，在协程需要暂停的时候，当前协程的所有非易失的寄存器(包括ESP、EIP)都会被保存在`control-block`当中，而新激活的协程会从其相关的`control-block`中加载回复相关的寄存器信息，称之为上下文切换，相关的上下文切换不需要系统特权。
`Boost.Context`提供的协程包括两类：非对称型协程`asymmetric_coroutine`的和对称型协程`symmetric_coroutine`。

非对称型协程知道唤醒自己的协程，当需要暂停的时候控制流转换给那个特定的协程。
对称协程中所有的协程都是相等的，协程可以把控制流给任何一个其它的协程。

所以对称协程主要是表达并发编程中的多个独立的执行单元，而非对称协程常常用于对数据进行顺序处理的过程。

## ucontext 接口

接口主要有如下四个：

- getcontext
获取当前context
- setcontext
切换到指定context
- makecontext
用于将一个新函数和堆栈，绑定到指定context中
- swapcontext
保存当前context，并且切换到指定context

`setcontext`与`getcontext`是对寄存器进行保存和恢复：
```c
#define setcontext(u)   setmcontext(&(u)->uc_mcontext)
#define getcontext(u)   getmcontext(&(u)->uc_mcontext)
```
`makecontext` 主要的工作就是设置函数指针 和 堆栈到对应`context`保存的`sp`和`pc`寄存器中，这也就是为什么`makecontext`前，必须要先调用`getcontext`的原因。

```c
void makecontext(ucontext_t *uc, void (*fn)(void), int argc, ...)
{
    int i, *sp;
    va_list arg;
    
    // 将函数参数陆续设置到r0, r1，r2 .. 等参数寄存器中
    sp = (int*)uc->uc_stack.ss_sp + uc->uc_stack.ss_size / 4;
    va_start(arg, argc);
    for(i=0; i<4 && iuc_mcontext.gregs[i] = va_arg(arg, uint);
    va_end(arg);

    // 设置堆栈指针到sp寄存器
    uc->uc_mcontext.gregs[13] = (uint)sp;

    // 设置函数指针到lr寄存器，切换时会设置到pc寄存器中进行跳转到fn
    uc->uc_mcontext.gregs[14] = (uint)fn;
}
```
这套接口简单有效，不支持的平台还可以通过汇编实现来支持，看上去已经很完美了，但是确有个问题，就是效率不高，因为每次切换保存和恢复的寄存器太多。

```c
#include <ucontext.h>

static ucontext_t ctx[3];

static void func1(void)
{
    // 切换到func2
    swapcontext(&ctx[1], &ctx[2]);

    // 返回后，切换到ctx[1].uc_link，也就是main的swapcontext返回处
}
static void func2(void)
{
    // 切换到func1
    swapcontext(&ctx[2], &ctx[1]);

    // 返回后，切换到ctx[2].uc_link，也就是func1的swapcontext返回处
}

int main (void)
{
    // 初始化context1，绑定函数func1和堆栈stack1
    char stack1[8192];
    getcontext(&ctx[1]);
    ctx[1].uc_stack.ss_sp   = stack1;
    ctx[1].uc_stack.ss_size = sizeof(stack1);
    ctx[1].uc_link = &ctx[0];
    makecontext(&ctx[1], func1, 0);

    // 初始化context2，绑定函数func2和堆栈stack2
    char stack2[8192];
    getcontext(&ctx[2]);
    ctx[2].uc_stack.ss_sp   = stack2;
    ctx[2].uc_stack.ss_size = sizeof(stack1);
    ctx[2].uc_link = &ctx[1];
    makecontext(&ctx[2], func2, 0);

    // 保存当前context，然后切换到context2上去，也就是func2
    swapcontext(&ctx[0], &ctx[2]);
    return 0;
}
```

## setjmp/longjmp（sigsetjmp/siglongjmp）接口

libmill里面的切换主要用的就是此套接口，其实应该是sigsetjmp/siglongjmp，不仅保存了寄存器，还保存了signal mask。

## windows 的 fibers 接口

## tbox

http://fabiensanglard.net/macosxassembly/index.php

```asm
/* make context (refer to boost.context)
 *
 *             -------------------------------------------------------------------------------
 * stackdata: |                                                |         context        |||||||
 *             -------------------------------------------------------------------------|-----
 *                                                                             (16-align for macosx)
 *
 *
 *             -------------------------------------------------------------------------------
 * context:   |   r12   |   r13   |   r14   |   r15   |   rbx   |   rbp   |   rip   |   end   | ...
 *             -------------------------------------------------------------------------------
 *            0         8         16        24        32        40        48        56        |
 *                                                                                  |  16-align for macosx
 *                                                                                  |
 *                                                                       esp when jump to function
 *
 * @param stackdata     the stack data (rdi)
 * @param stacksize     the stack size (rsi)
 * @param func          the entry function (rdx)
 *
 * @return              the context pointer (rax)
 */
function(tb_context_make)

    // 保存栈顶指针到rax
    addq %rsi, %rdi
    movq %rdi, %rax

    /* 先对栈指针进行16字节对齐
     *
     *                      
     *             ------------------------------
     * context:   | retaddr |    padding ...     |
     *             ------------------------------
     *            |         | 
     *            |     此处16字节对齐
     *            |
     *  esp到此处时，会进行ret
     *
     * 这么做，主要是因为macosx下，对调用栈布局进行了优化，在保存调用函数返回地址的堆栈处，需要进行16字节对齐，方便利用SIMD进行优化
     */
    movabs $-16, %r8
    andq %r8, %rax

    // 保留context需要的一些空间，因为context和stack是在一起的，stack底指针就是context
    leaq -64(%rax), %rax

    // 保存func函数地址到context.rip
    movq %rdx, 48(%rax)

    /* 保存__end地址到context.end，如果在在func返回时，没有指定jump切换到有效context
     * 那么会继续会执行到此处，程序也就退出了
     */
    leaq __end(%rip), %rcx
    movq %rcx, 56(%rax)

    // 返回rax指向的栈底指针，作为context返回
    ret 

__end:
    // exit(0)
    xorq %rdi, %rdi
#ifdef TB_ARCH_ELF
    call _exit@PLT
#else
    call __exit
#endif
    hlt

endfunc

/* jump context (refer to boost.context)
 *
 * @param context       the to-context (rdi)
 * @param priv          the passed user private data (rsi)
 *
 * @return              the from-context (context: rax, priv: rdx)
 */
function(tb_context_jump)

    // 保存寄存器，并且按布局构造成当前context，包括jump()自身的返回地址retaddr(rip)
    pushq %rbp
    pushq %rbx
    pushq %r15
    pushq %r14
    pushq %r13
    pushq %r12

    // 保存当前栈基址rsp，也就是contex，到rax中
    movq %rsp, %rax

    // 切换到指定的新context上去，也就是切换堆栈
    movq %rdi, %rsp

    // 然后按context上的栈布局依次恢复寄存器
    popq %r12
    popq %r13
    popq %r14
    popq %r15
    popq %rbx
    popq %rbp

    // 获取context.rip，也就是make时候指定的func函数地址，或者是对方context中jump()调用的返回地址
    popq %r8

    // 设置返回值(from.context: rax, from.priv: rdx)，也就是来自对方jump()的context和传递参数
    movq %rsi, %rdx

    // 传递当前(context: rax, priv: rdx)，作为function(from)函数调用的入口参数
    movq %rax, %rdi

    /* 跳转切换到make时候指定的func函数地址，或者是对方context中jump()调用的返回地址
     *
     * 切换过去后，此时的栈布局如下：
     *
     * end是func的返回地址，也就是exit
     *
     *             -------------------------------
     * context: .. |   end   | args | padding ... |
     *             -------------------------------
     *             0             8        
     *             |             |  
     *            rsp   16-align for macosx
     */
    jmp *%r8

endfunc
```