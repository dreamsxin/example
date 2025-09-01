## 建立 person 的索引（类似表）
```txt
"person" : {
    "aliases" : { },
    "mappings" : {
      "properties" : {
        "address" : {
          "type" : "text",
          "fields" : {
            "keyword" : {
              "type" : "keyword",
              "ignore_above" : 256
            }
          }
        },
        "age" : {
          "type" : "long"
        },
        "name" : {
          "type" : "text",
          "fields" : {
            "keyword" : {
              "type" : "keyword",
              "ignore_above" : 256
            }
          }
        }
      }
    }
```

## _source
就相当于mysql数据库中的【select 字段1,字段2】形式。
- _source=false，表示所有字段都不返回。
- _source: ["username", "age"]，只返回指定的字段。
```txt
# 查询数据，不返回字段
GET /idx_20221124/_search
{
  "query": {
    "match": {
      "age": 20
    }
  },
  "_source": false
}
 
# 查询数据，返回指定字段
GET /idx_20221124/_search
{
  "query": {
    "match": {
      "age": 20
    }
  },
  "_source": ["username", "age"]
}
```

### match
相当于mysql数据库中的like查询，match查询的字段如果是text类型，那么text会被分词，match就会匹配分词，查询所有包含分词的doc文档，如果不是text类型的，那就是精确查询。
- match：查询指定条件的数据，match会将查询的条件进行分词操作，然后只有doc文档中包含分词，就都会查询出来。
- match_all：查询所有数据。
- match_phrase：匹配短语，match是会查询所有包含分词的doc文档，而match_phrase则是匹配整个短语，才会返回对应的doc文档。
- match_phrase_prefix：匹配短语的前缀部分，这个只能使用在text类型字段。

### must
和mysql数据库中的【and】是相同作用，【must】接收一个数组，数组中的所有条件都成立，这个时候才会查询对应数据。
```txt
# 查询数据
GET /idx_20221124/_search
{
  "query": {
    "bool": {
      "must": [
        {
          "match": {
            "nickname": "java"
          }
        },
        {
          "match": {
            "age": 25
          }
        }
      ]
    }
  }
```
### must_not

### should
相当于mysql数据库中的【or】连接符。
```txt
# 查询数据
GET /idx_20221124/_search
{
  "query": {
    "bool": {
      "should": [
        {
          "match": {
            "nickname": "java"
          }
        },
        {
          "match": {
            "age": 31
          }
        }
      ]
    }
  }
}
```
### filter
在filter里面可以使用多种查询条件，例如：range、term、terms、exists、ids几种常见的查询
```txt

# 查询数据
GET /idx_20221124/_search
{
  "query": {
    "bool": {
      "filter": [
        {
          "range": {
            "age": {
              "gt": 20,
              "lt": 30
            }
          }
        }
      ]
    }
  }
}
```
## 基本查询操作


### 查询所有数据并进行排序
```txt
GET person/_search
{
  "query":{
    "match_all": {}
  },
  "sort":{
    "age":"desc"
  }
}
```

### 分页查询
```txt
GET person/_search
{
  "query":{
    "match_all": {}
  },
  "sort":{
    "age":"desc"
  },
  "from":1,
  "size":1
}
```

### 全文搜索（分词）
```txt
GET person/_search
{
  "query": {
    "match": {
      "name": "冯"
    }
  }

```
### 短语匹配
```txt
GET person/_search
{
  "query":{
    "match_phrase": {
      "address": "疾风"
    }
  }
}
```
### bool 查询
```txt
GET /blogs_index/_search
{
  "query": {
    "bool": {
      "filter": [
        {
          "term": {
            "author": "方才兄"
          }
        },
        {
          "term": {
            "tag": "2"
          }
        },
        {
          "match": {
            "title": "es"
          }
        }
      ]
    }
  }
}
```

