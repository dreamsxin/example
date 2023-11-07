# tio

See https://www.tiocloud.com/tio/docs/t-io/start/de

## TioConfig

用 tio 作 tcp 服务器，那么你需要创建 `ServerTioConfig`

### packetHandlerMode
消息包的处理模式，分为单线程和队列模式，默认 `SINGLE_THREAD` 模式。

```java
public enum PacketHandlerMode {
	/**
	 * 处理消息与解码在同一个线程中处理
	 */
	SINGLE_THREAD(1),
	/**
	 * 把packet丢到一个队列中，让线程池去处理
	 */
	QUEUE(2);
}
```

## ReadCompletionHandler
读取数据，完成时调用 `decodeRunnable`
```java
public class ReadCompletionHandler implements CompletionHandler<Integer, ByteBuffer> {
	@Override
	public void completed(Integer result, ByteBuffer byteBuffer) {
//...
			if (channelContext.sslFacadeContext == null) {
				if (tioConfig.useQueueDecode) { // 队列模式
					channelContext.decodeRunnable.addMsg(ByteBufferUtils.copy(readByteBuffer));
					channelContext.decodeRunnable.execute();
				} else {
					channelContext.decodeRunnable.setNewReceivedByteBuffer(readByteBuffer);
					channelContext.decodeRunnable.decode();
				}
			}
//...
	}
}
```

## Runnable 基类

```java
public abstract class AbstractSynRunnable implements Runnable {
//...
	/**
	 * 把本任务对象提交到线程池去执行
	 * @author tanyaowu
	 */
	public void execute() {
		executor.execute(this);
	}
//...

	@Override
	public final void run() {
	}
//...
}

public abstract class AbstractQueueRunnable<T> extends AbstractSynRunnable {
//...
	public boolean addMsg(T t) {
		if (this.isCanceled()) {
			log.error("任务已经取消");
			return false;
		}

		return getMsgQueue().add(t);
	}
//...
}
```

## DecodeRunnable

解码任务
```java
public class DecodeRunnable extends AbstractQueueRunnable<ByteBuffer> {
//...
	public void handler(Packet packet, int byteCount) {
		switch (tioConfig.packetHandlerMode) {
		case SINGLE_THREAD:
			channelContext.handlerRunnable.handler(packet);
			break;
		case QUEUE:
			channelContext.handlerRunnable.addMsg(packet);
			channelContext.handlerRunnable.execute();
			break;
		default:
			channelContext.handlerRunnable.handler(packet);
			break;
		}
	}
//...
}
```

## HandlerRunnable
消息处理
```java
public class HandlerRunnable extends AbstractQueueRunnable<Packet> {
//...

	/**
	 * 处理packet
	 * @param packet
	 * @return
	 *
	 * @author tanyaowu
	 */
	public void handler(Packet packet) {
//...
			Integer synSeq = packet.getSynSeq();
			if (synSeq != null && synSeq > 0) {
				MapWithLock<Integer, Packet> syns = tioConfig.getWaitingResps();
				Packet initPacket = syns.remove(synSeq);
				if (initPacket != null) {
					synchronized (initPacket) {
						syns.put(synSeq, packet);
						initPacket.notify();
					}
				} else {
					log.error("[{}]同步消息失败, synSeq is {}, 但是同步集合中没有对应key值", synFailCount.incrementAndGet(), synSeq);
				}
			} else {
				tioConfig.getAioHandler().handler(packet, channelContext); //wsMsgHandler.onAfterHandshaked 
			}
//...
	}
//...
}
```

### WsServerAioHandler

