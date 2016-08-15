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
SELECT jsonb_set('{"foo": [true, "bar"], "tags": {"a": 1, "b": null}}'::jsonb, '{foo}', '"test"'::jsonb)->'foo';
-- 不存在键值就不修改
SELECT jsonb_set('{"foo": [true, "bar"], "tags": {"a": 1, "b": null}}'::jsonb, '{sex}', '1'::jsonb, false)->'sex';
-- 不存在就新增键值
SELECT jsonb_set('{"foo": [true, "bar"], "tags": {"a": 1, "b": null}}'::jsonb, '{sex}', '1'::jsonb)->'sex';
```

## `to_jsonb`

```sql
SELECT to_jsonb(timestamp '2014-05-28 12:22:35.614298');
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