### 多条件 bool 查询
```txt
GET person/_search
{
  "query":{
    "bool":{
      "must":[
        {"match":{"age":"23"}}
        ],
      "must_not": [
        {"match": {
          "address": "海南"
        }}
      ]
    }
  }
}
```

### minimum_should_match
最小匹配度
```josn
GET /_search
{
  "size": 20,
  "query": {
    "bool": {
      "minimum_should_match": 1,
      "should": [
        {
          "bool": {
            "must": [
              {
                "term": {
                  "_index": {
                    "value": "索引一"
                  }
                }
              },
              {
                "term": {
                  "索引一字段": {
                    "value": "少年歌行"
                  }
                }
              }
            ]
          }
        },
        {
          "bool": {
            "must": [
              {
                "term": {
                  "_index": {
                    "value": "索引二"
                  }
                }
              }
            ],
            "filter": {
              "term": {
                "索引而字段": "少年歌行"
              }
            }
          }
        }
      ]
    }
  },
  "sort": [
    {
      "_index": {
        "order": "desc"
      }
    },
    {
      "_score": {
        "order": "desc"
      }
    }
  ]
}
```

### query_string
```txt
GET person/_search
{
  "query": {
    "query_string": {
      "default_field": "address",  
      "query": " 疾风 OR 达州"
    }
  }

GET person/_search
{
  "query": {
    "query_string": {
      "default_field": "address",
      "query": "疾风 AND 归途"
    }
  }
}
```
## term 查询
```txt
GET person/_search
{
  "query": {
    "ids":{
      "values":[1,2,3]
    }
  }
}
```
### 前缀查询
```txt
GET person/_search
{
  "query": {
    "prefix": {
      "name": {
        "value": "冯"
      }
    }
  }
}
```

## 分词匹配查询
```txt
GET person/_search
{
  "query": {
    "term": {
      "name": {
        "value": "老"
      }
    }
  }
}

GET person/_search
{
  "query": {
    "terms": {
      "name": [
        "冯",
        "陈"
      ]
    }
  }
}
```

### 通配符

```txt
GET person/_search
{
  "query": {
    "wildcard": {
      "name": {
        "value": "冯*"
      }
    }
  }
}
```

### 范围查询
```txt
GET person/_search
{
  "query": {
    "range": {
      "age": {
        "gte": 20,
        "lte": 30
      }
    }
  }
}
```

### 正则：regexp
```txt
get person/_search
{
  "query":{
    "regexp": {
      "name": "冯*"
    }
  }
}
```

### 模糊匹配：fuzzy
```txt
get person/_search
{
  "query":{
    "fuzzy": {
      "address": {
        "value": "疾"
      }
    }
  }
}
```

## 标准聚合

```shell
POST /test-agg-cars/_bulk
{ "index": {}}
{ "price" : 10000, "color" : "red", "make" : "honda", "sold" : "2014-10-28" }
{ "index": {}}
{ "price" : 20000, "color" : "red", "make" : "honda", "sold" : "2014-11-05" }
{ "index": {}}
{ "price" : 30000, "color" : "green", "make" : "ford", "sold" : "2014-05-18" }
{ "index": {}}
{ "price" : 15000, "color" : "blue", "make" : "toyota", "sold" : "2014-07-02" }
{ "index": {}}
{ "price" : 12000, "color" : "green", "make" : "toyota", "sold" : "2014-08-19" }
{ "index": {}}
{ "price" : 20000, "color" : "red", "make" : "honda", "sold" : "2014-11-05" }
{ "index": {}}
{ "price" : 80000, "color" : "red", "make" : "bmw", "sold" : "2014-01-01" }
{ "index": {}}
{ "price" : 25000, "color" : "blue", "make" : "ford", "sold" : "2014-02-12" }
```

```txt
GET test-agg-cars/_search
{
  "size":0,    							//siz指定为0，hits不会返回搜索结果
  "aggs": {     						//聚合查询
    "pop_colors": {         //为聚合查询的结果指定一个想要的名称
      "terms": { 						//定义桶的类型为terms（桶：满足特定条件的文档集合）
        "field": "color.keyword"   //每个桶的key都与color字段里找到的唯一词对应
      }
    }
  }
}
```

