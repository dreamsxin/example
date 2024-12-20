# 异步io之io\_uring技术详解，与epoll有什么区别？

### 一、io\_uring 概述

io\_uring 是 Linux 内核中的一种高效异步 I/O 框架，于 Linux 5.1 版本引入，旨在提高大规模并发 I/O 操作的性能。与传统的异步 I/O 接口（如 epoll、select、poll）相比，io\_uring 提供了更低的延迟和更高的吞吐量。

### 二、核心概念

**1.提交队列（Submission Queue, SQ）**：

用户空间应用程序将 I/O 请求添加到提交队列中。每个请求都会被描述为一个提交队列条目（Submission Queue Entry, SQE），包含操作类型、目标文件描述符、缓冲区等信息。

**2.完成队列（Completion Queue, CQ）**：

当 I/O 操作完成时，内核会将结果添加到完成队列中。每个结果都是一个完成队列条目（Completion Queue Entry, CQE），其中包含了操作的返回值、状态码以及用户自定义的数据。

**3.异步操作**：

`io_uring` 允许用户将 I/O 操作提交给内核，内核在后台异步处理这些操作。用户不需要等待操作完成，而是可以在稍后查询完成队列以获取操作结果。

### 三、主要系统调用

### 1\. `io_uring_setup`

**功能**：

`io_uring_setup` 是用于创建和初始化一个 `io_uring` 实例的系统调用。它分配和配置提交队列（SQ）和完成队列（CQ），并返回一个用于标识 `io_uring` 实例的文件描述符。

**依赖函数**：

+   **`io_uring_queue_init`**：`io_uring_queue_init` 是最常用的初始化函数，内部调用 `io_uring_setup` 来创建一个 `io_uring` 实例。
+   **`io_uring_queue_init_params`**：`io_uring_queue_init_params` 是一个增强的初始化函数，它允许用户传递 `io_uring_params`结构体以配置额外的参数。它也依赖于`io_uring_setup`系统调用来创建和初始化`io_uring`实例。

### 2\. `io_uring_enter`

**功能**：

`io_uring_enter` 是用于将已准备好的 I/O 操作提交给内核并处理这些操作的系统调用。它可以用于提交操作、等待操作完成，或者两者兼而有之。

**依赖函数**：

+   **`io_uring_submit`**：`io_uring_submit` 是用户提交操作到内核的函数，它在内部调用 `io_uring_enter`，将所有在提交队列中的 I/O 请求提交给内核。
+   **`io_uring_submit_and_wait`**：`io_uring_submit_and_wait` 提交 I/O 操作后，还可以等待至少一个操作完成，它也是通过调用 `io_uring_enter` 来实现这一功能。
+   **`io_uring_wait_cqe`**和**`io_uring_wait_cqe_nr`**：这些函数用于等待一个或多个操作完成，它们在内部也依赖于 `io_uring_enter`，通过传递适当的参数来等待完成队列中的事件。

### 3\. `io_uring_register`

**功能**：

`io_uring_register` 是用于将文件描述符、缓冲区或其他资源预先注册到 `io_uring` 实例中的系统调用。这可以提高操作的效率，因为内核在处理这些操作时可以直接访问预先注册的资源，而无需每次都重新设置。

**依赖函数**：

+   **`io_uring_register_buffers`**：这个函数用于注册一组内存缓冲区，使它们可以在后续的 I/O 操作中重复使用。它在内部调用 `io_uring_register` 系统调用。
+   **`io_uring_unregister_buffers`**：这个函数用于取消之前注册的缓冲区，它也依赖于 `io_uring_register` 系统调用来取消注册。
+   **`io_uring_register_files`**和**`io_uring_unregister_files`**：这些函数分别用于注册和取消注册文件描述符集合，均依赖于 `io_uring_register` 系统调用。
+   **`io_uring_register_eventfd`**和**`io_uring_unregister_eventfd`**：这些函数用于注册和取消注册一个 `eventfd`，用来通知完成事件，同样依赖于 `io_uring_register` 系统调用。

### 四、常用操作

**`io_uring_prep_*`** **系列函数**：

用于准备 I/O 操作，如 `io_uring_prep_read`、`io_uring_prep_write`、`io_uring_prep_accept`、`io_uring_prep_send` 等。这些函数将操作的细节填写到提交队列条目（SQE）中。

**`io_uring_submit`**：

