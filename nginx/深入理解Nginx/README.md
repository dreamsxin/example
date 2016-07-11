# 基本的数据结构和方法

在头文件 `src/core/ngx_core.h` 定义了类型
```c
typedef struct ngx_module_s      ngx_module_t;
typedef struct ngx_conf_s        ngx_conf_t;
typedef struct ngx_cycle_s       ngx_cycle_t;
typedef struct ngx_pool_s        ngx_pool_t;
typedef struct ngx_chain_s       ngx_chain_t;
typedef struct ngx_log_s         ngx_log_t;
typedef struct ngx_open_file_s   ngx_open_file_t;
typedef struct ngx_command_s     ngx_command_t;
typedef struct ngx_file_s        ngx_file_t;
typedef struct ngx_event_s       ngx_event_t;
typedef struct ngx_event_aio_s   ngx_event_aio_t;
typedef struct ngx_connection_s  ngx_connection_t;
```

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

相关的方法：`ngx_string`、`ngx_strncmp`

## buf 数据结构
定义在文件 `src/core/ngx_buf.h`

```c
typedef struct ngx_buf_s  ngx_buf_t;

struct ngx_buf_s {
    u_char          *pos;
    u_char          *last;
    off_t            file_pos;
    off_t            file_last;

    u_char          *start;         /* start of buffer */
    u_char          *end;           /* end of buffer */
    ngx_buf_tag_t    tag;
    ngx_file_t      *file;
    ngx_buf_t       *shadow;


    /* the buf's content could be changed */
    unsigned         temporary:1;

    /*
     * the buf's content is in a memory cache or in a read only memory
     * and must not be changed
     */
    unsigned         memory:1;

    /* the buf's content is mmap()ed and must not be changed */
    unsigned         mmap:1;

    unsigned         recycled:1;
    unsigned         in_file:1;
    unsigned         flush:1;
    unsigned         sync:1;
    unsigned         last_buf:1;
    unsigned         last_in_chain:1;

    unsigned         last_shadow:1;
    unsigned         temp_file:1;

    /* STUB */ int   num;
};

struct ngx_chain_s {
    ngx_buf_t    *buf;
    ngx_chain_t  *next;
};
```

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
} ngx_table_elt_t;
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

# 创建扩展 `ngx_http_mytest_module`

## 编写config

```config
ngx_addon_name=ngx_http_mytest_module
HTTP_MODULES="$HTTP_MODULES ngx_http_mytest_module"
NGX_ADDON_SRCS="$NGX_ADDON_SRCS $ngx_addon_dir/ngx_http_mytest_module.c"
```

## 编写 `ngx_http_mytest_module.c`

```c
#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>


static char *ngx_http_mytest(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);

static ngx_int_t ngx_http_mytest_handler(ngx_http_request_t *r);

static ngx_command_t  ngx_http_mytest_commands[] =
{

    {
        ngx_string("mytest"),
        NGX_HTTP_MAIN_CONF | NGX_HTTP_SRV_CONF | NGX_HTTP_LOC_CONF | NGX_HTTP_LMT_CONF | NGX_CONF_NOARGS,
        ngx_http_mytest,
        NGX_HTTP_LOC_CONF_OFFSET,
        0,
        NULL
    },

    ngx_null_command
};

static ngx_http_module_t  ngx_http_mytest_module_ctx =
{
    NULL,                              /* preconfiguration */
    NULL,                  		/* postconfiguration */

    NULL,                              /* create main configuration */
    NULL,                              /* init main configuration */

    NULL,                              /* create server configuration */
    NULL,                              /* merge server configuration */

    NULL,       			/* create location configuration */
    NULL         			/* merge location configuration */
};

ngx_module_t  ngx_http_mytest_module =
{
    NGX_MODULE_V1,
    &ngx_http_mytest_module_ctx,           /* module context */
    ngx_http_mytest_commands,              /* module directives */
    NGX_HTTP_MODULE,                       /* module type */
    NULL,                                  /* init master */
    NULL,                                  /* init module */
    NULL,                                  /* init process */
    NULL,                                  /* init thread */
    NULL,                                  /* exit thread */
    NULL,                                  /* exit process */
    NULL,                                  /* exit master */
    NGX_MODULE_V1_PADDING
};

static char *ngx_http_mytest(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_http_core_loc_conf_t  *clcf;

    // 首先找到mytest配置项所属的配置块，clcf可以是main、srv或者loc级别配置项，也就是说在每个http{}和server{}内也都有一个ngx_http_core_loc_conf_t结构体
    clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);

    // http框架在处理用户请求进行到NGX_HTTP_CONTENT_PHASE阶段时，如果请求的主机域名、URI与mytest配置项所在的配置块相匹配，就将调用我们实现的ngx_http_mytest_handler方法处理这个请求
    clcf->handler = ngx_http_mytest_handler;

    return NGX_CONF_OK;
}

static ngx_int_t ngx_http_mytest_handler(ngx_http_request_t *r)
{
    //必须是GET或者HEAD方法，否则返回405 Not Allowed
    if (!(r->method & (NGX_HTTP_GET | NGX_HTTP_HEAD))) {
        return NGX_HTTP_NOT_ALLOWED;
    }

    //丢弃请求中的包体
    ngx_int_t rc = ngx_http_discard_request_body(r);
    if (rc != NGX_OK) {
        return rc;
    }

    //设置返回的Content-Type
    ngx_str_t type = ngx_string("text/plain");

    //返回的包体内容
    ngx_str_t response = ngx_string("Hello World!");

    //设置返回状态码
    r->headers_out.status = NGX_HTTP_OK;

    //响应包是有包体内容的，所以需要设置Content-Length长度
    r->headers_out.content_length_n = response.len;

    //设置Content-Type
    r->headers_out.content_type = type;

    //发送http头部
    rc = ngx_http_send_header(r);
    if (rc == NGX_ERROR || rc > NGX_OK || r->header_only) {
        return rc;
    }

    //构造ngx_buf_t结构准备发送包体
    ngx_buf_t *b;
    b = ngx_create_temp_buf(r->pool, response.len);
    if (b == NULL) {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

    // 将Hello World拷贝到ngx_buf_t指向的内存中
    ngx_memcpy(b->pos, response.data, response.len);

    // 注意，一定要设置好last指针
    b->last = b->pos + response.len;

    // 声明这是最后一块缓冲区
    b->last_buf = 1;

    // 构造发送时的ngx_chain_t结构体
    ngx_chain_t out;
    out.buf = b;
    out.next = NULL;

    return ngx_http_output_filter(r, &out);
}
```