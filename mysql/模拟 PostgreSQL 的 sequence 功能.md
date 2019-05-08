# 在 MySQL 中模拟 PostgreSQL 的 sequence 功能

在 id 生成器介绍 一文中, 我们介绍了几种生成 id 的方式, 其中的 last_insert_id 小节中, 简单说明了中小业务可以使用 last_insert_id 的方式来生成 id, 在 InnoDB 表的情况下容易产生死锁(下文会介绍在一个事务中操作可以降低死锁概率), 将表改成 MyISAM 也可以提高性能, 不过不利于 xtrabackup 的在线备份; 在 postgresql 序列生成器 小节中, 介绍了 postgresql 自带序列生成器的功能, 使用起来也很方便.

如果我们使用的是阿里的 MySQL 分支版本 AliSQL, 则可以直接使用内置的 Sequence 逻辑引擎, 效果类似 PostgreSQL, 不过其基于 MySQL 的 InnoDB 或 MyISAM 引擎, 这样做可以更好的兼容 xtrabackup 等备份.

不过在本文中我们只考虑的普通的 MySQL 版本, 所以我们参考了 emulating-nextval-function-to-get-sequence-in-mysql, 在 MySQL 中实现 PostgreSQL 的三个函数: nextval, setval 和 currval. 当然肯定没有 PostgreSQL 自带的函数全面、强大, 只是实现了通用的功能. 在这个实现中, 我们以 update 替换了 replace 语句. 这样做的好处是更新的时候不需要增加 next-key 锁, 只需要增加 index-record 锁, 减少了死锁发生的概率, 另外函数在事务中执行, 这样可以避免脏读等问题. 下文则详细介绍实现的过程.

## PostgrepSQL sequence 操作

在 PostgreSQL 中, 我们需要先创建一个 sequence, 然后才能使用 nextval() 等函数, 如下所示:
```text
cztest_2=> create sequence seq1;
CREATE SEQUENCE
cztest_2=> select setval('seq1', 20); 
 setval 
--------
     20
(1 row)

cztest_2=> select nextval('seq1');    
 nextval 
---------
      21
(1 row)

cztest_2=> select nextval('seq1');
 nextval 
---------
      22
(1 row)

cztest_2=> select currval('seq1');
 currval 
---------
      22
(1 row)

cztest_2=> select nextval('seq1');
 nextval 
---------
      23
(1 row)
```
在本文中我们只实现 PostgreSQL 的下面几个函数:

- nextval(regclass)                 bigint   递增序列并返回新值
- currval(regclass)                 bigint   返回最近一次用 nextval 获取的指定序列的数值
- setval(regclass, bigint)          bigint   设置序列的当前数值

## 在 MySQL 中实现

我们是以 MySQL 函数的方式实现了上述的三个功能, 虽然可以通过 select db.func() 的方式查询, 不过还是建议大家最好在每个业务的对应库中进行以下操作, 以免复制规则等引起数据不能同步到 slave.
1. 创建表结构

可以在对应的库中创建表:
```sql
CREATE TABLE `sequence_data` (
  `sequence_name` varchar(100) NOT NULL,
  `sequence_increment` int(11) unsigned NOT NULL DEFAULT '1',
  `sequence_min_value` int(11) unsigned NOT NULL DEFAULT '1',
  `sequence_max_value` bigint(20) unsigned NOT NULL DEFAULT '18446744073709551615',
  `sequence_cur_value` bigint(20) unsigned DEFAULT '0',
  `sequence_cycle` tinyint(1) NOT NULL DEFAULT '0',
  PRIMARY KEY (`sequence_name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8
```
这里的 sequence_name 对应 postgresql 中的 regclass; sequence_increment 则为自增的步长, 默认为 1; sequence_min_value 和 sequence_max_value 则为序列数的取值范围; sequence_cur_value 为当前序列的值, 也可以当做序列的起始值; sequence_cycle 则类似 postgresql 中的 CYCLE 选项, 序列达到上限后是报错还是重新开始, 默认为 0, 即报错(我们的函数认为 0 不是有效的序列, 实现中返回 NULL).

在该表中, sequence_cur_value 为无符号整形, 并没有考虑负数的情况.

* 更新数据:
```sql
-- This code will create sequence with default values.
INSERT INTO sequence.sequence_data (sequence_name) VALUE  ('sq_my_sequence');

-- You can also customize the sequence behavior.
INSERT INTO sequence.sequence_data (sequence_name, sequence_increment, sequence_max_value)VALUE  ('sq_sequence_2', 10, 100);
```

## 创建函数

### nextval 函数

创建 nextval 函数, 与上述的参考链接相比, 我们先进行了更新, 再进行查询, 所以上述的表结构 sequence_cur_value 的默认值改为了 0. 另外没有 sequence_name 的话则返回 NULL. 如下所示:

```sql
DROP FUNCTION IF EXISTS `nextval`;
DELIMITER $$

