# libuv 源码分析

- 在 linux 上，是对epoll的封装；
- 在 windows 上，是对完成端口的封装；
- 在 macOS/FreeBSD 上，是对 kqueue 的封装。

![https://github.com/dreamsxin/example/blob/master/libuv/img/01.png](https://github.com/dreamsxin/example/blob/master/libuv/img/01.png?raw=true)

## loop

* uv_run

是 uv 消息队列的入口函数。

![https://github.com/dreamsxin/example/blob/master/libuv/img/02.png](https://github.com/dreamsxin/example/blob/master/libuv/img/02.png?raw=true)
![https://github.com/dreamsxin/example/blob/master/libuv/img/03.png](https://github.com/dreamsxin/example/blob/master/libuv/img/03.png?raw=true)

Event Loop 的执行顺序：

1. timers 阶段
timers 是事件循环的第一个阶段，会去检查有无已过期的timer，如果有则把它的回调压入 timer 的任务队列中等待执行，事实上，并不能保证timer在预设时间到了就会立即执行，它会受机器上其它运行程序影响，或者那个时间点主线程不空闲。
（在 Node 中这个是定时器阶段，处理`setTimeout()`和`setInterval()`的回调函数。进入这个阶段后，主线程会检查一下当前时间，是否满足定时器的条件。如果满足就执行回调函数，否则就离开这个阶段。）

2. I/O callbacks 阶段
执行一些系统调用错误，比如网络通信的错误回调。

（
在 Node 中除了以下操作的回调函数，其他的回调函数都在这个阶段执行。

- setTimeout()和setInterval()的回调函数
- setImmediate()的回调函数
- 用于关闭请求的回调函数
）


3. idle, prepare 阶段
uv_idle 总是在 uv_prepare阶段之前运行。在它们的下一阶段是 `poll`。而poll阶段是不断轮训执行`callback`，所以是会阻塞的。
通过对 `uv_idle` 的开关控制，开的时候（uv_idle_start，idle 队列加入 handle）可以直接越过poll阶段，关的时候（uv_idle_stop）则执行 poll 阶段。
（在 Node 中，`uv_run_prepare`通知此时 vm(v8) 的状态。）

4. poll 阶段
获取新的I/O事件, 适当的条件下将阻塞在这里
（
在 Node 中，这个阶段是轮询时间，用于等待还未返回的 I/O 事件，比如服务器的回应、用户移动鼠标等等。
这个阶段的时间会比较长。如果没有其他异步任务要处理（比如到期的定时器），会一直停留在这个阶段，等待 I/O 请求返回结果。
）

5. check 阶段
（在 Node 中，该阶段执行setImmediate()的回调函数。）

6. close callbacks 阶段
该阶段执行关闭请求的回调函数。


```c
int uv_run(uv_loop_t* loop, uv_run_mode mode) {
  int timeout;
  int r;
  int ran_pending;

  r = uv__loop_alive(loop); // 是否有异步任务
  if (!r)
    uv__update_time(loop);

  while (r != 0 && loop->stop_flag == 0) {
    uv__update_time(loop);
    uv__run_timers(loop);
    ran_pending = uv__run_pending(loop);
    uv__run_idle(loop);
    uv__run_prepare(loop);

    timeout = 0;
    if ((mode == UV_RUN_ONCE && !ran_pending) || mode == UV_RUN_DEFAULT)
      timeout = uv_backend_timeout(loop);

    uv__io_poll(loop, timeout);
    uv__run_check(loop);
    uv__run_closing_handles(loop);

    if (mode == UV_RUN_ONCE) {
      /* UV_RUN_ONCE implies forward progress: at least one callback must have
       * been invoked when it returns. uv__io_poll() can return without doing
       * I/O (meaning: no callbacks) when its timeout expires - which means we
       * have pending timers that satisfy the forward progress constraint.
       *
       * UV_RUN_NOWAIT makes no guarantees about progress so it's omitted from
       * the check.
       */
      uv__update_time(loop);
      uv__run_timers(loop);
    }

    r = uv__loop_alive(loop);
    if (mode == UV_RUN_ONCE || mode == UV_RUN_NOWAIT)
      break;
  }

  /* The if statement lets gcc compile it to a conditional store. Avoids
   * dirtying a cache line.
   */
  if (loop->stop_flag != 0)
    loop->stop_flag = 0;

  return r;
}
```

- uv__update_time
更新时间

- uv__run_timers
调用timer。

- uv__run_pending
调用回调。通常来说回调一般在poll fd结束后就立即执行，但是总有例外：有一些I/O回调会延迟到下一次迭代中执行。那些被延迟的回调正是在这里执行

- uv__run_idle
执行idle。查看uv__run_idle的实现，发现仅仅是对idle进行遍历、执行回调，并没有删除的操作，所以idle是每次while都执行的。idle名不副实。(除了idle, prepare, check，都是执行完一个删除一个。既然回调都执行完了，保存也肯定没有必要)

- uv__run_prepare
执行prepare

- uv__io_poll(loop, timeout)
把新增的需要被监听的fd放到poll中；poll我们所关心的fd，注意有一个timeout。

- uv__run_check
执行check

- uv__run_closing_handles
执行close handle。

* uv_handle_t

```c
#define UV_HANDLE_FIELDS                                                      \
  /* public */                                                                \
  void* data;                                                                 \
  /* read-only */                                                             \
  uv_loop_t* loop;                                                            \
  uv_handle_type type;                                                        \
  /* private */                                                               \
  uv_close_cb close_cb;                                                       \
  void* handle_queue[2];                                                      \
  union {                                                                     \
    int fd;                                                                   \
    void* reserved[4];                                                        \
  } u;                                                                        \
  UV_HANDLE_PRIVATE_FIELDS                                                    \

/* The abstract base class of all handles. */
struct uv_handle_s {
  UV_HANDLE_FIELDS
};

typedef struct uv_handle_s uv_handle_t;
```

是`uv_tcp_t`, `uv_udp_t`, `uv_timer_t`, `uv_poll_t` 等的父类(结构体的继承，通过 `uv_handle_type` 属性进行标识类型)。

```c
struct uv_tcp_s {
  UV_HANDLE_FIELDS
  UV_STREAM_FIELDS
  UV_TCP_PRIVATE_FIELDS
};

typedef struct uv_tcp_s uv_tcp_t;
```

对与 `uv_handle_t` 的子类需要调用`uv_close(uv_handle_t*, uv_close_cb)`进行关闭。其中`uv_close_cb`一般用于释放资源，比如`handle`是通过`malloc`分配的，此时就要`free`。（特别要注意 `uv_timer_t`，他的 stop 不等同于 close。）

* uv_run_timers

```c
void uv__run_timers(uv_loop_t* loop) {
  struct heap_node* heap_node;
  uv_timer_t* handle;

  for (;;) {
    heap_node = heap_min(timer_heap(loop));
    if (heap_node == NULL)
      break;

    handle = container_of(heap_node, uv_timer_t, heap_node);
    if (handle->timeout > loop->time)
      break;

    uv_timer_stop(handle);
    uv_timer_again(handle);
    handle->timer_cb(handle);
  }
}
```
从一个小堆中依次取出 `heap_node`，如果超时了，执行 `timer_cb`，再把timer放回小堆里面，如果没超时，则`break`，后续的也不再检查（最小的都没超时，后面更大的也不可能超时）。

* uv_run_pending

```c
static int uv__run_pending(uv_loop_t* loop) {
  QUEUE* q;
  QUEUE pq;
  uv__io_t* w;

  if (QUEUE_EMPTY(&loop->pending_queue))
    return 0;

  QUEUE_MOVE(&loop->pending_queue, &pq);

  while (!QUEUE_EMPTY(&pq)) {
    q = QUEUE_HEAD(&pq);
    QUEUE_REMOVE(q);
    QUEUE_INIT(q);
    w = QUEUE_DATA(q, uv__io_t, pending_queue);
    w->cb(loop, w, POLLOUT);
  }

  return 1;
}
```

* `uv__run_idle`/`uv__run_prepare`/`uv__run_check`

同上。uv__run_prepare、uv__run_check和uv__run_idle都是一样的。因为它们的定义都是通过同一个宏实现的，从队列取出后，会再放回去。所以需要手动的`stop`。

* `uv_backend_timeout`/`uv__io_poll`

监听文件描述符，也就是调用`epoll`/`kqueue`/`IOCP`的函数。

```c
int uv__next_timeout(const uv_loop_t* loop) {
  const struct heap_node* heap_node;
  const uv_timer_t* handle;
  uint64_t diff;

  heap_node = heap_min(timer_heap(loop));
  if (heap_node == NULL)
    return -1; /* block indefinitely */

  handle = container_of(heap_node, uv_timer_t, heap_node);
  if (handle->timeout <= loop->time)
    return 0;

  diff = handle->timeout - loop->time;
  if (diff > INT_MAX)
    diff = INT_MAX;

  return (int) diff;
}

int uv_backend_timeout(const uv_loop_t* loop) {
  if (loop->stop_flag != 0)
    return 0;

  if (!uv__has_active_handles(loop) && !uv__has_active_reqs(loop))
    return 0;

  if (!QUEUE_EMPTY(&loop->idle_handles))
    return 0;

  if (!QUEUE_EMPTY(&loop->pending_queue))
    return 0;

  if (loop->closing_handles)
    return 0;

  return uv__next_timeout(loop);
}

static int uv__loop_alive(const uv_loop_t* loop) {
  return uv__has_active_handles(loop) ||
         uv__has_active_reqs(loop) ||
         loop->closing_handles != NULL;
}

int uv_loop_alive(const uv_loop_t* loop) {
    return uv__loop_alive(loop);
}
```

`uv_backend_timeout` 返回给 `uv__io_poll` 的 `timeout` 规则：
1. 没有活跃的handle，返回 0. epoll中，timeout 为 0的意思是立即返回。
2. idle或pending_queue不为空，返回0
3. loop被关闭返会0
4. timer队列为空，返回-1. epoll中，timeout为-1的意思是阻塞直到有fd有event产生
5. timer队列不为空，如果timer超时了，返回0。否则timeout为从现在到最早timer要超时的时间。比如现在是19:00:00, timer最早超时未19:00:10，那么timeout为10s。

这也就解释了上面说的不手动`stop`，`loop` 会一直空转，就是因为这里 `uv__io_poll` 超时时间为 0 不等待。

```c

void uv__io_poll(uv_loop_t* loop, int timeout) {
  struct port_event events[1024];
  struct port_event* pe;
  struct timespec spec;
  QUEUE* q;
  uv__io_t* w;
  sigset_t* pset;
  sigset_t set;
  uint64_t base;
  uint64_t diff;
  unsigned int nfds;
  unsigned int i;
  int saved_errno;
  int have_signals;
  int nevents;
  int count;
  int err;
  int fd;

  if (loop->nfds == 0) {
    assert(QUEUE_EMPTY(&loop->watcher_queue));
    return;
  }

  while (!QUEUE_EMPTY(&loop->watcher_queue)) {
    q = QUEUE_HEAD(&loop->watcher_queue);
    QUEUE_REMOVE(q);
    QUEUE_INIT(q);

    w = QUEUE_DATA(q, uv__io_t, watcher_queue);
    assert(w->pevents != 0);

    if (port_associate(loop->backend_fd, PORT_SOURCE_FD, w->fd, w->pevents, 0))
      abort();

    w->events = w->pevents;
  }

  pset = NULL;
  if (loop->flags & UV_LOOP_BLOCK_SIGPROF) {
    pset = &set;
    sigemptyset(pset);
    sigaddset(pset, SIGPROF);
  }

  assert(timeout >= -1);
  base = loop->time;
  count = 48; /* Benchmarks suggest this gives the best throughput. */

  for (;;) {
    if (timeout != -1) {
      spec.tv_sec = timeout / 1000;
      spec.tv_nsec = (timeout % 1000) * 1000000;
    }

    /* Work around a kernel bug where nfds is not updated. */
    events[0].portev_source = 0;

    nfds = 1;
    saved_errno = 0;

    if (pset != NULL)
      pthread_sigmask(SIG_BLOCK, pset, NULL);

    err = port_getn(loop->backend_fd,
                    events,
                    ARRAY_SIZE(events),
                    &nfds,
                    timeout == -1 ? NULL : &spec);

    if (pset != NULL)
      pthread_sigmask(SIG_UNBLOCK, pset, NULL);

    if (err) {
      /* Work around another kernel bug: port_getn() may return events even
       * on error.
       */
      if (errno == EINTR || errno == ETIME)
        saved_errno = errno;
      else
        abort();
    }

    /* Update loop->time unconditionally. It's tempting to skip the update when
     * timeout == 0 (i.e. non-blocking poll) but there is no guarantee that the
     * operating system didn't reschedule our process while in the syscall.
     */
    SAVE_ERRNO(uv__update_time(loop));

    if (events[0].portev_source == 0) {
      if (timeout == 0)
        return;

      if (timeout == -1)
        continue;

      goto update_timeout;
    }

    if (nfds == 0) {
      assert(timeout != -1);
      return;
    }

    have_signals = 0;
    nevents = 0;

    assert(loop->watchers != NULL);
    loop->watchers[loop->nwatchers] = (void*) events;
    loop->watchers[loop->nwatchers + 1] = (void*) (uintptr_t) nfds;
    for (i = 0; i < nfds; i++) {
      pe = events + i;
      fd = pe->portev_object;

      /* Skip invalidated events, see uv__platform_invalidate_fd */
      if (fd == -1)
        continue;

      assert(fd >= 0);
      assert((unsigned) fd < loop->nwatchers);

      w = loop->watchers[fd];

      /* File descriptor that we've stopped watching, ignore. */
      if (w == NULL)
        continue;

      /* Run signal watchers last.  This also affects child process watchers
       * because those are implemented in terms of signal watchers.
       */
      if (w == &loop->signal_io_watcher)
        have_signals = 1;
      else
        w->cb(loop, w, pe->portev_events);

      nevents++;

      if (w != loop->watchers[fd])
        continue;  /* Disabled by callback. */

      /* Events Ports operates in oneshot mode, rearm timer on next run. */
      if (w->pevents != 0 && QUEUE_EMPTY(&w->watcher_queue))
        QUEUE_INSERT_TAIL(&loop->watcher_queue, &w->watcher_queue);
    }

    if (have_signals != 0)
      loop->signal_io_watcher.cb(loop, &loop->signal_io_watcher, POLLIN);

    loop->watchers[loop->nwatchers] = NULL;
    loop->watchers[loop->nwatchers + 1] = NULL;

    if (have_signals != 0)
      return;  /* Event loop should cycle now so don't poll again. */

    if (nevents != 0) {
      if (nfds == ARRAY_SIZE(events) && --count != 0) {
        /* Poll for more events but don't block this time. */
        timeout = 0;
        continue;
      }
      return;
    }

    if (saved_errno == ETIME) {
      assert(timeout != -1);
      return;
    }

    if (timeout == 0)
      return;

    if (timeout == -1)
      continue;

update_timeout:
    assert(timeout > 0);

    diff = loop->time - base;
    if (diff >= (uint64_t) timeout)
      return;

    timeout -= diff;
  }
}
```

* uv__run_closing_handles

遍历等待关闭的队列，关闭 stream (包括tcp，pipe等)或者 udp 以及其他 handle，调用handle对应的close_cb。

