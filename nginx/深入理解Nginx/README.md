# 基本的数据结构和方法

## 整形

```c
typedef intptr_t	ngx_int_t;	// 有符号整形
typedef uintptr_t	ngx_uint_t;	// 无符号整形
```

## 字符串

```c
typedef struct {
    size_t	len;	// 字符串有效长度
    u_char	*data;	// 字符串起始位置，未必以'\0'结尾
}
```

相关的方法：`ngx_strncmp`

## 链表容器

```c
typedef struct ngx_list_part_s ngx_list_part_t;
struct ngx_list_part_s {
    void		*elts;	// 数组的起始地址
    ngx_uint_t		nelts;	// 已有多少个元素
    ngx_list_part_t	*next;	// 下一个链表元素地址
}

typedef struct {
    ngx_list_part_t	*part;	//首个链表元素地址
    ngx_list_part_t	*last;	// 最后一个链表元素地址
    size_t		size;	// 链表元素中每一个数组元素占用空间大小
    ngx_uint_t		nalloc;	// 链表元素中数组元素最大个数 
    ngx_pool_t		*pool;	// 内存池对象
} ngx_list_t;
```

相关的方法：`ngx_list_create`、`ngx_list_init`、`ngx_list_push`

## 表格元素数据结构

```c
typedef struct {
    ngx_uint_t	hash;		// 用于在 ngx_hash_t 快速查找
    ngx_str_t	key;		// 名称
    ngx_str_t	value;		// 值
    u_char	*lowcase_key;	// 小写名称
}
```

## hash 结构
```c
typedef struct {
    void	*value;		// value，即某个key对应的值，即<key,value>中的value
    u_short	len;		// name 长度
    u_char	name[1];	// 某个要hash的数据(在nginx中表现为字符串)，即<key,value>中的key
} ngx_hash_elt_t;

typedef struct {
    ngx_hash_elt_t	**buckets;	// hash桶(有size个桶)
    ngx_uint_t		size;		// hash桶个数
} ngx_hash_t;

typedef struct {
    ngx_hash_t		*hash;		// 指向待初始化的hash结构
    ngx_hash_key_pt	key;		// hash函数指针

    ngx_uint_t		max_size;	// bucket的最大个数
    ngx_uint_t		bucket_size;	// 每个bucket的空间

    char		*name;		// 该hash结构的名字(仅在错误日志中使用)
    ngx_pool_t		*pool;		// 该hash结构从pool指向的内存池中分配
    ngx_pool_t		*temp_pool;	// 分配临时数据空间的内存池
} ngx_hash_init_t;

typedef struct {
    ngx_str_t		key;		// key，为nginx的字符串结构
    ngx_uint_t		key_hash;	// 由该key计算出的hash值(通过hash函数如ngx_hash_key_lc())
    void		*value;		// 该key对应的值，组成一个键-值对<key,value>
} ngx_hash_key_t;

typedef struct {
    size_t	len;	// 字符串长度
    u_char	*data;	// 字符串内容
} ngx_str_t;
```

相关方法：`ngx_hash_init`、`ngx_hash_wildcard_init`、`ngx_hash_key`、`ngx_hash_key_lc`、`ngx_hash_strlow`

## 