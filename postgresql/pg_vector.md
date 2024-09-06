https://github.com/pgvector/pgvector.git

##
```psql
CREATE EXTENSION IF NOT EXISTS vector;
CREATE TABLE items (id bigserial PRIMARY KEY, name, features vector(3));
INSERT INTO items (features) VALUES ('[1,2,3]'), ('[4,5,6]');
INSERT INTO items (id, features) VALUES (1, '[1,2,3]'), (2, '[4,5,6]') ON CONFLICT (id) DO UPDATE SET features = EXCLUDED.features;
```
**查询运算符**
- <->：该运算符计算两个向量之间的欧几里德距离。欧几里德距离是多维空间中向量表示的点之间的直线距离。较小的欧几里德距离表示向量之间的相似性较大，因此该运算符在查找和排序相似项目时非常有用。
```psql
SELECT id, name, features, features <-> '[0.45, 0.4, 0.85]' as distance
2FROM items
3ORDER BY features <-> '[0.45, 0.4, 0.85]';
```
- <=>：该运算符计算两个向量之间的余弦相似度。余弦相似度比较两个向量的方向而不是它们的大小。余弦相似度的范围在 -1 到 1 之间，1 表示向量相同，0 表示无关，-1 表示向量指向相反方向。
```psql
SELECT id, name, features, features <=> '[0.45, 0.4, 0.85]' as similarity
2FROM items
3ORDER BY features <=> '[0.45, 0.4, 0.85]' DESC;
```
<+>
- <#>：该运算符计算两个向量之间的曼哈顿距离（也称为 L1 距离或城市街区距离）。曼哈顿距离是每个维度对应坐标差的绝对值之和。相对于欧几里德距离而言，曼哈顿距离更加强调沿着维度的较小移动。
```psql
SELECT id, name, features, features <#> '[0.45, 0.4, 0.85]' as distance
2FROM items
3ORDER BY features <#> '[0.45, 0.4, 0.85]';p
```
## 索引
为要使用的每个距离函数添加一个索引。
**L2距离**
```psql
CREATE INDEX ON items USING ivfflat (embedding vector_l2_ops) WITH (lists = 100);
```
**L1距离**
```psql
create index on items using hnsw (embedding vector_l1_ops);
```
**汉明距离**
```psql
create index on items using hnsw (embedding bit_hamming_ops);
```
**内积距离**
```psql
CREATE INDEX ON items USING ivfflat (embedding vector_ip_ops) WITH (lists = 100);
```
**余弦距离**
```psql
CREATE INDEX ON items USING ivfflat (embedding vector_cosine_ops) WITH (lists = 100);
```
**杰卡德距离**
```psql
create index on vector using hnsw (vector bit_jaccard_ops);
```

## 核心概念及原理
嵌入向量（Embedding Vectors）
向量 Embedding 是在自然语言处理和机器学习中广泛使用的概念。各种文本、图片或其他信号，均可通过一些算法转换为向量化的 Embedding。在向量空间中，相似的词语或信号距离更近，可以用这种性质来表示词语或信号之间的关系和相似性。例如，通过一定的向量化模型算法，将如下三句话，转换成二维向量（x，y），我们可通过坐标系来画出这些向量的位置，它们在二维坐标中的远近，就显示了其相似性，坐标位置越接近，其内容就越相似。

接下来将介绍如何利用云数据库 PostgreSQL 版提供的 pg_vector 插件构建用于向量高效存储、检索的向量数据库。