### 多个聚合
```txt
GET /test-agg-cars/_search
{
  "size": 0,
  "aggs":{
    "pop_colors":{
      "terms": {
        "field": "color.keyword"
      }
    },
    "make_by":{
      "terms": {
        "field": "make.keyword"
      }
    }
  }
}
```
### 聚合嵌套
```txt
GET test-agg-cars/_search
{
  "size": 0,
  "aggs": {
    "colors": {
      "terms": {
        "field": "color.keyword"
      },
    "aggs":{
      "avg_price":{
        "avg": {
          "field": "price"
        }
      }
    }
  }
}
}
```

### 前置过滤条件：filter
```txt
GET test-agg-cars/_search
{
  "size": 0,
  "aggs":{
    "make_by":{
      "filter": {
        "term": {
          "make": "honda"
        }
      },
      "aggs": {
        "avg_price": {
          "avg": {
            "field": "price"
          }
        }
      }
    }
  }
}
```

### 对number类型聚合：range
```txt
{
  "size":0,
  "aggs": {
    "price_ranges": {
      "range": {
        "field": "price",
        "ranges": [
          {
            "from": 10000,
            "to": 15000
          }
        ]
      }
    }
  }
}
```

### 对日期类型聚合
```txt
GET test-agg-cars/_search
{
  "size":0,
  "aggs": {
    "range": {
      "date_range": {
        "field": "sold",
        "ranges": [
          {
            "from": "2014-10-28",
            "to": "2014-11-05"
          }
        ]
      }
    }
  }
}
```

## 聚合查询之metric聚合
metric聚合从分类上来看，可以分为单值分析和多值分析。

单值分析就是只输出一个分析结果，标准的stat型。

1. avg 平均值
2. max 最大值
3. min 最小值
4. sum 和
5. value_count 数量

其他类型 cardinality记述(distinct去重)，weighted_avg 带权重的avg。。。

## 多值分析

省略，因为我觉得不是很常用，到时候会查文档就行。

### 单值分析

avg平均值：计算平均值
```txt
GET person/_search
{
  "size": 0, 
  "aggs":{
    "avg_age":{
      "avg": {
        "field": "age"
      }
    }
  }
}
//返回结果
 "aggregations" : {
    "avg_age" : {
      "value" : 30.857142857142858
    }
  }
```
max最大值：
```txt
GET person/_search
{
  "size":0,
  "aggs": {
    "max_age": {
      "max": {
        "field": "age"
      }
    }
  }
}
//返回结果
 "aggregations" : {
    "max_age" : {
      "value" : 40.0
 }
```
min最小值：与最大值类似

sum求和：
```txt
GET person/_search
{
  "size":0,
  "aggs": {
    "sum_age": {
      "sum": {
        "field": "age"
      }
    }
  }
}
//返回结果
  "aggregations" : {
    "sum_age" : {
      "value" : 216.0
    }
  }
Value_count数量

GET person/_search?size=0
{
  "aggs": {
    "name_count": {
      "value_count": {
        "field": "age"
      }
    }
  }
}
//返回结果
  "aggregations" : {
    "sum_age" : {
      "value" : 216.0
    }
  }
```
目前就简单介绍了这些查询，其实在es官网可以看到很多不同的查询，包括管道啥之类的，但是我们以后使用的时候要知道大致的查询分为哪几类，然后每种查询得能在官网快速定位，然后通过例子学会使用并理解。

##

好的，除了 nested 和 must，Elasticsearch 提供了极其丰富的查询参数（更准确地称为“查询子句”或“查询类型”）来满足各种搜索需求。下面我将它们分类进行解析，并附上示例。

一、布尔查询（Bool Query）的其他核心子句