```java
public interface AioHandler {

	/**
	 * 根据ByteBuffer解码成业务需要的Packet对象.
	 * 如果收到的数据不全，导致解码失败，请返回null，在下次消息来时框架层会自动续上前面的收到的数据
	 * @param buffer 参与本次希望解码的ByteBuffer
	 * @param limit ByteBuffer的limit
	 * @param position ByteBuffer的position，不一定是0哦
	 * @param readableLength ByteBuffer参与本次解码的有效数据（= limit - position）
	 * @param channelContext
	 * @return
	 * @throws AioDecodeException
	 */
	Packet decode(ByteBuffer buffer, int limit, int position, int readableLength, ChannelContext channelContext) throws TioDecodeException;

	/**
	 * 编码
	 * @param packet
	 * @param tioConfig
	 * @param channelContext
	 * @return
	 * @author: tanyaowu
	 */
	ByteBuffer encode(Packet packet, TioConfig tioConfig, ChannelContext channelContext);

	/**
	 * 处理消息包
	 * @param packet
	 * @param channelContext
	 * @throws Exception
	 * @author: tanyaowu
	 */
	void handler(Packet packet, ChannelContext channelContext) throws Exception;

}

public interface ServerAioHandler extends AioHandler {

}

public class WsServerAioHandler implements ServerAioHandler {
}
```


## 服务器实例
```java
    public void start() throws Exception {
        wsServerStarter = new WsServerStarter(port, msgHandler, getTioExecutor(), getGroupExecutor());

        serverTioConfig = wsServerStarter.getServerTioConfig();
        serverTioConfig.setName(protocolName);
        serverTioConfig.setServerAioListener(new WsServerAioListener);
	serverTioConfig.setPacketHandlerMode(PacketHandlerMode.QUEUE);

        //serverTioConfig.setIpStatListener();
        //serverTioConfig.ipStats.addDurations();
        //serverTioConfig.setHeartbeatTimeout();
        wsServerStarter.start();
    }

    public static int AVAILABLE_PROCESSORS = Runtime.getRuntime().availableProcessors();

    public static ThreadPoolExecutor getGroupExecutor() {
        int CORE_POOL_SIZE_FOR_GROUP = Integer.getInteger("TIO_CORE_POOL_SIZE_FOR_GROUP", Math.min(AVAILABLE_PROCESSORS * 3, 64));
        int MAX_POOL_SIZE_FOR_GROUP = Integer.getInteger("TIO_MAX_POOL_SIZE_FOR_GROUP", Math.min(AVAILABLE_PROCESSORS * 8, 128));
        LinkedBlockingQueue<Runnable> runnableQueue = new LinkedBlockingQueue();
        String threadName = "tio-group";
        DefaultThreadFactory threadFactory = DefaultThreadFactory.getInstance(threadName, 10);
        ThreadPoolExecutor.CallerRunsPolicy callerRunsPolicy = new TioCallerRunsPolicy();
        ThreadPoolExecutor groupExecutor = new ThreadPoolExecutor(CORE_POOL_SIZE_FOR_GROUP, MAX_POOL_SIZE_FOR_GROUP, 10L, TimeUnit.SECONDS, runnableQueue, threadFactory, callerRunsPolicy);
        groupExecutor.prestartCoreThread();
        return groupExecutor;
    }

    public static SynThreadPoolExecutor getTioExecutor() {
        int CORE_POOL_SIZE_FOR_TIO = Integer.getInteger("TIO_CORE_POOL_SIZE_FOR_TIO", Math.min(AVAILABLE_PROCESSORS, 32));
        int MAX_POOL_SIZE_FOR_TIO = Integer.getInteger("TIO_MAX_POOL_SIZE_FOR_TIO", Math.min(AVAILABLE_PROCESSORS * 3, 64));
        LinkedBlockingQueue<Runnable> runnableQueue = new LinkedBlockingQueue();
        String threadName = "tio-worker";
        DefaultThreadFactory defaultThreadFactory = DefaultThreadFactory.getInstance(threadName, 10);
        SynThreadPoolExecutor.CallerRunsPolicy callerRunsPolicy = new TioCallerRunsPolicy();
        SynThreadPoolExecutor tioExecutor = new SynThreadPoolExecutor(CORE_POOL_SIZE_FOR_TIO, MAX_POOL_SIZE_FOR_TIO, 10L, runnableQueue, defaultThreadFactory, threadName, callerRunsPolicy);
        tioExecutor.prestartCoreThread();
        return tioExecutor;
    }
```
