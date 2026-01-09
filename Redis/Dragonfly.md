# Dragonfly

混合存储（Hybrid Storage）功能目前仍处于**实验性（experimental）阶段**，其设计和实现正在快速迭代中。因此，**强烈不建议直接在生产环境中使用**，但非常适合用于技术验证和测试。

- https://github.com/dragonflydb/dragonfly/blob/main/README.zh-CN.md
- https://www.dragonflydb.io/blog/a-preview-of-dragonfly-ssd-tiering

```bash
# 启用分层存储并配置参数
./dragonfly --tiered_storage_enabled=true \
            --tiered_storage_path=/data/tiered \
            --tiered_offload_threshold=0.8 \
            --tiered_upload_threshold=0.6 \
            --maxmemory=16gb
```

```shell
# 查看分层存储统计信息
curl http://localhost:6379/metrics | grep tiered
 
# 输出示例：
tiered_storage_cool_memory_used 524288000
tiered_storage_offloaded_items 12500
tiered_storage_read_hits 98000
tiered_storage_read_misses 1200
```

```shell
./dragonfly --tiered_storage_enabled=true \
            --tiered_storage_path=/data/order_storage \
            --tiered_offload_threshold=0.75 \
            --tiered_upload_threshold=0.55 \
            --tiered_min_value_size=256 \
            --maxmemory=32gb \
            --cache_mode=true
```