CREATE FUNCTION `nextval` (`seq_name` varchar(100))
RETURNS bigint(20) NOT DETERMINISTIC
BEGIN
    DECLARE pre_val bigint(20);
    DECLARE cur_val bigint(20);

    SELECT sequence_cur_value 
    INTO pre_val
    FROM sequence.sequence_data
    WHERE sequence_name = seq_name;

    IF pre_val IS NOT NULL THEN
        UPDATE
            sequence.sequence_data
        SET
            sequence_cur_value = IF (
                (sequence_cur_value + sequence_increment) > sequence_max_value,
                IF (
                    sequence_cycle = TRUE,
                    sequence_min_value,
                    NULL
                ),
                sequence_cur_value + sequence_increment
            )
        WHERE sequence_name = seq_name;
    ELSE
        -- seq_name does not exist
        RETURN NULL;
    END IF;

    SELECT sequence_cur_value 
    INTO cur_val
    FROM sequence.sequence_data
    WHERE sequence_name = seq_name;

    RETURN cur_val;
END$$
```

### currval 函数

currval 函数则仅返回当前时刻 sequence_cur_value 的值, 如果当前值为 0 的话则返回 NULL.

```sql
DROP FUNCTION IF EXISTS `currval`;
DELIMITER $$

CREATE FUNCTION `currval` (`seq_name` varchar(100))
RETURNS bigint(20) READS SQL DATA
BEGIN
    DECLARE cur_val bigint(20);

    SELECT sequence_cur_value 
    INTO cur_val
    FROM sequence.sequence_data
    WHERE sequence_name = seq_name;

    IF cur_val = 0 THEN
        RETURN NULL;
    END IF;
    RETURN cur_val;
END$$
```

### setval 函数

setval 函数则更新 sequence_cur_value 的值, 如果要设置起始值可以通过该函数完成. 另外我们并没有实现 PostgreSQL 中setval 函数的下面的语法， cycle 选项可以在往 sequence_data 表插入数据的时候就指定好:

- setval(regclass, bigint, boolean)

我们去掉了 cycle 选项:
```sql
DROP FUNCTION IF EXISTS `setval`;
DELIMITER $$

CREATE FUNCTION `setval` (`seq_name` varchar(100), `seq_val` bigint(20))
RETURNS bigint(20) NOT DETERMINISTIC
BEGIN
    DECLARE pre_val bigint(20);
    DECLARE cur_val bigint(20);

    -- return null if val less than 0
    IF seq_val + 0 < 0 THEN
        RETURN NULL;
    END IF;

    SELECT sequence_cur_value 
    INTO pre_val 
    FROM sequence.sequence_data
    WHERE sequence_name = seq_name;  

    IF pre_val IS NOT NULL THEN
        UPDATE sequence.sequence_data SET
        sequence_cur_value = IF (
            (sequence_cur_value + seq_val) > sequence_max_value,
            IF (
                sequence_cycle = TRUE,
                sequence_min_value,
                NULL
            ),
            seq_val
        )
        WHERE sequence_name = seq_name;
    ELSE
        -- seq_name does not exist.
        RETURN NULL;
    END IF;

    SELECT sequence_cur_value 
    INTO cur_val 
    FROM sequence.sequence_data
    WHERE sequence_name = seq_name;  

    RETURN cur_val;
END$$
```

## 如何使用

可以同上述的 PostgreSQL 进行比较:
```text
mysql root@[localhost:s3301 sequence] > select setval('sq_my_sequence', 20) as setval; 
+--------+
| setval |
+--------+
|     20 |
+--------+
1 row in set (0.01 sec)

mysql root@[localhost:s3301 sequence] > select nextval('sq_my_sequence') as nextval;            
+---------+
| nextval |
+---------+
|      21 |
+---------+
1 row in set (0.01 sec)

mysql root@[localhost:s3301 sequence] > select nextval('sq_my_sequence') as nextval;
+---------+
| nextval |
+---------+
|      22 |
+---------+
1 row in set (0.00 sec)

mysql root@[localhost:s3301 sequence] > select currval('sq_my_sequence') as currval;     
+---------+
| currval |
+---------+
|      22 |
+---------+
1 row in set (0.00 sec)

mysql root@[localhost:s3301 sequence] > select nextval('sq_my_sequence') as nextval;
+---------+
| nextval |
+---------+
|      23 |
+---------+
1 row in set (0.01 sec)
```

## 权限及复制

### 执行权限

上面的 nextval 和 setval 函数都有更新和查询操作, 如果函数创建的时候 definer 是 root 用户, 则只需要给业务用户赋予这些函数的 execute 权限即可, 更新和查询会以 definer 用户的权限操作; 如果 definer 是普通用户, 那么该用户至少要有 select, update 权限, 业务用户则需要 execute 权限;

### 复制

在主从环境中, 可以参考官方文档 stored-programs-logging 查看存储过程及函数对 binlog 的影响. 在上述的 setval 和 nextval 函数中, 由于函数更新了数据, 每次的返回值也不同, 所以我们声明了 NOT DETERMINISTIC, 这些声明需要开启参数 log_bin_trust_function_creators, 另外其中并没有使用一些不安全函数, 所以在复制格式为 statement 和 mixed 的时候, binlog 都以正常的语句显示