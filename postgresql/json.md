# JSON/JSONB

```sql
CREATE TABLE public.test_json
(
   data jsonb,
   id serial,
   CONSTRAINT test_json_id_pkey PRIMARY KEY (id)
);
```

## 运算符 `->`

用来直接从 JSON 数据库获取字段值，使用文本值来标注字段的键。

```sql
SELECT '{"foo": [true, "bar"], "tags": {"a": 1, "b": null}}'::jsonb->'foo';
SELECT '{"foo": [true, "bar"], "tags": {"a": 1, "b": null}}'::jsonb->'tags'->'a';
```

使用整数作为键，可直接从存储的数组获取数据

```sql
SELECT '{"foo": [true, "bar"], "tags": {"a": 1, "b": null}}'::jsonb->'foo'->1;
```

## 运算符 `->>`

与 `->` 不同的是，该运算符返回返回纯文本，而 `->` 会返回 `json` 文本。 看下面的结果就是没有带双引号。

```sql
SELECT '{"foo": [true, "bar"], "tags": {"a": 1, "b": null}}'::jsonb->'foo'->>1;
```

## 运算符 `#>` 和 `#>>`

指定路径查询。

```sql
SELECT '{"foo": [true, "bar"], "tags": {"a": 1, "b": null}}'::jsonb#>'{foo,1}';
SELECT '{"foo": [true, "bar"], "tags": {"a": 1, "b": null}}'::jsonb#>>'{foo,1}';
```

## 运算符 `||`

```sql
-- 覆盖元素值
SELECT ('{"foo": [true, "bar"], "tags": {"a": 1, "b": null}}'::jsonb || '{"foo":"test"}'::jsonb)->'foo';
-- 新增元素值
SELECT ('{"foo": [true, "bar"], "tags": {"a": 1, "b": null}}'::jsonb || '{"sex":1}'::jsonb)->'sex';
```

## `jsonb_set` 函数

第四个参数默认为 `true`。

```sql
-- 设置文本值
SELECT jsonb_set('{"foo": [true, "bar"], "tags": {"a": 1, "b": null}}'::jsonb, '{foo}', '"test"'::jsonb)->'foo';
SELECT jsonb_set('{"foo": [true, "bar"], "tags": {"a": 1, "b": null}}'::jsonb, '{foo}', to_jsonb('test'::text))->'foo';
-- 不存在键值就不修改
SELECT jsonb_set('{"foo": [true, "bar"], "tags": {"a": 1, "b": null}}'::jsonb, '{sex}', '1'::jsonb, false)->'sex';
-- 不存在就新增键值
SELECT jsonb_set('{"foo": [true, "bar"], "tags": {"a": 1, "b": null}}'::jsonb, '{sex}', '1'::jsonb)->'sex';
```

## `to_jsonb`

```sql
SELECT to_jsonb(timestamp '2014-05-28 12:22:35.614298');
SELECT to_jsonb(123)
SELECT to_jsonb('123'::text)
```

## `jsonb_build*`

构建`jsonb`类型数据
```sql
SELECT jsonb_build_array('foo',1,'bar',2); -- ["foo",1,"bar",2]
SELECT jsonb_build_object('foo',1,'bar',2); -- {"foo": 1, "bar": 2}
```

## `jsonb_object`

```sql
-- jsonb_object(text[])
-- jsonb_object(keys text[], values text[])

json_object('{}')	-- {}
json_object('{{a, 1},{b, "def"},{c, 3.5}}')	-- {"a": "1", "b": "def", "c": "3.5"}
json_object('{a, b}', '{1,2}')			-- {"a": "1", "b": "2"}
```

## 删除元素

```sql
SELECT '{"foo": [true, "bar"], "tags": {"a": 1, "b": null}}'::jsonb - 'foo';
SELECT '["red","green","blue"]'::jsonb - 0;
-- 指定路径删除
SELECT '{"foo": [true, "bar"], "tags": {"a": 1, "b": null}}'::jsonb #- '{foo,0}';
```

## `jsonb_pretty` 函数来格式化输出

```sql
SELECT jsonb_pretty('{"foo": [true, "bar"], "tags": {"a": 1, "b": null}}'::jsonb);
```

## 更新`jsonb`字段

`jsonb_set(target jsonb, path text[], new_value jsonb[, create_missing boolean])`

```sql
INSERT INTO test_json (data) VALUES (NULL);
UPDATE test_json SET data = '{}'::jsonb;
UPDATE test_json SET data = jsonb_set(data, '{Profile}', '{"Username":"Love"}');
UPDATE test_json SET data = jsonb_set(data, '{Profile, Age}', '18');
UPDATE test_json SET data = jsonb_set(data, '{Profile, Age}', ((data#>>'{Profile, Age}')::int + 1)::text::jsonb);
SELECT * FROM test_json;
```

## 拼接键值：

```sql
SELECT concat('abcde', 2, NULL, 22);		-- abcde222
SELECT concat_ws(',', 'abcde', 2, NULL, 22);	-- abcde,2,22
SELECT 'abcde' || 2 || 22;			-- abcde222

UPDATE test_json SET data = jsonb_set(data, ('{' || concat_ws('Profile', 'Age')  || '}')::text[], '20');
SELECT * FROM test_json;
```

## `jsonb` 字段默认值

```sql
DROP SEQUENCE IF EXISTS test_data_seq CASCADE;
CREATE SEQUENCE test_data_seq;

DROP TABLE IF EXISTS test CASCADE;
CREATE TABLE test (
   id serial,
   data jsonb not null default jsonb_object(ARRAY['id'], ARRAY[nextval('test_data_seq')::text]), 
   txt text not null, 
   CONSTRAINT test_id_pkey PRIMARY KEY (id)
);

ALTER SEQUENCE test_data_seq OWNED BY test.data;

INSERT INTO test(txt) VALUES ('');

select * from test;
```

