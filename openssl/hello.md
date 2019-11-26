# SSL 协议分析：ClientHello 过程分析

- sslctx_init()
初始化 SSL_Ctx 对象相关
- SSL_new(g_sslctx)
创建 SSL 对象
- SSL_set_fd(ssl,fd)
用创建的 socket 初始化 SSL 对象
- SSL_connect(ssl)
连接 SSL服务器，这里实现了 ClientHello 
- SSL_shutdown(ssl)
关闭SSL连接

![图（1）](https://github.com/dreamsxin/example/blob/master/openssl/img/clienthello.png?raw=true)

按照时间顺序，显然是客户端先发起 TLS 请求， 即ClientHello消息；
然后服务器响应客户端请求，返回 ServerHello 消息以及服务器的证书；
客户端根据服务器返回的证书验证服务器是否合法，并确定使用的TLS版本；
客户端发送交换密钥请求；
服务器响应密钥交换请求；接着就进行加密的通讯了。

- SSL_connect  ：

函数仅仅是简单的调用 SSL_do_handshake 函数，分析 SSL_do_handshake 函数。

- SSL_do_handshake：

由于可以使用不同版本的TLS协议，所以 SSL_do_handshake  最终调用 s->handshake_func(s);
使用函数指针可以根据不同版本TLS协议调用不同的握手函数。
我用 strings 搜索了对 s->handshake_func 的赋值语句，发现了赋值为 ossl_statem_connect

- ossl_statem_connect：

简单的对 state_machine(s, 0); 函数进行调用

- state_machine：

根据函数名，知道这是个协议状态机函数。根据 st->state 以及  st->hand_state 等协议状态，

进行不同的操作。由于当前是 ClientHello 协议，所以我只分析关于 ClientHello 相关代码。

根据协议跳转，最终调用 write_state_machine 函数。

- write_state_machine：

初始化一些函数指针，根据当前握手状态（ClientHello），调用 tls_construct_client_hello 函数。

- tls_construct_client_hello：

这个函数就是最底层的组包函数了。（在 statem_clnt.c 文件中）

- WPACKET_put_bytes_u16(pkt, s->client_version);
写入 2 字节的客户端版本信息

- WPACKET_memcpy(pkt, s->s3->client_random, SSL3_RANDOM_SIZE);
写入客户端随机数

- WPACKET_memcpy(pkt, session_id,sess_id_len);
如果会话ID不为0，则写入会话ID 

- ssl_cipher_list_to_bytes(s, SSL_get_ciphers(s), pkt);
写入和密码套件相关的信息，具体调用 s->method->put_cipher_by_char(c, pkt, &len) 写入密码套件信息

- ssl_allow_compression
如果有压缩信息，则写入压缩信息

- tls_construct_extensions(s, pkt, SSL_EXT_CLIENT_HELLO, NULL, 0)
最后添加 扩展信息