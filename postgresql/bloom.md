# bloom 索引

https://www.postgresql.org/docs/9.6/static/bloom.html

例：

```sql
create table test(c1 int);
insert into test select trunc(random()*100000) from generate_series(1,10000);
```

使用所有的 `test.c1` 值，通过 `bloom filter` 算法生成一个值 val。
然后给定一个值例如 100，判断 100 是否在 `test.c1` 中。

```sql
select * from test where c1=100; 
```

通过bloom filter可以快速得到，不需要遍历表来得到。可能得到的结果是 `true` 或者 `false`。
`true` 表示 100 可能在这里面，`false` 表示 100 一定不在这里面。
由于 `bloom filter` 是有损过滤器，并且真的不一定为真，但是假的一定为假。

我们可以关闭和开启位图扫描来测试性能对比：
```sql
set enable_bitmapscan=off;
set enable_bitmapscan=on;

explain (analyze, verbose, timing, costs, buffers) select * from test where c1=100;
```

## 多个字段测试

16个字段，任意测试组合，速度和recheck有关, recheck越少越好。

```sql
create table test1(c1 int, c2 int, c3 int, c4 int, c5 int, c6 int ,c7 int, c8 int, c9 int, c10 int, c11 int, c12 int, c13 int, c14 int, c15 int, c16 int);
insert into test1 select i,i+1,i-1,i+2,i-2,i+3,i-3,i+4,i-4,i+5,i-5,i+6,i-6,i+7,i-7,i+8 from (select trunc(100000000*(random())) i from generate_series(1,10000000)) t;
create index idx_test1_1 on test1 using bloom (c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15);
```

任意字段组合查询都能用上这个索引。如果使用其他的索引方法，任意条件组合查询，需要为每一种组合创建一个索引来支持。而使用bloom索引方法，只需要创建一个索引就够了。

## 实现 bloom filter 类型字段

https://www.pipelinedb.com/blog/making-postgres-bloom?spm=5176.100239.blogcont51131.10.CsARaj

```sql
CREATE TYPE dumbloom AS (
	m integer,  -- bit 位数
	k integer,  --  hash 函数数量
	bits integer[]    --  bit
);
```

创建一个空的bloom ，设置false值异常设置为TRUE的概率p， 设置期望存储多少个唯一值n 。
```sql
CREATE FUNCTION dumbloom_empty (
  -- 2% false probability
  p float8 DEFAULT 0.02,
  -- 100k expected uniques
  n integer DEFAULT 100000
) RETURNS dumbloom AS
$$
DECLARE
	m    integer;
	k    integer;
	i    integer;
	bits integer[];   
BEGIN
	-- Getting m and k from n and p is some math sorcery
	-- See: https://en.wikipedia.org/wiki/Bloom_filter#Optimal_number_of_hash_functions
	m := abs(ceil(n * ln(p) / (ln(2) ^ 2)))::integer;
	k := round(ln(2) * m / n)::integer;
	bits := NULL;

	-- Initialize all bits to 0
	FOR i in 1 .. ceil(m / 32.0) LOOP
		bits := array_append(bits, 0);
	END LOOP;

	RETURN (m, k, bits)::dumbloom;
END;
$$
LANGUAGE 'plpgsql' IMMUTABLE;
```

创建一个指纹函数，存储使用K个哈希函数得到的值，存入数组：
```sql
CREATE FUNCTION dumbloom_fingerprint (
	b    dumbloom,
	item text
) RETURNS integer[] AS 
$$
DECLARE
	h1     bigint;
	h2     bigint;
	i      integer;
	fingerprint integer[];
BEGIN
	h1 := abs(hashtext(upper(item)));
	-- If lower(item) and upper(item) are the same, h1 and h2 will be identical too,
	-- let's add some random salt
	h2 := abs(hashtext(lower(item) || 'yo dawg!'));
	finger := NULL; 

	FOR i IN 1 .. b.k LOOP
		-- This combinatorial approach works just as well as using k independent
		-- hash functions, but is obviously much faster
		-- See: http://www.eecs.harvard.edu/~kirsch/pubs/bbbf/esa06.pdf
		fingerprint := array_append(fingerprint, ((h1 + i * h2) % b.m)::integer);
	END LOOP;

	RETURN fingerprint;
END;
$$
LANGUAGE 'plpgsql' IMMUTABLE;
```

添加元素，同样也是设置对应的bit为1
```sql
CREATE FUNCTION dumbloom_add (
	b    dumbloom,
	item text,
) RETURNS dumbloom AS 
$$
DECLARE
	i    integer;
	idx  integer;
BEGIN
	IF b IS NULL THEN
		b := dumbloom_empty();  -- 生成空bloom
	END IF;

	FOREACH i IN ARRAY dumbloom_fingerprint(b, item) LOOP  -- 设置k个哈希产生的值对应的bit位为1
		-- Postgres uses 1-indexing, hence the + 1 here
		idx := i / 32 + 1;
		b.bits[idx] := b.bits[idx] | (1 << (i % 32));
	END LOOP;

	RETURN b;
END;
$$
LANGUAGE 'plpgsql' IMMUTABLE;
```

检测是否包含某元素：
```sql
CREATE FUNCTION dumbloom_contains (
	b    dumbloom,
	item text
) RETURNS boolean AS 
$$
DECLARE
	i   integer;
	idx integer;
BEGIN
	IF b IS NULL THEN
		RETURN FALSE;
	END IF;

	FOREACH i IN ARRAY dumbloom_fingerprint(b, item) LOOP  -- 计算k个哈希产生的值，判断是否有非1的bit, 有则返回false，如果全部为1则返回true. 
		idx := i / 32 + 1;
		IF NOT (b.bits[idx] & (1 << (i % 32)))::boolean THEN
			RETURN FALSE;
		END IF;
	END LOOP;

	RETURN TRUE;
END;
$$
LANGUAGE 'plpgsql' IMMUTABLE;
```

创建表：
```sql
CREATE TABLE t (
	users dumbloom
);

INSERT INTO t VALUES (dumbloom_empty());

UPDATE t SET users = dumbloom_add(users, 'usmanm');
UPDATE t SET users = dumbloom_add(users, 'billyg');
UPDATE t SET users = dumbloom_add(users, 'pipeline');

-- This first three will return true
SELECT dumbloom_contains(users, 'usmanm') FROM t;
SELECT dumbloom_contains(users, 'billyg') FROM t;
SELECT dumbloom_contains(users, 'pipeline') FROM t;
-- This will return false because we never added 'unknown' to the Bloom filter
SELECT dumbloom_contains(users, 'unknown') FROM t;
```