## 索引

PostgreSQL支持Json格式数据，有两种类型：json和jsonb。

json 类型可以作包含判断和是否存在的判断（containment or existence），表示符号分别为@>和?（以及其它一些变种）。对于这两种牵涉到多个键和元素的判断场景，json类型比下面要讲的arrays更适合，因为其对查询有内在的优化机制，而array只是单纯的线性查找。

jsonb 支持两种特有的GIN索引`jsonb_ops`和`jsonb_path_ops`。创建的语法如下：
```sql
CREATE INDEX idxgin ON api USING GIN (jdoc);
CREATE INDEX idxginp ON api USING GIN (jdoc jsonb_path_ops); -- 只是比前一行多了jsonb_path_ops标记
```
我们知道，GIN索引建立时，会先通过内建函数从表中每行数据的索引字段的值中，抽取键（key），一个字段值一般可抽取多个key。然后，将每个key与含有此key的所有行的ID组成键值对，再将它们插入b树索引供查询。那么这两种GIN索引有什么区别呢？

它们的区别在于，生成`GIN key`的方式不同。`jsonb_ops`调用`gin_extract_jsonb`函数生成key，这样每个字段的json数据中的所有键和值都被转成GIN的key；而jsonb_path_ops使用函数gin_extract_jsonb_path抽取：如果将一个jsonb类型的字段值看做一颗树，叶子节点为具体的值，中间节点为键，则抽取的每个键值实际上时每个从根节点到叶子节点的路径对应的hash值。

不难推测，`jsonb_path_ops`索引的key的数目和`jsonb`的叶子节点数有关，用叶子节点的路径做查询条件时会比较快（这也是这种索引唯一支持的查询方式）；而`jsonb_ops`索引的key的数目与jsonb包含的键和值（即树形结构的所有节点）的总数有关，可以用于路径查询之外的其他查询。

我们可以对json数据中的某一属性建GIN索引（可称之为属性索引），如：
`CREATE INDEX idxgintags ON api USING GIN ((jdoc -> 'tags'));`
这能提升检索键值对的效率，比如如下场景：
```sql
SELECT jdoc->'guid', jdoc->'name' FROM api WHERE jdoc -> 'tags' ? 'qui';
```
当然我们也可以不使用属性索引，而是换一种查询方式：
```sql
SELECT jdoc->'guid', jdoc->'name' FROM api WHERE jdoc @> '{"tags": ["qui"]}';
```

## 唯一索引

```sql
CREATE UNIQUE INDEX people_data_pos_idx ON peoples( (data->>'pos') ) ;
CREATE INDEX idx_btree_hobbies ON jsonTbl USING BTREE ((jsb->>'hobbies'));
CREATE INDEX idx_btree_hobbies ON jsonTbl USING HASH ((jsb->>'hobbies'));
SELECT count(*) FROM jsonTbl WHERE jsb->>'hobbies' = 'snowboarding';
SELECT count(*) FROM jsonTbl WHERE jsb->>'hobbies' = 'snowboarding' OR jsb->>'hobbies' = 'varenie';
CREATE INDEX idx_btree_hobbies ON jsonTbl USING GIN ((jsb->'hobbies'));

SELECT count(*) FROM jsonTbl WHERE jsb->'hobbies' ? 'snowboarding';
SELECT count(*) FROM jsonTbl WHERE jsb->'hobbies' ? 'snowboarding' OR jsb->'hobbies' ? 'varenie';

CREATE INDEX idx_btree_hobbies ON jsonTbl USING GIN (jsb jsonb_ops);
CREATE INDEX idx_btree_hobbies ON jsonTbl USING GIN (jsb jsonb_path_ops);

SELECT count(*) FROM jsonTbl WHERE jsb @> '{“hobbies” :“snowboarding”};
SELECT count(*) FROM jsonTbl WHERE jsb @> '{“hobbies”:”snowboarding”}' OR jsb @> '{“hobbies”:”varenie”}';
```

## 检查约束

```sql
ALTER TABLE books ADD CONSTRAINT books_doc_is_object CHECK (jsonb_typeof(doc) = 'object'); 

ALTER TABLE books ADD CONSTRAINT books_isbn_is_positive_13_digit_number CHECK ( 
	(doc->'ISBN') is not null 
	AND jsonb_typeof(doc->'ISBN') = 'number' 
	AND (doc->>'ISBN')::bigint > 0 
	AND LENGTH(((doc->>'ISBN')::bigint)::text) = 13 
); 

CREATE FUNCTION top_level_keys_ok(json_obj in jsonb) RETURNS boolean LANGUAGE plpgsql as $body$ 
	DECLARE 
	key text;
	legal_keys constant varchar(10)[] := array[ 'ISBN', 'title', 'year', 'genre', 'author', 'editors'];
	
	BEGIN
		FOR key IN ( 
			select jsonb_object_keys(json_obj)
		) 
		LOOP 
			IF not (key = any (legal_keys)) THEN 
				RETURN FALSE; 
			END IF;
		END LOOP; 
		RETURN true; 
	END;
$body$; 

ALTER TABLE books ADD CONSTRAINT books_doc_keys_OK CHECK (top_level_keys_ok(doc)); 
```

## 复杂更新

```psql
with cte as
(
    SELECT ext->'time' as time, user_id, eventid, row_number() over(PARTITION by user_id, eventid) rn FROM test t1
) 
UPDATE test t2 SET ext = ext || jsonb_set('{}', '{time}', cte.time) FROM cte WHERE cte.rn=1 AND l.user_id=cte.user_id AND l.eventid=cte.eventid and ext->>'time' is null
```
