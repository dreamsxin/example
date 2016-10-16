# 共享内存

使用过程可分为 `创建`->`连接`->`使用`->`分离`->`销毁` 这几步。

## 创建

共享内存的创建使用`shmget（SHared Memory GET）函数`，使用方法如下：
```c
int segment_id = shmget (shm_key, getpagesize (),IPC_CREAT | S_IRUSR | S_IWUSER);
```
`shmget`根据`shm_key`创建一个大小为`page_size`的共享内存空间，参数3是一系列的创建参数。如果`shm_key`已经创建，使用该`shm_key`会返回可以连接到该以创建共享内存的id。

## 连接

为了使共享内存可以被当前进程使用，必须紧接着进行连接操作。使用函数`shmat（SHared Memory ATtach）`，参数传入通过shmget返回的共享内存id即可：
```c
shared_memory = (char*) shmat (segment_id, 0, 0);
```

## 使用

shmat返回映射到进程虚拟地址空间的地址指针，这样进程就能像访问一块普通的内存缓冲一样访问共享内存。例子中后两个参数我们全部传入0，采用默认的连接方式。实际上，第二个参数是希望连接的进程虚拟地址空间的地址，如果使用0，Linux会自己选用一个合适的地址；第三个参数是连接选项，可以是：
 
- SHM_RND：将第二个参数指向的内存空间自动提升到page size的整数倍，如果不知明该参数，你需要自己控制第二个参数所指内存空间的大小。
- SHM_RDONLY：该共享内存是只读的，不可写。

## 分离

当共享内存使用完毕后，使用函数`shmdt (SHared Memory DeTach)`进行解连接。该函数以shmat返回的内存地址作为参数

## 销毁

。每最后一个使用该共享内存的进程分离该共享内存后，内核将会对该共享内存自动销毁。当然，我们最好能显式的进行销毁，以避免不必要的共享内存资源浪费。函数`shmctl (SHared Memory ConTroL)`可以返回共享内存的信息并对其进行控制，如：
```c
shmctl(segment_id, IPC_STAT, &shmbuffer);
```

可以返回该共享内存的信息，并将信息保存在第三个参数指向的shmid_ds结构体中。当向第二个参数传入IPC_RMID时，共享内存将会在最后一个使用该共享内存的进程分离共享内存是销毁共享内存。

## 示例程序

下面的示例程序，进程a每一秒的向共享内存写入一个随机数，进程b每隔一秒从该共享内存读出该数。
`a.c`
```c
/* 
 * a.c 
 * write a random number between 0 and 999 to the shm every 1 second 
*/
#include<stdio.h>  
#include<unistd.h>  
#include<sys/shm.h>  
#include<stdlib.h>  
#include<error.h>  
int main(){
    int shm_id;
    int *share;
    int num;
    srand(time(NULL));
    shm_id = shmget (1234, getpagesize(), IPC_CREAT);
    if(shm_id == -1){
        perror("shmget()");  
    }
    share = (int *)shmat(shm_id, 0, 0);
    while(1){
        num = random() % 1000;  
        *share = num;  
        printf("write a random number %d\n", num);  
        sleep(1);  
    }
    return 0;
}
```

`b.c`
```c
/* 
 * b.c 
 * read from the shm every 1 second 
*/
#include<stdio.h>  
#include<unistd.h>  
#include<sys/shm.h>  
#include<stdlib.h>  
#include<error.h>  
int main(){
    int shm_id;
    int *share;
    shm_id = shmget (1234, getpagesize(), IPC_CREAT);
    if(shm_id == -1){
        perror("shmget()");  
    }
    share = (int *)shmat(shm_id, 0, 0);
    while(1){
        sleep(1);  
        printf("%d\n", *share);  
    }
    return 0;
}
```
