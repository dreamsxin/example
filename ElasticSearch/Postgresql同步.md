## 方法一

https://github.com/appbaseio/abc

## 方法二

首先需要安装：

1、PostgreSQL

2、elasticsearch

3、安装multicorn:

`https://github.com/Kozea/Multicorn`

在centos里安装不需要上边链接里花里胡哨的操作，那个是在mac上安装的

4、在pg里新建extension: multicorn，在你想要优化的表所在的schema里创建：

`create extension multicorn;`

5、安装postgresql插件postgresql-elasticsearch-fdw,上边链接里的pg-es-fw不适用用es7

https://github.com/matthewfranglen/postgres-elasticsearch-fdw

6、在你想要优化的表所在的schema里创建foreign server:
```sql
CREATE SERVER multicorn_es FOREIGN DATA WRAPPER multicorn
OPTIONS (
  wrapper 'pg_es_fdw.ElasticsearchFDW'
);
```
7、如果是新表，或者你打算试试，可以先建个测试表：
```sql
CREATE TABLE articles (
    id serial PRIMARY KEY,
    title text NOT NULL,
    content text NOT NULL,
    created_at timestamp
);

然后创建对应的外表:
```sql
CREATE FOREIGN TABLE articles_es (
    id bigint,
    title text,
    content text
) SERVER multicorn_es OPTIONS (host '127.0.0.1', port '9200', node 'test', index 'articles');
```
外表对应的index就是es里你的index。外表的字段应当与你想要缓存的字段相同，涉及触发器内的操作。host，port，node，index分别对应es的连接信息，index应该在es里提前建好。

8、然后在你想要优化的表所在的schema里创建触发器:


```sql
CREATE OR REPLACE FUNCTION schema.index_article() RETURNS trigger AS $def$
    BEGIN
        INSERT INTO schema.articles_es (id, title, content) VALUES
            (NEW.id, NEW.title, NEW.content);
        RETURN NEW;
    END;
$def$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION schema.reindex_article() RETURNS trigger AS $def$
    BEGIN
        UPDATE schema.articles_es SET
            title = NEW.title,
            content = NEW.content
        WHERE id = NEW.id;
        RETURN NEW;
    END;
$def$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION schema.delete_article() RETURNS trigger AS $def$
    BEGIN
        DELETE FROM schema.articles_es a WHERE a.id = OLD.id;
        RETURN OLD;
    END;
$def$ LANGUAGE plpgsql;

CREATE TRIGGER schema.es_insert_article
    AFTER INSERT ON schema.articles
    FOR EACH ROW EXECUTE PROCEDURE schema.index_article();

CREATE TRIGGER schema.es_update_article
    AFTER UPDATE OF title, content ON schema.articles
    FOR EACH ROW
    WHEN (OLD.* IS DISTINCT FROM NEW.*)
    EXECUTE PROCEDURE schema.reindex_article();

CREATE TRIGGER schema.es_delete_article
    BEFORE DELETE ON schema.articles
    FOR EACH ROW EXECUTE PROCEDURE schema.delete_article();
```
9、然后在es里创建你的索引：
curl -H "Content-Type:application/json" -X PUT 'http://localhost:9200/articles/' -d '{}'

10、然后就是相关增删改查的测试操作了，以下不再赘述，记得把之前的表数据往es里传一份。以后读写es的表就可以了

我在其中遇到的一些问题：
python报错：’hit‘
就介仨字母。反映在运行的sql语句里。说明没有从es取到数据
咋办呢？安装postgresql-elasticsearch-fdw以适应es7,es最坑的就是版本。

查询es返回404:
es索引建错了 curl一下索引地址看看。

对这个方法的疑问：
触发器机制的可靠性和性能

发现的问题：当es崩掉的时候，所有的查询都会直接报错，

## 方法三

自己写代码分别操作postgresql和es