布尔查询是组合多个查询条件的强大工具，除了 must，它还有以下几个关键子句：

1. filter
   · 作用：包含必须满足的条件，但它最重要的特点是不参与评分（_score），查询结果会被自动缓存，性能极高。
   · 场景：用于过滤结构化数据（时间范围、状态、标签等），你只关心是否匹配，不关心匹配度（相关度）。
   · 示例：查找所有包含“手机”且在2023年之后发布的产品。
   ```json
   {
     "query": {
       "bool": {
         "must": [
           { "match": { "title": "手机" } }
         ],
         "filter": [
           { "range": { "publish_date": { "gte": "2023-01-01" } } }
         ]
       }
     }
   }
   ```
2. should
   · 作用：表示“应该满足”的条件，用于匹配一个或多个条件。它会影响相关性评分，满足的条件越多，分数越高。
   · 与 minimum_should_match 配合：可以指定至少需要满足多少个 should 条件。
   · 场景：实现“或”的逻辑，或者用于推荐和提升相关度。
   · 示例：查找标题中包含“苹果”或者“水果”的商品。
   ```json
   {
     "query": {
       "bool": {
         "should": [
           { "match": { "title": "苹果" } },
           { "match": { "title": "水果" } }
         ],
         "minimum_should_match": 1 // 默认情况下，如果没有 must 或 filter，至少需匹配1个
       }
     }
   }
   ```
3. must_not
   · 作用：表示必须不满足的条件。和 filter 一样，它也不参与评分，结果会被缓存。
   · 场景：排除不需要的文档。
   · 示例：查找所有包含“手机”但不属于“苹果”品牌的产品。
   ```json
   {
     "query": {
       "bool": {
         "must": [
           { "match": { "title": "手机" } }
         ],
         "must_not": [
           { "term": { "brand.keyword": "Apple" } }
         ]
       }
     }
   }
   ```

---

二、常用全文检索查询（Full-Text Queries）

1. match (最常用的全文检索查询)
   · 作用：对文本进行标准分词处理（如转为小写、分词），然后构建查询。
   · 场景：处理用户搜索框的输入，是模糊、智能搜索的首选。
   · 示例：{ "match": { "content": "Elasticsearch learning" } } 会先对输入进行分词，变成 elasticsearch 和 learning，然后去匹配包含这两个词（或其一）的文档。
2. match_phrase
   · 作用：不仅要求分词后的词条都出现，还要求它们出现的顺序一致且紧邻。
   · 场景：搜索精确的短语。
   · 示例：{ "match_phrase": { "content": "quick brown fox" } } 能匹配 "the quick brown fox"，但不能匹配 "quick fox brown"。
3. match_phrase_prefix
   · 作用：类似 match_phrase，但允许最后一个词条进行前缀匹配（即通配符搜索）。
   · 场景：用于实现搜索时的自动补全（As-You-Type）。
   · 示例：{ "match_phrase_prefix": { "title": "quick brown f" } } 可以匹配到以 "quick brown f" 开头的短语，如 "quick brown fox"。
4. multi_match
   · 作用：在多个字段上执行相同的 match 查询。
   · 场景：不确定用户输入的内容属于哪个字段时，在多个字段中同时搜索。
   · 示例：在 title 和 content 字段中搜索同一个关键词。
   ```json
   {
     "multi_match": {
       "query": "搜索引擎",
       "fields": ["title", "content"]
     }
   }
   ```

---

三、常用精确值查询（Term-Level Queries）

这些查询用于未经分词的精确值（如关键字、数字、日期等），不参与评分。

1. term
   · 作用：精确匹配一个值，直接倒排索引查找。
   · 场景：匹配状态、标签、ID等关键字字段（通常为 .keyword 子字段）。
   · 示例：{ "term": { "status.keyword": "published" } }
