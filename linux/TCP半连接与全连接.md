全连接队列大小和半连接队列大小如何设置？
全连接大小：全连接队列大小取决于backlog 和somaxconn 的最小值，也就是 min(backlog,somaxconn)

somaxconn 是Linux内核参数，默认128，可通过/proc/sys/net/core/somaxconn进行配置
backlog是 listen(int sockfd,int backlog)函数中的参数backlog
半连接队列大小：通过/proc/sys/net/ipv4/tcp_max_syn_backlog来设置

```shell
#查看半连接队列
netstat -natp | grep SYN_RECV | wc -l
233 #表示半连接状态的TCP连接有233个

#查看全连接队列
ss -lnt |grep 6080
State  Recv-Q Send-Q  Local Address:Port   Peer Address:Port
LISTEN     0   100       :::6080                  :::*
# -l 显示正在Listener 的socket
# -n 不解析服务名称
# -t 只显示tcp
# Recv-Q 完成三次握手并等待服务端 accept() 的 TCP 全连接总数，
# Send-Q 全连接队列大小
```
```shell
netstat -antp|grep SYN_RECV|wc -l
ss -ant|grep SYN-RECV|wc -l
```

如何查看全连接半连接溢出？

```shell
netstat -s|egrep "SYNs to LISTEN"  #直接查看半链接队列是否有溢出，数字一直变大就是有溢出
netstat -s|grep overflowed  #直接查看全链接队列是否有溢出，数字一直变大就是有溢出

netstat -s | egrep "listen|LISTEN"

7102 SYNs to LISTEN sockets ignored #表示半连接队列溢出次数
7102 times the listen queue of a socket overflowed #全连接队列溢出的次数
```


## 全连接半连接溢出后如何处理？

当全连接队列已满就会根据tcp_abort_on_overflow策略进行处理。Linux 可通过 /proc/sys/net/ipv4/tcp_abort_on_overflow 进行配置。

当tcp_abort_on_overflow=0，服务accept 队列满了，客户端发来ack,服务端直接丢弃该ACK，此时服务端处于【syn_rcvd】的状态，客户端处于【established】的状态。在该状态下会有一个定时器重传服务端 SYN/ACK 给客户端（不超过 /proc/sys/net/ipv4/tcp_synack_retries 指定的次数，Linux下默认5）。超过后，服务器不在重传，后续也不会有任何动作。如果此时客户端发送数据过来，服务端会返回RST。（这也就是我们的异常原因了）
当tcp_abort_on_overflow=1，服务端accept队列满了，客户端发来ack，服务端直接返回RST通知client，表示废掉这个握手过程和这个连接，client会报connection reset by peer。

如果半连接队列满了，并且没有开启 tcp_syncookies，则会丢弃；

net.ipv4.tcp_syncookies = 1

表示开启SYN Cookies。当出现SYN等待队列溢出时，启用cookies来处理，可防范少量SYN攻击，默认为0，表示关闭；

tcp_syncookies 是一个开关，是否打开SYN Cookie功能，该功能可以防止部分SYN攻击。tcp_synack_retries和tcp_syn_retries定义SYN的重试次数。

 
## syn flood
Client发送SYN包给Server后挂了，
Linux下默认会进行5次重发SYN-ACK包，重试的间隔时间从1s开始，下次的重试间隔时间是前一次的双倍，5次的重试时间间隔为1s, 2s, 4s, 8s, 16s，总共31s，第5次发出后还要等32s都知道第5次也超时了，所以，总共需要 1s + 2s + 4s+ 8s+ 16s + 32s = 63s，TCP才会把断开这个连接。由于，SYN超时需要63秒，那么就给攻击者一个攻击服务器的机会，攻击者在短时间内发送大量的SYN包给Server(俗称 SYN flood 攻击)，用于耗尽Server的SYN队列。对于应对SYN 过多的问题，linux提供了几个TCP参数：tcp_syncookies、tcp_synack_retries、tcp_max_syn_backlog、tcp_abort_on_overflow 来调整应对。
 
## syncookie
Linux实现了一种称为SYNcookie的机制，通过net.ipv4.tcp_syncookies控制，设置为1表示开启。简单说SYNcookie就是将连接信息编码在ISN(initialsequencenumber)中返回给客户端，这时server不需要将半连接保存在队列中，而是利用客户端随后发来的ACK带回的ISN还原连接信息，以完成连接的建立，避免了半连接队列被攻击SYN包填满。

对于SYN半连接队列的大小是由（/proc/sys/net/ipv4/tcp_max_syn_backlog）这个内核参数控制的，有些内核似乎也受listen的backlog参数影响，取得是两个值的最小值。当这个队列满了，不开启syncookies的时候，Server会丢弃新来的SYN包，而Client端在多次重发SYN包得不到响应而返回（connection time out）错误。但是，当Server端开启了syncookies=1，那么SYN半连接队列就没有逻辑上的最大值了，并且/proc/sys/net/ipv4/tcp_max_syn_backlog设置的值也会被忽略。

## c

在系统压力很大时，遍历所有的socket不现实， 但是LISTEN socket的数量是固定的 ，你见过系统的Listener超过1万的吗？不超过1万，遍历开销就不是事儿。

```c
#include <linux/module.h>
#include <net/tcp.h>

static unsigned int dump_syn_recv(void)
{
	unsigned int num = 0;
	int i;
	struct inet_hashinfo *hashinfo = &tcp_hashinfo;

	for (i = 0; i < INET_LHTABLE_SIZE; i++) {
		struct sock *sk;
		struct hlist_nulls_node *node;
		struct inet_listen_hashbucket *ilb;

		ilb = &hashinfo->listening_hash[i];
		spin_lock_bh(&ilb->lock);
		sk_nulls_for_each(sk, node, &ilb->head) {
			struct inet_connection_sock *icsk = inet_csk(sk);
			struct listen_sock *lopt;

			read_lock_bh(&icsk->icsk_accept_queue.syn_wait_lock);
			lopt = icsk->icsk_accept_queue.listen_opt;
			if (lopt && lopt->qlen)
				num += lopt->qlen;
			read_unlock_bh(&icsk->icsk_accept_queue.syn_wait_lock);
		}
		spin_unlock_bh(&ilb->lock);
	}
	return num;
}

static ssize_t dump_read(struct file *file, char __user *ubuf, size_t count, loff_t *ppos)
{
	unsigned int num = dump_syn_recv(), n;
	char kbuf[16] = {0};

	if (*ppos != 0) {
		return 0;
	}

	n = snprintf(kbuf, 16, "%d\n", num);
	memcpy(ubuf, kbuf, n);
	*ppos += n;

	return n;
}

static struct file_operations dump_ops = {
	.owner = THIS_MODULE,
	.read = dump_read,
};

static struct proc_dir_entry *ent;
static int __init dumpstat_init(void)
{
	ent = proc_create("syn-recv-cnt", 0660, NULL, &dump_ops);
	if (!ent)
		return -1;

	return 0;
}

static void __exit dumpstat_exit(void)
{
	proc_remove(ent);
}

module_init(dumpstat_init);
module_exit(dumpstat_exit);
MODULE_LICENSE("GPL");

```