将准备好的 SQE 提交给内核，触发内核执行操作，内部依赖 `io_uring_enter` 系统调用。

**`io_uring_wait_cqe`**与**`io_uring_peek_batch_cqe`**：

+   `io_uring_wait_cqe`：阻塞等待至少一个操作完成，并返回完成的 CQE。
+   `io_uring_peek_batch_cqe`：非阻塞地检查完成队列，获取已经完成的操作。

### 五、优势

**减少系统调用开销**：通过批量提交和批量获取结果，减少了系统调用的次数，降低了上下文切换的开销。

**高效的异步操作**：内核异步处理 I/O 操作，用户空间无需阻塞等待，可以在处理其他任务的同时等待操作完成。

**灵活的事件模型**：`io_uring` 支持多种 I/O 操作，并可以在不同的操作之间灵活切换，适用于网络 I/O、文件 I/O、内存映射等多种场景。

**扩展性强**：`io_uring` 支持大量并发的 I/O 操作，适合需要处理高并发连接的应用程序，如高性能服务器和数据库。

### 六、代码实践

完整代码：

```text
#include <stdio.h>
#include <liburing.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
 
#define EVENT_ACCEPT 0
#define EVENT_READ 1
#define EVENT_WRITE 2
 
struct conn_info
{
	int fd;
	int event;
};
 
int init_server(unsigned short port)
{
 
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(struct sockaddr_in));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(port);
 
	if (-1 == bind(sockfd, (struct sockaddr *)&serveraddr, sizeof(struct sockaddr)))
	{
		perror("bind");
		return -1;
	}
 
	listen(sockfd, 10);
 
	return sockfd;
}
 
#define ENTRIES_LENGTH 1024
#define BUFFER_LENGTH 1024
 
int set_event_recv(struct io_uring *ring, int sockfd,
				   void *buf, size_t len, int flags)
{
 
	struct io_uring_sqe *sqe = io_uring_get_sqe(ring);
 
	struct conn_info accept_info = {
		.fd = sockfd,
		.event = EVENT_READ,
	};
 
	io_uring_prep_recv(sqe, sockfd, buf, len, flags);
	memcpy(&sqe->user_data, &accept_info, sizeof(struct conn_info));
}
 
int set_event_send(struct io_uring *ring, int sockfd,
				   void *buf, size_t len, int flags)
{
 
	struct io_uring_sqe *sqe = io_uring_get_sqe(ring);
 
	struct conn_info accept_info = {
		.fd = sockfd,
		.event = EVENT_WRITE,
	};
 
	io_uring_prep_send(sqe, sockfd, buf, len, flags);
	memcpy(&sqe->user_data, &accept_info, sizeof(struct conn_info));
}
 
int set_event_accept(struct io_uring *ring, int sockfd, struct sockaddr *addr,
					 socklen_t *addrlen, int flags)
{
 
	struct io_uring_sqe *sqe = io_uring_get_sqe(ring);
 
	struct conn_info accept_info = {
		.fd = sockfd,
		.event = EVENT_ACCEPT,
	};
 
	io_uring_prep_accept(sqe, sockfd, (struct sockaddr *)addr, addrlen, flags);
	memcpy(&sqe->user_data, &accept_info, sizeof(struct conn_info));
}
 
int main(int argc, char *argv[])
{
 
	unsigned short port = 9999;
	int sockfd = init_server(port);
 
	struct io_uring_params params;
	memset(&params, 0, sizeof(params));
 
	struct io_uring ring;
	io_uring_queue_init_params(ENTRIES_LENGTH, &ring, &params);
 
#if 0
	struct sockaddr_in clientaddr;	
	socklen_t len = sizeof(clientaddr);
	accept(sockfd, (struct sockaddr*)&clientaddr, &len);
#else
 
	struct sockaddr_in clientaddr;
	socklen_t len = sizeof(clientaddr);
	set_event_accept(&ring, sockfd, (struct sockaddr *)&clientaddr, &len, 0);
 
#endif
 
	char buffer[BUFFER_LENGTH] = {0};
 
	while (1)
	{
 
		io_uring_submit(&ring);
 
		struct io_uring_cqe *cqe;
		io_uring_wait_cqe(&ring, &cqe);
 
		struct io_uring_cqe *cqes[128];
		int nready = io_uring_peek_batch_cqe(&ring, cqes, 128); // epoll_wait
 
		int i = 0;
		for (i = 0; i < nready; i++)
		{
 
			struct io_uring_cqe *entries = cqes[i];
			struct conn_info result;
			memcpy(&result, &entries->user_data, sizeof(struct conn_info));
 
			if (result.event == EVENT_ACCEPT)
			{
 
				set_event_accept(&ring, sockfd, (struct sockaddr *)&clientaddr, &len, 0);
				// printf("set_event_accept\n"); //
 
				int connfd = entries->res;
 
				set_event_recv(&ring, connfd, buffer, BUFFER_LENGTH, 0);
			}
			else if (result.event == EVENT_READ)
			{ //
 
				int ret = entries->res;
				// printf("set_event_recv ret: %d, %s\n", ret, buffer); //
 
				if (ret == 0)
				{
					close(result.fd);
				}
				else if (ret > 0)
				{
					set_event_send(&ring, result.fd, buffer, ret, 0);
				}
			}
			else if (result.event == EVENT_WRITE)
			{
				//
 
				int ret = entries->res;
				// printf("set_event_send ret: %d, %s\n", ret, buffer);
 
				set_event_recv(&ring, result.fd, buffer, BUFFER_LENGTH, 0);
			}
		}
 
		io_uring_cq_advance(&ring, nready);
	}
}
```

