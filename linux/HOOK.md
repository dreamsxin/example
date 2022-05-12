# HOOK

## LD_PRELOAD

利用 LD_PRELOAD 可以影响程序的运行时的链接，它允许你定义在程序运行前优先加载的动态链接库。
这个功能主要就是用来有选择性的载入Unix操作系统不同动态链接库中的相同函数。通过这个环境变量，我们可以在主程序和其动态链接库的中间加载别的动态链接库，甚至覆盖正常的函数库。
一方面，我们可以以此功能来使用自己的或是更好的函数（无需别人的源码），而另一方面，我们也可以以向别人的程序注入恶意程序，从而达到那不可告人的罪恶的目的。

下面，我们使用一个动态函数库来重载strcmp函数：
```
// verifypasswd.c
#include <stdio.h>
#include <string.h>

int main(int argc,char **argv) {
    char passwd[] = "password";
    if (argc < 2) {
        printf("usage: %s <password>\n",argv[0]);
        return;
    }
    if (!strcmp(passwd,argv[1])) {
        printf("Correct Password!\n");
        return;
    }
    printf("Invalid Password!\n");
}
```
```c
// hack.c
#include <stdio.h>
#include <string.h>

int strcmp(const char *s1, const char *s2)
{
	printf("hack function invoked. s1=<%s> s2=<%s>\n");
	return 0;
}
```
编译：
```shell
gcc -o verifypasswd verifypasswd.c
gcc -shared -o hack.so hack.c

export LD_PRELOAD="./hack.so"
./verifypasswd test
```
编译指定动态库
```shell
gcc -c hack.c
gcc -shared -fPCI -o libhack.so hack.o
gcc -o verifypasswd verifypasswd.c -L. -lhack -Wl,-rpath,./
```

- `-shared`: 
属性为可共享文件
- `-fPCI`: 
作用于编译阶段，告诉编译器产生与位置无关代码(Position-Independent Code)， 则产生的代码中，没有绝对地址，全部使用相对地址，故而代码可以被加载器加载到内存的任意 位置，都可以正确的执行。这正是共享库所要求的，共享库被加载时，在内存的位置不是固定的。

## 普通函数

运行时把一个函数的入口 jmp 到另一个地址

```c
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>
#include <errno.h>
#include <unistd.h>

void set_hook(void *to_mock_func, void *mock_func)
{
    uint8_t machine_code[] = {
            //movq $0x0, %rax 后面8个字节的0为64位立即数
            0x48, 0xb8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            //jmpq *%rax
            0xff, 0xe0
        };

    int pagesize = sysconf(_SC_PAGE_SIZE);
    if (pagesize == -1)
    {
        exit(errno);
    }

    uint8_t *mem = (uint8_t *) to_mock_func;
    void *p = (uint8_t*) (mem - ((uint64_t) mem % pagesize));
    if (mprotect(p, pagesize, PROT_READ | PROT_WRITE | PROT_EXEC))
    {
        perror("mprotect error");
        exit(errno);
    }

    //改写立即数为mock的地址,写入函数入口处
    memcpy(machine_code + 2, &mock_func, sizeof(mock_func));
    memcpy(mem, machine_code, sizeof(machine_code));

    if (mprotect(p, pagesize, PROT_EXEC))
    {
        perror("mprotect error");
        exit(errno);
    }
}

using namespace std;
class Cal
{
public:
    int Sum(int a, int b, long c, long d, long e)
    {
        std::cout << "Cal::Sum called... " << std::endl;
        return 0;
    }
    int m;
};

int MockCalSum(Cal *p, int a, int b, long c, long d, long e)
{
    int x = 0;
    x += a + b + c + d;
    std::cout << "MockCalSum called... " << std::endl;
    p->m = x;
    return x;
}

int main()
{
    int a = 0;
    int b = 1;
    long c = 2;
    long d = 3;
    long e = 4;
    Cal cal;
    cal.Sum(a, b, c, d, e);
    MockCalSum(&cal, a, b, c, d, e);
    set_hook((void*)&Cal::Sum, (void*)MockCalSum);
    cal.Sum(a, b, c, d, e);
    return 0;
}
```
