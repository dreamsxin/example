# 信号量

信号量其实是一个计数器，用于计算当前的资源是否可用。为了获得共享资源，并且保持数据的同步，我们可以这样来使用信号量：

- 1) 测试控制该资源的信号量
- 2) 若此信号量的值为正，则进程可以使用该资源。对信号量做减1操作，表示他占用了这个资源
- 3) 若此信号量为的值0，则表示该资源已被占用。进程挂起等待，直到占用资源的进程释放该资源，使信号量的值为正。此时进程被唤醒，又返回至第1步

通常信号量的值可以取任意正值，代表有多少个共享的资源。但一般情况下，我只使用0和1两个值，分别代表某一个资源的占用与空闲，这也叫做二进制信号量。

信号量集是包含一个或多个信号量的集合，但通常我们只使用含有一个信号量的信号量集。

## 数据结构

每个信号量在内核中都有一个无名的结构表示，他至少包括以下成员：

```c
struct {
unsigned short semval;   //信号量的值
pid_t sempid;            //最后一次操作信号量的进程的ID
unsigned short semncnt;  //挂起的等待至信号量的值变为正的进程数
unsigned short semzcnt;  //挂起等待至信号量的值为0的进程数
// ...
}
```

## 获取信号量标识符

```c
int semget(key_t key, int nsems, int flag)
```

参数：
- key代表键值，可以用ftok接口产生
- nsems代表信号量的数目，若创建信号量则必须指定nsems，若引用已存在的信号量则将nsems指定为0
- flag：类似open参数里的flag标志。他低端的9比特是信号量的权限位，同时我们可以用该权限标志和IPC_CREAT标志作或操作，这样就可以创建一个新的信号量。当然若是该信号量已存在，也不会产生错误，该标志将会被忽略。

返回值：

成功返回信号量标识符，失败返回0

## 操作信号量

```c
int semctl(int semid, int semnum, int cmd, /*union semun*/)
```
参数：
- semid 信号量标识符
- semnum：要操作的信号量，若是在信号集中则可以取0值nsems-1之间的任意值，若只有一个信号量则取0即可。
- cmd：操作信号量的指令，系统提供了10种不同的指令提供用户操作信号量。这里我们只介绍两个最常用的指令，其余的指令可以查阅资料获得。
- semun

	a) SETVAL：用来把一个信号量的值初始化为我们设定的值，这个值通过第四个参数semun的val成员提供。该成员的结构如下：
```c
union semnu
{
int val;
stuct semid_ds *buf;
unsigned short *array;
};
```
	b) IPC_RMID：用于删除一个不再使用的信号量。该删除操作是立即发生的。任何在使用该信号量的进程，将在下次试图对信号量操作时产生错误。

返回值：

根据cmd的不同，返回不同的值。对于SETVAL和IPC_RMID这两个指令，成功返回0，失败返回-1.

## 对信号量进行增减操作

在该操作是一个原子操作：
```c
int semop(int semid, struct sembuf semoparray[], size_t nops)
```

参数：
- semid： 信号量标识符
- semoparray[]：指向信号量操作数组的指针。该指针有如下结构：
```c
struct sembuf{
unsigned short sem_num; //信号量的编号
short sem_op; //操作中改变的数值，+1表示释放资源，-1表示占用资源
short sem_flag; //通常设为SEM_UNDO，当系统在没有释放信号量的情况下退出进程，系统将自动释放进程占用的信号量
}
```
- nops : 操作的信号数量

返回值：

成功返回0，失败返回-1

## 信号量使用

系统提供的接口相对复杂，使我们不太清楚信号量的使用方法。下面我们将利用系统提供的接口编写信号量操作的简单接口，当然这个接口是针对二进制信号量的（只取0和1）。

### 创建信号量

```c
int creat_sem(void)
{
    int semid = 0;
    key_t key;

    key = ftok(FTOK_FILE, 11);
    if(key == -1)
    {
        printf("%s : key = -1!\n",__func__);
        return -1;
    }

    semid = semget(key, 1, IPC_CREAT|0666);
    if(semid == -1)
    {
        printf("%s : semid = -1!\n",__func__);
        return -1;
    }

    return semid;
    
}
```

### 初始化信号量。在使用之前必须先初始化

```c
int set_semvalue(int semid)
{
    union semun sem_arg;
    sem_arg.val = 1;

    if(semctl(semid, 0, SETVAL, sem_arg) == -1)
    {
        printf("%s : can't set value for sem!\n",__func__);
        return -1;
    }
    return 0;
}
```

### 占用资源，即执行p操作（p操作是用于描述获取信号量的术语）

```c
int sem_p(int semid)
{
    struct sembuf sem_arg;
    sem_arg.sem_num = 0;
    sem_arg.sem_op = -1;
    sem_arg.sem_flg = SEM_UNDO;

    if(semop(semid, & sem_arg, 1) == -1)
    {
        printf("%s : can't do the sem_p!\n",__func__);
        return -1;
    }
    return 0;
}
```

### 释放资源，即v操作（v操作适用于描述释放信号量的术语）

```c
int sem_v(int semid)
{
    struct sembuf sem_arg;
    sem_arg.sem_num = 0;
    sem_arg.sem_op = 1;
    sem_arg.sem_flg = SEM_UNDO;

    if(semop(semid, & sem_arg, 1) == -1)
    {
        printf("%s : can't do the sem_v!\n",__func__);
        return -1;
    }
    return 0;
}
```

### 删除信号量。

```c
int del_sem(int semid)
{
    if(semctl(semid, 0, IPC_RMID) == -1)
    {
        printf("%s : can't rm the sem!\n",__func__);
        return -1;
    }
    return 0;
}
```
