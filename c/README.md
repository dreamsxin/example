
* label

```c
int main() 
{
     static void* p = &&label;
     goto *p;

     printf("before label\n");
     label:
     printf("after label\n");

     return 0;
}
```

- &&label是语法，&&就是获得label的地址，并不是取地址在取地址。
- goto *p 就是跳转到p指针，所指向的地址。
- 如果p指向了的是不正确的地址，程序会运行时崩溃。
- label地址需要用void*指针才存放。