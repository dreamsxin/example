## 主要选项


- max_connections               100     允许客户端连接的最大数目
- fsync                         on      强制把数据同步更新到磁盘
- shared_buffers                24MB	决定有多少内存可以被PostgreSQL用于缓存数据（推荐内存的1/4)，在IO压力很大的情况下，提高该值可以减少IO
- work_mem                      1MB     使内部排序和一些复杂的查询都在这个buffer中完成，有助提高排序等操作的速度，并且减低IO
- effective_cache_size          128MB	优化器假设一个查询可以用的最大内存，和shared_buffers无关（推荐内存的1/2)，设置稍大，优化器更倾向使用索引扫描而不是顺序扫描
- maintenance_work_mem          16MB	这里定义的内存只是被VACUUM等耗费资源较多的命令调用时使用，把该值调大，能加快命令的执行
- wal_buffer                    768kB	日志缓存区的大小，可以降低IO，如果遇上比较多的并发短事务，应该和commit_delay一起用
- checkpoint_segments           3       设置wal log的最大数量数（一个log的大小为16M），默认的48M的缓存是一个严重的瓶颈，基本上都要设置为10以上
- checkpoint_completion_target	0.5     表示checkpoint的完成时间要在两个checkpoint间隔时间的N%内完成，能降低平均写入的开销
- commit_delay                  0       事务提交后，日志写到wal log上到wal_buffer写入到磁盘的时间间隔。需要配合commit_sibling，能够一次写入多个事务，减少IO，提高性能
- commit_siblings               5       设置触发commit_delay的并发事务数，根据并发事务多少来配置，减少IO，提高性能


## 参数设置例子

关闭 `fsync` 可以减少IO的负荷，但是这样就会在掉电的情况下，可能会丢失部分数据。

- fsync                         off
- shared_buffers                1GB
- work_mem                      10MB
- effective_cache_size          2GB
- maintenance_work_mem          512MB
- checkpoint_segments           32
- checkpoint_completion_target	0.9
- wal_buffer                    8MB
- commit_delay                  10
- commit_siblings               4