2. terms
   · 作用：匹配多个值中的任何一个，相当于 SQL 中的 IN。
   · 场景：一次匹配多个可能的值。
   · 示例：{ "terms": { "tags.keyword": ["科技", "数码", "手机"] } }
3. range
   · 作用：匹配某个范围内的值。
   · 参数：gt (大于), gte (大于等于), lt (小于), lte (小于等于)。
   · 场景：查询价格、日期等范围。
   · 示例：查询价格在 1000 到 2000 之间的商品。
   ```json
   {
     "range": {
       "price": {
         "gte": 1000,
         "lte": 2000
       }
     }
   }
   ```
4. exists
   · 作用：匹配那些包含指定字段的文档（字段值不为 null 或 []）。
   · 场景：过滤掉缺失某个字段的文档。
   · 示例：{ "exists": { "field": "description" } }
5. prefix / wildcard / regexp
   · 作用：前缀匹配、通配符匹配（* 和 ?）、正则表达式匹配。性能开销较大，尽量避免对大数据集使用。
   · 示例（prefix）：{ "prefix": { "user_id.keyword": "usr-" } }

---

四、其他特殊查询

1. query_string
   · 作用：支持复杂的Lucene查询语法，允许使用 AND/OR/NOT、分组、通配符等。功能强大但非常危险，容易引发语法错误和慢查询，通常不推荐使用，除非你非常清楚你在做什么。
2. has_child & has_parent
   · 作用：用于处理父子文档关系（Join类型），分别查询满足条件的子文档或父文档。
3. geo_distance & geo_bounding_box
   · 作用：用于地理空间查询，根据经纬度查找一定距离内或某个矩形区域内的点。

总结与选择建议

查询类型 典型场景 是否分词 是否影响评分
match 全文搜索，用户输入 是 是
term 精确值匹配（关键字、枚举） 否 否
bool must/should 组合复杂逻辑 - must/should 是
bool filter/must_not 过滤、排除 - 否（性能佳）
range 范围查询 否 否
nested 查询嵌套对象数组 - 是
exists 判断字段是否存在 否 否

核心选择思路：

1. 全文检索（文本内容搜索）用 match。
2. 精确匹配（状态、标签、ID）用 term（配合 .keyword）。
3. 组合多个条件用 bool，并根据是否需要评分选择 must（要评分）或 filter（不要评分，性能最优）。
4. 查询数组对象用 nested。
5. 过滤范围、日期用 range，放在 filter 里。

好的，我们来详细解析 Elasticsearch 中 nested 查询和 must 子句，这是构建复杂查询时非常重要的两个概念。

1. must - 布尔查询的逻辑核心

归属： bool 查询

作用： must 是布尔查询 (bool query) 中的一个子句。它表示“必须满足”的条件，逻辑上等同于“AND”。所有在 must 数组中的查询条件都必须出现在匹配的文档中。它会影响文档的评分 (_score)，满足的条件的越多，分数通常越高。

结构：

```json
{
  "query": {
    "bool": {
      "must": [
        { 某个查询条件 },
        { 另一个查询条件 }
      ]
    }
  }
}
```

示例： 查找title 中包含 “手机” 并且 price 等于 2999 的所有商品。

```json
GET /products/_search
{
  "query": {
    "bool": {
      "must": [
        { "match": { "title": "手机" } },
        { "term": { "price": 2999 } }
      ]
    }
  }
}
```

关键点：

· must 关乎的是整个文档的匹配逻辑。
· 它可以包含任何类型的查询（match, term, range, 甚至另一个 bool 查询等）。

---

2. nested - 处理嵌套对象的特殊查询

归属： 一种独立的查询类型

作用： 用于查询嵌套类型 (nested) 的字段。这是为了解决对象数组在 Elasticsearch 中默认的扁平化处理（Inner objects are flattened）所带来的问题。

为什么需要 nested 查询？ 假设你有一个users 索引，其中每个用户有一个 addresses 字段，是对象数组（城市、邮编）：

