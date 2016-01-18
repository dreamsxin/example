## 注意目录权限
```shell
chown postgres:postgres . -R
```
## main/pg_xlog/archive_status/  目录必须存在

# 配置 wal 日志
```conf
wal_level = archive
archive_mode = on
archive_command = 'test ! -f /mnt/server/archivedir/%f && cp /var/lib/postgresql/9.4/main/%p /mnt/server/archivedir/%f'         # command to use to archive a logfile segment
```

# 創建基礎備份
清空 /mnt/server/archivedir/ 目录
```shell
postgres=# select pg_start_backup('bakup_base');
```

# 備份整個data目錄 (tar方式)
```shell
tar cvf /var/lib/postgresql/9.4/main.tar /var/lib/postgresql/9.4/main
tar cvf main.tar main
```

# 停止備份
```shell
postgres=# select pg_stop_backup();
```

# 切換日誌
```shell
postgres=# select pg_switch_xlog();
```

# 写入测试数据

# 停止数据库
```shell
sudo service postgresql stop
```

# 使用基础备份目录替换当前目录
```shell
rm main -R
tar xvf main.tar 
```

# 编写 recovery.conf
时间点回复使用 select now() 获取的时间格式
```conf
restore_command = 'cp /mnt/server/archivedir/%f %p'
recovery_target_time = '2016-01-18 10:20:08.367751+08'
```

# 启动数据库