**linux网络编程视频教程**

[](https://link.zhihu.com/?target=https%3A//www.bilibili.com/video/BV1o7CRYwE5b/)

需要C/C++ Linux服务器架构师学习资料加qun**[579733396](https://link.zhihu.com/?target=https%3A//jq.qq.com/%3F_wv%3D1027%26k%3DxDVYJ870)**获取（资料包括**C/C++，Linux，golang技术，Nginx，ZeroMQ，MySQL，Redis，fastdfs，MongoDB，ZK，流媒体，CDN，P2P，K8S，Docker，TCP/IP，协程，DPDK，[ffmpeg](https://zhida.zhihu.com/search?content_id=250530080&content_type=Article&match_order=1&q=ffmpeg&zhida_source=entity)**等），免费分享

![](https://pic3.zhimg.com/v2-f5072f9bcad11b05c311896a210ba48e_1440w.jpg)

**1\. 服务器初始化**

```text
int init_server(unsigned short port)
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(struct sockaddr_in));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(port);
 
	if (-1 == bind(sockfd, (struct sockaddr *)&serveraddr, sizeof(struct sockaddr)))
	{
		perror("bind");
		return -1;
	}
 
	listen(sockfd, 10);
	return sockfd;
}
```

**解释**：

+   该函数初始化了一个 TCP 服务器[套接字](https://zhida.zhihu.com/search?content_id=250530080&content_type=Article&match_order=1&q=%E5%A5%97%E6%8E%A5%E5%AD%97&zhida_source=entity)，用于监听客户端连接请求。
+   `[socket](https://zhida.zhihu.com/search?content_id=250530080&content_type=Article&match_order=3&q=socket&zhida_source=entity)`、`bind` 和 `listen` 是常规的服务器初始化步骤，将服务器绑定到指定的端口，并使其开始监听客户端连接。

**2\. `io_uring` 环境初始化**

```text
struct io_uring_params params;
memset(&params, 0, sizeof(params));
 
struct io_uring ring;
io_uring_queue_init_params(ENTRIES_LENGTH, &ring, &params);
```

**解释**：

+   `io_uring_queue_init_params` 函数初始化了一个 `io_uring` 实例，这个实例将用于管理所有的异步 I/O 操作。
+   `ENTRIES_LENGTH` 定义了提交队列和完成队列的大小，表示可以同时处理的最大 I/O 操作数量。

**3\. 设置 `accept` 事件**

```text
struct sockaddr_in clientaddr;
socklen_t len = sizeof(clientaddr);
set_event_accept(&ring, sockfd, (struct sockaddr *)&clientaddr, &len, 0);
```

**解释**：

+   `set_event_accept` 函数将一个 `accept` 操作添加到 `io_uring` 的提交队列中。这个操作用于接受客户端连接请求。
+   这一步是服务器启动时的初始操作，它告诉 `io_uring` 开始监听并处理客户端连接。

**4\. 主循环：提交操作和处理完成事件**

```text
while (1)
{
	io_uring_submit(&ring);
	struct io_uring_cqe *cqe;
	io_uring_wait_cqe(&ring, &cqe);
 
	struct io_uring_cqe *cqes[128];
	int nready = io_uring_peek_batch_cqe(&ring, cqes, 128);
```

**解释**：

+   **`io_uring_submit`**：将之前添加到提交队列中的所有操作提交给内核，由内核异步执行这些操作。
+   **`io_uring_wait_cqe`**：等待至少一个操作完成，这是一个阻塞调用。
+   **`io_uring_peek_batch_cqe`**：批量获取已经完成的操作结果，`nready` 表示完成的操作数量。

**5\. 处理完成的事件**

```text
for (i = 0; i < nready; i++)
{
	struct io_uring_cqe *entries = cqes[i];
	struct conn_info result;
	memcpy(&result, &entries->user_data, sizeof(struct conn_info));
 
	if (result.event == EVENT_ACCEPT)
	{
		set_event_accept(&ring, sockfd, (struct sockaddr *)&clientaddr, &len, 0);
		int connfd = entries->res;
		set_event_recv(&ring, connfd, buffer, BUFFER_LENGTH, 0);
	}
	else if (result.event == EVENT_READ)
	{
		int ret = entries->res;
		if (ret == 0)
		{
			close(result.fd);
		}
		else if (ret > 0)
		{
			set_event_send(&ring, result.fd, buffer, ret, 0);
		}
	}
	else if (result.event == EVENT_WRITE)
	{
		int ret = entries->res;
		set_event_recv(&ring, result.fd, buffer, BUFFER_LENGTH, 0);
	}
}
```

**解释**：

**`EVENT_ACCEPT`**：处理 `accept` 事件。当一个新的客户端连接到来时，`io_uring` 完成队列会返回 `EVENT_ACCEPT` 事件，表示一个新的连接已经建立。此时，服务器会：

1.  重新设置 `accept` 事件，继续监听新的客户端连接。
2.  获取新连接的文件描述符 `connfd`，并设置一个 `recv` 事件来准备接收数据。

**`EVENT_READ`**：处理 `recv` 事件。当从客户端接收到数据时，`io_uring` 返回 `EVENT_READ` 事件。如果接收到的数据长度大于0，则会设置一个 `send` 事件来将数据发送回客户端。如果 `ret == 0`，说明客户端关闭了连接，则关闭文件描述符。

**`EVENT_WRITE`**：处理 `send` 事件。当数据成功发送给客户端后，`io_uring` 返回 `EVENT_WRITE` 事件。此时，服务器会再次设置一个 `recv` 事件，准备接收更多数据。

**6\. 完成队列的推进**

```text
io_uring_cq_advance(&ring, nready);
```

**解释**：

这个函数通知 `io_uring`，你已经处理完了 `nready` 个完成队列条目（CQE）。`io_uring` 可以释放这些 CQE 供后续操作使用。

### 7\. 总结

**`io_uring`** **的作用**：在这个示例中，`io_uring` 被用来高效地处理网络 I/O 操作。通过异步提交和处理 `accept`、`recv`、`send` 操作，服务器能够高效处理多个并发连接，而无需阻塞等待每个I/O操作完成。

**异步模型**：`io_uring` 提供了一种低延迟、高并发的异步 I/O 处理方式。操作在提交后由内核异步执行，完成后再由应用程序查询并处理结果。这种方式大大减少了系统调用的开销，提高了程序的并发处理能力。

**关键点**：

1.  **提交操作**：使用 `io_uring_prep_*` 函数准备操作，并提交给内核处理。
2.  **等待完成**：使用 `io_uring_wait_cqe` 等方法等待操作完成，并获取结果。
3.  **处理结果**：根据完成队列中的事件类型（如 `EVENT_ACCEPT`、`EVENT_READ`、`EVENT_WRITE`）进行相应的处理和后续操作。

### 七、问题与思考

### 1.提交队列肯定需要多线程操作，需要枷锁?

对于`io_uring`的提交队列来说，在多线程操作下**不需要**加锁。`io_uring`使用了**无锁环形队列**，通过[原子操作](https://zhida.zhihu.com/search?content_id=250530080&content_type=Article&match_order=1&q=%E5%8E%9F%E5%AD%90%E6%93%8D%E4%BD%9C&zhida_source=entity)来管理队列的头尾指针，确保多个线程可以安全且高效地并发提交I/O请求。这种设计避免了传统锁的开销，尤其适合高并发场景下的性能优化。

**无锁环形队列的原理**

**1.环形缓冲区**:

+   环形队列是一种常用的数据结构，其中队列的头和尾指针在到达缓冲区末尾时回绕到开始位置，从而形成一个“环”。
+   在`io_uring` 中，提交队列和完成队列实际上是一个环形缓冲区，允许多个线程高效地提交和消费I/O操作。

**2.原子操作**:

+   无锁设计的关键在于使用原子操作来管理队列指针，确保即使在多线程环境下，多个线程同时访问队列时也不会产生竞争条件。
+   `io_uring` 使用原子性操作（例如 `cmpxchg` 或 `fetch_add`）来更新队列的头尾指针，从而避免了锁的使用。

**3.多生产者/多消费者支持**:

`io_uring` 的设计支持多生产者（即多个线程提交I/O请求）和多消费者（即多个线程处理I/O完成事件）。在这种模式下，通过原子操作，多个线程可以无锁地同时对提交队列或完成队列进行操作。

### 2.io\_uring如何避免频繁的拷贝的？

`io_uring` 的实现旨在通过减少用户空间和内核空间之间的数据复制来提高 I/O 操作的性能。虽然`io_uring`并不是直接依赖于`mmap`来避免拷贝，但它使用了多种技术和机制来减少不必要的数据复制和系统资源消耗。

**`io_uring`** **的关键机制**

**1.提交队列和完成队列（SQ 和 CQ）**：

io\_uring 使用了两个环形队列：提交队列（Submission Queue, SQ）和完成队列（Completion Queue, CQ）。用户空间通过 SQ 提交 I/O 请求，而内核通过 CQ 返回操作的完成状态。这两个队列都可以通过 mmap 映射到用户空间，允许用户空间直接操作这些队列，减少了系统调用的频率。

**2.直接提交和批量处理**：

用户空间可以将多个 I/O 请求直接写入 SQ，然后通过一个系统调用将它们提交给内核。内核可以批量处理这些请求，并将结果写入 CQ。这减少了频繁的系统调用和数据拷贝。

**3.注册固定缓冲区和文件描述符**：

`io_uring` 允许用户提前注册缓冲区和文件描述符，这些缓冲区和文件描述符在后续的 I/O 操作中可以被重复使用。由于这些资源已经预先注册并**映射到内核**，因此在实际的 I/O 操作中不需要再次传递和复制这些资源。

**4.直接 I/O 支持**：

`io_uring` 可以与直接 I/O（Direct I/O）结合使用，使得数据可以直接从用户空间传输到存储设备或网络设备，或者从设备直接读取到用户空间，绕过内核缓冲区。这进一步减少了内核空间和用户空间之间的数据拷贝。

**5.零拷贝发送和接收**：

在网络传输中，`io_uring` 支持零拷贝发送和接收，特别是在高性能网络应用中，这意味着数据可以直接从用户空间传输到网络栈，而不需要在内核缓冲区和用户缓冲区之间进行拷贝。

**实现细节**

+   **内存映射（mmap）**：SQ 和 CQ 通常会通过 `mmap` 映射到用户空间，这样用户空间可以直接访问这些队列的数据结构。这不仅减少了系统调用的开销，也避免了在用户空间和内核空间之间的数据复制。
+   **原子操作和无锁队列**：在多线程环境下，`io_uring` 使用原子操作来管理队列的头和尾指针，从而避免了锁的使用，进一步提高了性能。
+   **批量提交与处理**：`io_uring` 支持批量提交 I/O 请求，允许多个请求在一次系统调用中被提交到内核。这种机制减少了系统调用的次数，并且通过批量处理可以减少内核空间与用户空间之间的数据交换。

**总结**

`io_uring` 通过设计高效的提交和完成队列、支持直接 I/O 和零拷贝技术，以及允许注册和重用缓冲区来避免频繁的数据复制。这种设计使得 `io_uring` 在处理大规模异步 I/O 操作时，能够提供极高的性能和低延迟。

### 3.当服务器通过 `listen` 函数开始监听端口时,程序会阻塞在哪里？

**阻塞点的分析**

在程序运行后，当服务器通过 `listen` 函数开始监听端口时，它并不会立即处理任何连接请求，而是等待客户端连接的到来。在这期间，程序的执行流会阻塞在某些地方，具体来说，它阻塞在两个可能的地方：

**1.`io_uring_wait_cqe(&ring, &cqe);`**：

这个调用是 `io_uring` 的一个重要部分，它用于等待完成队列（CQE）中至少有一个事件完成。在没有事件发生的情况下，比如没有客户端发起连接请求时，这个函数会阻塞，直到有新的事件到达为止。因此，如果没有客户端连接请求，程序会阻塞在这里。

**2.`io_uring_peek_batch_cqe(&ring, cqes, 128);`**：

这个函数用于检查 `io_uring` 完成队列中是否有已完成的事件。虽然它本身并不阻塞，但在 `io_uring_wait_cqe` 解除了阻塞之后，这个函数会获取已经完成的事件列表并处理它们。

**重点总结**

+   **阻塞点**：程序在没有客户端连接请求时，会阻塞在 **`io_uring_wait_cqe`** **函数**上，等待 `io_uring` 完成队列中的事件。
+   **异步处理**：一旦有事件完成（例如客户端连接到来），程序会解除阻塞并处理该事件，接着继续等待下一个事件的完成。

### 4.io\_Uring和epoll有什么相同点和区别

**相同点**

**1.高并发I/O处理**：两者都旨在高效处理大量并发I/O操作，特别是在网络服务器等需要处理众多客户端连接的场景中。

**2.事件驱动模型**：两者都采用事件驱动的模型，程序通过等待I/O事件发生然后进行相应处理，避免了轮询的低效性。

**3.减少阻塞**：`epoll` 和 `io_uring` 都通过异步或非阻塞的方式减少了I/O操作中的等待时间，提高了应用程序的响应速度和整体性能。

**区别**

**1.设计与用途**：

**`epoll`**：

+   `epoll` 是基于 `poll` 和 `select` 的改进版，专门用于监控多个文件描述符（socket、文件等）的事件（如读、写、异常等）。
+   它**本身并不执行I/O操作**，而是等待并通知I/O事件的发生。
+   `epoll` 适合事件驱动的网络编程，例如监视多个客户端连接的服务器。

**`io_uring`**：

+   `io_uring` 是一个更广泛的异步I/O框架，它不仅仅用于事件通知，还**直接执行I/O操作**。
+   支持文件读写、网络I/O等操作，并且设计上**避免了频繁的上下文切换和数据复制**。
+   `io_uring` 适合需要处理大量I/O操作的高性能应用，例如高吞吐量的服务器、数据库系统等。

**2.系统调用的数量与性能**：

**`epoll`**：

+   使用时需要多次系统调用。例如，你需要用 `epoll_ctl` 注册或修改事件，再用 `epoll_wait` 等待事件发生。
+   每次等待事件都需要从用户空间切换到内核空间，尽管 `epoll` 的性能比 `select` 和 `poll` 高，但频繁的系统调用仍然是一个瓶颈。

**`io_uring`**：

+   通过提交和完成队列（SQ和CQ）机制，大大减少了系统调用的数量。你可以批量提交多个I/O操作，然后一次性等待它们的完成。
+   `io_uring` 利用共享内存区域在用户空间和内核空间之间传递I/O请求和结果，减少了上下文切换和系统调用开销，性能优势明显。

**3.I/O操作类型的支持**：

**`epoll`**：

+   主要用于监听和处理文件描述符上的事件，不直接执行I/O操作。
+   你可以监控 `EPOLLIN`、`EPOLLOUT` 等事件，但具体的I/O操作仍需由用户代码完成。

**`io_uring`**：

+   不仅可以处理事件通知，还可以直接执行I/O操作（如读写文件、网络I/O）。
+   支持零拷贝传输、固定缓冲区等高级功能，适合需要高效I/O处理的复杂场景。

**4.阻塞与非阻塞**：

**`epoll`**：

+   `epoll_wait` 可以设置为阻塞或非阻塞模式，通常情况下会阻塞直到有事件发生。

**`io_uring`**：

+   `io_uring` 支持完全异步的操作，通过提交和完成队列的机制实现了非阻塞I/O。
+   可以同时处理多个I/O操作并等待它们的完成，无需像 `epoll` 那样分别等待每个事件的发生。

**开发复杂度**：

**`epoll`**：

+   相对来说更简单，只需关注文件描述符的事件注册和处理。

**`io_uring`**：

+   功能更强大，支持更多操作类型，但开发复杂度较高。需要管理提交队列和完成队列，以及处理可能的错误和资源管理。