```json
{
  "user": "张三",
  "addresses": [
    { "city": "北京", "zipcode": "100001" },
    { "city": "上海", "zipcode": "200002" }
  ]
}
```

默认情况下，ES 会将其扁平化为： { “user”: “张三”, “addresses.city”: [“北京”, “上海”], “addresses.zipcode”: [“100001”, “200002”] }

这时，如果你想查询 “城市是北京并且邮编是200002” 的人，这个文档错误地也会被匹配到。因为扁平化后，北京和100001关联，上海和200002关联，但查询条件会交叉匹配：“北京” in addresses.city AND “200002” in addresses.zipcode -> true。

nested 类型的解决方案： 将addresses 字段定义为 nested 类型后，ES 会将数组中的每个对象作为一个独立的隐藏文档来存储和维护，从而保持对象内部的独立性。

nested 查询结构： nested查询允许你像查询独立文档一样，去查询这些嵌套对象。

```json
{
  "query": {
    "nested": {
      "path": "要查询的嵌套字段名",
      "query": {
        // 具体的查询条件，这个条件是在每个嵌套对象上独立执行的
      }
    }
  }
}
```

示例： 正确查询addresses 中 同时满足 city 为 “北京” 且 zipcode 为 “100001” 的用户。

```json
GET /users/_search
{
  "query": {
    "nested": {
      "path": "addresses",
      "query": {
        "bool": {
          "must": [
            { "match": { "addresses.city": "北京" } },
            { "term": { "addresses.zipcode": "100001" } }
          ]
        }
      }
    }
  }
}
```

这个查询不会匹配到用户“张三”，因为在他的地址数组中，没有一个单独的地址对象同时满足这两个条件。

关键点：

· nested 查询只针对定义为 nested 类型的字段。
· 它的 query 参数内部的逻辑是在同一个嵌套对象上生效的。
· 它本质上是一个独立的查询类型，可以放在任何允许放查询的地方，当然也包括 bool 查询的 must 子句里。

---

3. 组合使用：must 里的 nested 查询

这是最常见的场景：在复杂的布尔逻辑中，需要对嵌套字段进行精确查询。

需求： 搜索所有满足以下条件的用户：

1. 用户名为 “张三” (整个文档的条件)
2. 并且有一个地址在 “北京” (嵌套对象的条件)

查询DSL：

```json
GET /users/_search
{
  "query": {
    "bool": {
      "must": [ 
        {
          "match": {
            "user": "张三" // 条件1：针对整个文档
          }
        },
        {
          "nested": { // 条件2：针对嵌套字段
            "path": "addresses",
            "query": {
              "match": {
                "addresses.city": "北京"
              }
            }
          }
        }
      ]
    }
  }
}
```

解析：

· 外层的 bool must 包含了两个条件，它们必须同时满足。
· 第一个条件是普通的 match 查询，在顶级字段 user 上执行。
· 第二个条件是一个 nested 查询，它告诉 ES：“请在 addresses 这个嵌套字段的数组里，至少找到一个对象满足 {addresses.city: ‘北京’} 这个条件”。
· 只有同时满足“用户是张三”和“有一个北京地址”的文档才会被返回。

总结对比

特性 must nested
类型 子句，属于 bool 查询 独立的查询类型
核心作用 定义逻辑与（AND） 关系 查询嵌套类型（nested） 的字段
操作对象 整个文档的字段 文档中某个嵌套数组里的单个对象
常见用法 在 bool 查询中组合多个条件 在需要精确查询对象数组时使用，常作为其他查询（如bool）的一部分
关系 must 内部可以包含 nested 查询 nested 查询 内部也可以包含 bool must 来组合嵌套对象自身的条件

简单来说：must 负责逻辑组合，nested 负责处理特殊类型的字段。 当你需要对嵌套类型的数组进行精确匹配时，就必须使用 nested 查询，并经常把它放入 bool 查询的 must、should 等子句中来构建更大的查询逻辑。
