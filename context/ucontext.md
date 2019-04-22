## ucontext

`ucontext.h` or `x86_64-linux-gnu/sys/ucontext.h`

```c
/* Structure to describe FPU registers.  */
typedef struct _libc_fpstate *fpregset_t;

/* Context to describe whole processor state.  */
typedef struct
  { 
    gregset_t gregs;
    /* Note that fpregs is a pointer.  */
    fpregset_t fpregs;
    __extension__ unsigned long long __reserved1 [8];
} mcontext_t;

/* Userlevel context.  */
typedef struct ucontext
  {
    unsigned long int uc_flags;
    struct ucontext *uc_link;
    stack_t uc_stack;
    mcontext_t uc_mcontext;
    __sigset_t uc_sigmask;
    struct _libc_fpstate __fpregs_mem;
  } ucontext_t

/* Definitions taken from the kernel headers.  */
struct _libc_fpreg
{
  unsigned short int significand[4];
  unsigned short int exponent;
};

struct _libc_fpstate
{
  unsigned long int cw;
  unsigned long int sw;
  unsigned long int tag;
  unsigned long int ipoff;
  unsigned long int cssel;
  unsigned long int dataoff;
  unsigned long int datasel;
  struct _libc_fpreg _st[8];
  unsigned long int status;
};

int getcontext(ucontext_t* ucp);
int setcontext(const ucontext_t* ucp);
void makecontext(ucontext_t* ucp, void (*func)(), int argc, ...);
int swapcontext(ucontext_t* ocp, const ucontext_t* ucp);
 ```

* `ucontext_t`

其中 `uc_stack` 是上下文切换时使用的栈空间，`uc_link` 指向下一个上下文。


* `getcontext` & `setcontext`
调用 `getcontext` 把当前的上下文保存到 `ucontext_t` 结构中，`setcontext` 将保存的上下文信息设为程序当前的上下文。
下面这个程序会一直不停的打印"hello world"。

```c
#include <ucontext.h>
#include <stdio.h>
 
int main()
{
    ucontext_t context;
    getcontext(&context);
    printf("hello world\n");
    setcontext(&context);
     
    return 0;
}
```

* `makecontext`

自定义一个上下文，它的第二个参数是指向要调用函数的指针，第三个参数是调用该函数时参数的个数，之后的可变参数是调用函数时的参数。
下面这个程序只会打印 "I'm parameter."，不会打印 "return"。
```c
#include <ucontext.h>
#include <stdio.h>
 
unsigned char stack[10240];
 
int func(const char* str)
{
    printf("%s\n",str);
    return 0;
}
 
int main()
{
    ucontext_t context;
    getcontext(&context);
    context.uc_stack.ss_sp = stack;
    context.uc_stack.ss_size = 10240;
    context.uc_link = NULL;
    makecontext(&context, (void(*)(void)) func , 1,"I'm parameter.");
    setcontext(&context);
    printf("return\n");
    return 0;
}
```

* `swapcontext`

保存当前上下文到第一个参数中，然后将第二个参数的内容设为当前的上下文。
下面这个程序，`uc_link` 设为back，使用 `swapcontext`保存当前的上下文到`back`中，因而能够打印 "return"。
```c
#include <ucontext.h>
#include <stdio.h>
 
unsigned char stack[10240];
 
int func(const char* str)
{
    printf("%s\n",str);
    return 0;
}
 
int main()
{
    ucontext_t context,back;
    getcontext(&context);
    context.uc_stack.ss_sp = stack;
    context.uc_stack.ss_size = 10240;
    context.uc_link = &back;
    makecontext(&context, (void(*)(void)) func , 1,"I'm parameter.");
    swapcontext(&back, &context);
    printf("return\n");
    return 0;
}
```

## 协同程序


协程包含两个操作恢复(resume)和让出(yield)，此外在加上创建：
- 创建
将协程的函数和参数保存起来，返回一个指示它们的句柄。
- 恢复
第一次调用，通过 `makecontext` 和 `swapcontext` 启动协程函数。
- 让出
通过 `swapcontext` 切换到恢复函数中调用`swapcontext`的位置。
