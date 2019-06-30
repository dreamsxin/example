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