# missing chunk number x for toast value x in pg_toast_x

## 报错原因

某张表关联的toast表的data发生损坏。

## 解决方法

1、 定位是哪张表的toast有问题：
```sql
select 2619::regclass; 
```
输出
```output
pg_statistic
```

2、 找到哪个表有问题后，先对该表做一下简单的修复：

```sql
REINDEX table pg_toast.pg_toast_2619;
REINDEX table pg_statistic;
VACUUM ANALYZE pg_statistic;
```

3、 定位该表中损坏的数据行。执行

```sql
DO $$
declare
	 v_rec record;
BEGIN	
	for v_rec in SELECT * FROM pg_statistic loop
	        raise notice 'Parameter is: %', v_rec.ctid;
		raise notice 'Parameter is: %', v_rec;
	end loop; 
END;
$$
  LANGUAGE plpgsql;
```

4、 将第3步中定位的记录删除:

```sql
delete from pg_statistic where ctid ='(50,3)';
```

5、 重复执行第3,4步，直到全部有问题的记录被清除。
6、 至此，toast问题就解决完了，解决之后，对数据库进行一次完整的维护或者索引重建。

# invalid page header in block x of relation base/x/x

## 报错原因

系统检测到磁盘页损坏，并导致postgresql数据取消当前的事务并提交一份错误报告信息。

## 解决方法

1. 关闭数据库服务器。
2. 编辑postgresql.conf文件，最后一行加入：zero_damaged_pages = on。保存文件，退出。
3. 启动数据库服务器，确认数据库服务是否恢复运行。
4. 关闭数据库服务器。
5. 编辑postgresql.conf文件，去掉最后一行zero_damaged_pages = on。保存文件，退出。
6. 重启数据库。

# missing chunk number x for toast value x in pg_toast_x

## 报错原因

主键重复，导致重建索引报错。

## 解决方法

1. 根据报错信息定位问题记录。
2. 将问题记录拷贝出，并判断正确状态的数据行。
3. 将正确的记录拷贝会表里。
4. 对数据库重建索引。


# invalid page in block x of relation base/x1/x2

依照數字可以找出database跟object
```sql
select oid,* from pg_database where oid::varchar='x1';
select oid,* from pg_class where oid::varchar='x1';
```

整個DB reindex:
```shell
reindexdb -U postgres -a
```
單一table Reindex:
```shell
psql -U postgres -c "reindex table customers"
```


ps.若是reindex時還是有大量的error，則在postgresql.conf裏最後一行加上zero_damaged_pages = on。

```sql
SET zero_damaged_pages = on;
VACUUM FULL table;
REINDEX TABLE table;
```

```shell
dd if=/dev/zero of=/xx/postgres/data/base/x1/x2 bs=8192 seek=1 count=1 conv=notrunc
```

# SQLSTATE[XX001]: Data corrupted: 7 ERROR: could not read block 0 in file "base/16385/16881": read only 0 of 8192 bytes

```sql
select relname,relkind  from pg_class where pg_relation_filepath(oid)='base/16385/16881';
```

发现是索引的类型错误，删除索引重建。