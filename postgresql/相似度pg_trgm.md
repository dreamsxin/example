**表 F-23. pg_trgm 函数**

函数	返回	描述
- similarity(text, text)	real	返回一个数字指示两个参数有多相似。该结果的范围是 0（指示两个字符串完全不相似） 到 1（指示两个字符串完全一样）。
- show_trgm(text)	text[]	返回一个给定字符串中所有的 trigram 的数组（实际上除了调试很少有用）。
- show_limit()	real	返回%操作符使用的当前相似度阈值。例如， 这将两个单词之间的最小相似性设置为被认为足够相似以致彼此拼写错误。
- set_limit(real)	real	设定%操作符使用的当前相似度阈值。该阈值必须介于 0 和 1 之间 （默认为 0.3）。返回传递进来的相同值。

**表 F-24. pg_trgm 操作符**

操作符	返回	描述
- text % text	boolean	如果它的参数的相似度高于set_limit设置的当前相似度阈值， 则返回true。
- text <-> text	real	返回参数之间的"距离"，即 1 减去similarity()值。

### 类别1 : 元素重叠度相似
类似倒排，以元素重叠度为基准的相似计算。广泛应用于数组、全文检索、字符串、文本特征值、多列任意组合查询的相似搜索。

代表的PostgreSQL插件如下

1、rum
https://github.com/postgrespro/rum

2、pg_trgm
https://www.postgresql.org/docs/devel/static/pgtrgm.html

3、smlar
http://sigaev.ru/git/gitweb.cgi?p=smlar.git;a=summary

4、smlar+海明码(向量相似)
《海量数据,海明(simhash)距离高效检索(smlar) - 阿里云RDS PosgreSQL最佳实践》

5、pg_similarity
https://github.com/eulerto/pg_similarity

### 类别2 : 向量相似(类似knn距离)
向量相似与元素重叠度计算，显然是不同的，基于元素的重叠度相似，可以利用倒排来实现，如上节描述。而基于元素向量相似，需要用到自定义的索引接口，典型的代表是GiST索引在空间距离上的计算，以及imgsmlr插件在图像特征值相似方面的计算。

1、imgsmlr(图片向量相似)
https://github.com/postgrespro/imgsmlr

原理如下

64*64的图像，取16个区域的平均值，生成16个浮点数，作为图像特征值。

一个值求相似，相减绝对值最小。

2个值求相似，可以理解为平面坐标，求距离最小（GiST knn距离排序）。

3个值求相似，可以理解为3D坐标里面的点，求距离最小的点。

...

16个值求相似，与上类似。imgsmlr插件使用gist索引接口实现了16个元素的向量相似索引排序。

例子

postgres=# \d t_img  
                Table "public.t_img"  
 Column |   Type    | Collation | Nullable | Default   
--------+-----------+-----------+----------+---------  
 id     | integer   |           | not null |   
 sig    | signature |           |          |   
Indexes:  
    "t_img_pkey" PRIMARY KEY, btree (id)  
    "idx_t_img_1" gist (sig)  
数据量

postgres=# select count(*) from t_img;  
   count     
-----------  
 319964709  
(1 row)  
  
Time: 698.075 ms  
图像特征值搜索例子，速度杠杠的。(以上使用citus+postgres+128 shard)

postgres=# select * from t_img order by sig <-> '(3.539080, 0.243861, 1.509150, 1.781380, 8.677560, 4.232060, 8.979810, 1.665030, 1.294100, 4.449800, 9.200450, 1.859860, 5.440250, 7.788580, 0.514258, 8.424920)' limit 1;  
    id     |                                                                               sig                                                                                  
-----------+------------------------------------------------------------------------------------------------------------------------------------------------------------------  
 148738668 | (2.554440, 0.310499, 2.322520, 0.478624, 7.816080, 4.360440, 8.287050, 1.011060, 2.114320, 3.541110, 9.166300, 1.922250, 4.488640, 7.897890, 1.600290, 7.462080)  
(1 row)  
  
Time: 337.301 ms  
2 CUBE
https://www.postgresql.org/docs/devel/static/cube.html

a <-> b	float8	Euclidean distance between a and b.
a <#> b	float8	Taxicab (L-1 metric) distance between a and b.
a <=> b	float8	Chebyshev (L-inf metric) distance between a and b.
计算图片向量相似时，cube比imgsmlr性能稍差，因为cube使用的是float8，而imgsmlr使用的是float4。

例子

cube

