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
