# 开发 Phalcon 过程中遇到的问题

## Segmentation fault

* 忘记在 `phalcon.c` 初始化类

## 创建共享内存失败，errno 13

`Failed to create shared memory (13)`

进入 `/dev/shm` 目录，查看 `shmname` 文件权限。