postgres=# explain (analyze,verbose,timing,costs,buffers) select * from t_img0 order by sig::Text::cube <-> '(0.435404, 6.602870, 9.050220, 9.379750, 2.483920, 1.534660, 0.363753, 4.079670, 0.124681, 3.611220, 7.127460, 7.880070, 2.574830, 6.778820, 5.156320, 8.329430)' limit 1;
                                                                                                   QUERY PLAN                                                                                                   
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 Limit  (cost=0.36..0.37 rows=1 width=76) (actual time=147.432..147.434 rows=1 loops=1)
   Output: id, sig, ((((sig)::text)::cube <-> '(0.435404, 6.60287, 9.05022, 9.37975, 2.48392, 1.53466, 0.363753, 4.07967, 0.124681, 3.61122, 7.12746, 7.88007, 2.57483, 6.77882, 5.15632, 8.32943)'::cube))
   Buffers: shared hit=16032
   ->  Index Scan using idx_t_img0_1 on public.t_img0  (cost=0.36..13824.28 rows=754085 width=76) (actual time=147.430..147.430 rows=1 loops=1)
         Output: id, sig, (((sig)::text)::cube <-> '(0.435404, 6.60287, 9.05022, 9.37975, 2.48392, 1.53466, 0.363753, 4.07967, 0.124681, 3.61122, 7.12746, 7.88007, 2.57483, 6.77882, 5.15632, 8.32943)'::cube)
         Order By: (((t_img0.sig)::text)::cube <-> '(0.435404, 6.60287, 9.05022, 9.37975, 2.48392, 1.53466, 0.363753, 4.07967, 0.124681, 3.61122, 7.12746, 7.88007, 2.57483, 6.77882, 5.15632, 8.32943)'::cube)
         Buffers: shared hit=16032
 Planning Time: 0.096 ms
 Execution Time: 148.905 ms
(9 rows)
imgsmlr

postgres=# explain (analyze,verbose,timing,costs,buffers) select * from t_img0 order by sig <-> '(0.435404, 6.602870, 9.050220, 9.379750, 2.483920, 1.534660, 0.363753, 4.079670, 0.124681, 3.611220, 7.127460, 7.880070, 2.574830, 6.778820, 5.156320, 8.329430)' limit 2;
                                                                                                    QUERY PLAN                                                                                                    
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 Limit  (cost=0.36..0.37 rows=2 width=72) (actual time=40.284..48.183 rows=2 loops=1)
   Output: id, sig, ((sig <-> '(0.435404, 6.602870, 9.050220, 9.379750, 2.483920, 1.534660, 0.363753, 4.079670, 0.124681, 3.611220, 7.127460, 7.880070, 2.574830, 6.778820, 5.156320, 8.329430)'::signature))
   Buffers: shared hit=2914
   ->  Index Scan using t_img0_sig_idx on public.t_img0  (cost=0.36..7032.36 rows=754085 width=72) (actual time=40.282..48.179 rows=2 loops=1)
         Output: id, sig, (sig <-> '(0.435404, 6.602870, 9.050220, 9.379750, 2.483920, 1.534660, 0.363753, 4.079670, 0.124681, 3.611220, 7.127460, 7.880070, 2.574830, 6.778820, 5.156320, 8.329430)'::signature)
         Order By: (t_img0.sig <-> '(0.435404, 6.602870, 9.050220, 9.379750, 2.483920, 1.534660, 0.363753, 4.079670, 0.124681, 3.611220, 7.127460, 7.880070, 2.574830, 6.778820, 5.156320, 8.329430)'::signature)
         Buffers: shared hit=2914
 Planning Time: 0.091 ms
 Execution Time: 48.210 ms
(9 rows)
cube相比imgsmlr的好处是：cube可以计算任意维度的向量相似，imgsmlr则仅用于计算16维(signation类型)的向量相似```sql
CREATE EXTENSION IF NOT EXISTS pg_trgm CASCADE;

CREATE SEQUENCE tg_fingerprints_id_seq CYCLE;
DROP TABLE IF EXISTS tg_fingerprints CASCADE;
CREATE TABLE tg_fingerprints (
	id INT4 PRIMARY KEY DEFAULT nextval('tg_fingerprints_id_seq'),
	ip inet NOT NULL,  -- IP
	ua CHARACTER VARYING(1024) NOT NULL,  -- User-Agent
	fingerprints JSONB, -- 指纹信息
	created TIMESTAMPTZ DEFAULT now() NOT NULL	-- 创建时间
);

CREATE INDEX tg_fingerprints_ip_index ON tg_fingerprints USING BTREE (ip);
CREATE INDEX tg_fingerprints_ua_index ON tg_fingerprints USING BTREE (ua);
CREATE INDEX tg_fingerprints_fingerprints_index ON tg_fingerprints USING GIN (fingerprints);

-- SELECT create_hypertable('tg_fingerprints', 'created', if_not_exists => TRUE);
-- SELECT * FROM chunk_relation_size('tg_fingerprints');

COMMENT ON COLUMN public.tg_fingerprints.ip IS 'IP';
COMMENT ON COLUMN public.tg_fingerprints.ua IS 'User-Agent';

-- SELECT similarity(fingerprints::text, '{"ip": "ut exercitation aliqua esse incididxxx", "intl": "eiusmod xxx", "math": "veniam sint xxx", "audio": "nulla voluptate xxx", "fonts": "mollit voluptatexxx", "webGL": "ut xxx", "canvas": "", "screen": "voluptate esse xxx", "domRect": "Duis veniam mollit xxx", "svgRect": "eiusmod ea fugiat", "timezone": "enim ex proxxx", "userAgent": "sunt"}') FROM tg_fingerprints ORDER BY created LIMIT 100;
```

## 索引

```pgsql
CREATE EXTENSION btree_gist;
CREATE INDEX tg_fingerprints_fingerprints_trgm_idx ON tg_fingerprints USING gist (text(fingerprints) gist_trgm_ops);
```


