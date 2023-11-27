# 
ElasticSearch 可视化工具
- kinaba
- Dejavu
```shell
docker run -p 1358:1358 -d appbaseio/dejavu
```
- ElasticHD

## ES API

### 查看健康状态
`curl -X GET 127.0.0.1:9200/_cat/health?v`
### 查询当前es集群中所有的indices
`curl -X GET 127.0.0.1:9200/_cat/indices?v`
### 创建索引
`curl -X PUT 127.0.0.1:9200/www`
### 删除索引
`curl -X DELETE 127.0.0.1:9200/www`

### 插入记录
```shell
curl -H "ContentType:application/json" -X POST 127.0.0.1:9200/user/person -d '
    {
        "name": "LMH",
        "age": 18,
        "married": true
    }'
```
### 也可以使用PUT方法，但是需要传入id

```shell
curl -H "ContentType:application/json" -X PUT 127.0.0.1:9200/user/person/4 -d '
    {
        "name": "LMH",
        "age": 18,
        "married": false
    }'
```

### 检索
Elasticsearch的检索语法比较特别，使用GET方法携带JSON格式的查询条件。

### 全检索：

`curl -X GET 127.0.0.1:9200/user/person/_search`
按条件检索：
```shell
curl -H "ContentType:application/json" -X PUT 127.0.0.1:9200/user/person/4 -d '
    {
        "query":{
            "match": {"name": "LMH"}
        }    
    }'
```
ElasticSearch默认一次最多返回10条结果，可以像下面的示例通过size字段来设置返回结果的数目。
```shell
curl -H "ContentType:application/json" -X PUT 127.0.0.1:9200/user/person/4 -d '
    {
        "query":{
            "match": {"name": "LMH"},
            "size": 2
        }    
    }'
```

## elastic client

https://github.com/olivere/elastic

go.mod来管理依赖：
```shell
go get github.com/olivere/elastic/v7 v7.0.4
```
```mod
require (
  github.com/olivere/elastic/v7 v7.0.4
)
```
### Elasticsearch demo
```go
package main

import (
    "context"
    "fmt"

    "github.com/olivere/elastic/v7"
)

type Person struct {
    Name    string `json:"name"`
    Age     int    `json:"age"`
    Married bool   `json:"married"`
}

func main() {
    client, err := elastic.NewClient(elastic.SetURL("http://127.0.0.1:9200"))
    if err != nil {
        // Handle error
        panic(err)
    }

    fmt.Println("connect to es success")
    p1 := Person{Name: "lmh", Age: 18, Married: false}
    put1, err := client.Index().
        Index("user").
        BodyJson(p1).
        Do(context.Background())
    if err != nil {
        // Handle error
        panic(err)
    }
    fmt.Printf("Indexed user %s to index %s, type %s\n", put1.Id, put1.Index, put1.Type)
}

```
```go
package main

import (
    "context"
    "fmt"
    "log"
    "os"
    "reflect"

    "gopkg.in/olivere/elastic.v7" //这里使用的是版本5，最新的是6，有改动
)

var client *elastic.Client
var host = "http://127.0.0.1:9200/"

type Employee struct {
    FirstName string   `json:"first_name"`
    LastName  string   `json:"last_name"`
    Age       int      `json:"age"`
    About     string   `json:"about"`
    Interests []string `json:"interests"`
}

//初始化
func init() {
    errorlog := log.New(os.Stdout, "APP", log.LstdFlags)
    var err error
    client, err = elastic.NewClient(elastic.SetErrorLog(errorlog), elastic.SetURL(host))
    if err != nil {
        panic(err)
    }
    info, code, err := client.Ping(host).Do(context.Background())
    if err != nil {
        panic(err)
    }
    fmt.Printf("Elasticsearch returned with code %d and version %s\n", code, info.Version.Number)

    esversion, err := client.ElasticsearchVersion(host)
    if err != nil {
        panic(err)
    }
    fmt.Printf("Elasticsearch version %s\n", esversion)

}

/*下面是简单的CURD*/

//创建
func create() {

    //使用结构体
    e1 := Employee{"Jane", "Smith", 32, "I like to collect rock albums", []string{"music"}}
    put1, err := client.Index().
        Index("megacorp").
        Type("employee").
        Id("1").
        BodyJson(e1).
        Do(context.Background())
    if err != nil {
        panic(err)
    }
    fmt.Printf("Indexed tweet %s to index s%s, type %s\n", put1.Id, put1.Index, put1.Type)

    //使用字符串
    e2 := `{"first_name":"John","last_name":"Smith","age":25,"about":"I love to go rock climbing","interests":["sports","music"]}`
    put2, err := client.Index().
        Index("megacorp").
        Type("employee").
        Id("2").
        BodyJson(e2).
        Do(context.Background())
    if err != nil {
        panic(err)
    }
    fmt.Printf("Indexed tweet %s to index s%s, type %s\n", put2.Id, put2.Index, put2.Type)

    e3 := `{"first_name":"Douglas","last_name":"Fir","age":35,"about":"I like to build cabinets","interests":["forestry"]}`
    put3, err := client.Index().
        Index("megacorp").
        Type("employee").
        Id("3").
        BodyJson(e3).
        Do(context.Background())
    if err != nil {
        panic(err)
    }
    fmt.Printf("Indexed tweet %s to index s%s, type %s\n", put3.Id, put3.Index, put3.Type)

}

//删除
func delete() {

    res, err := client.Delete().Index("megacorp").
        Type("employee").
        Id("1").
        Do(context.Background())
    if err != nil {
        println(err.Error())
        return
    }
    fmt.Printf("delete result %s\n", res.Result)
}

//修改
func update() {
    res, err := client.Update().
        Index("megacorp").
        Type("employee").
        Id("2").
        Doc(map[string]interface{}{"age": 88}).
        Do(context.Background())
    if err != nil {
        println(err.Error())
    }
    fmt.Printf("update age %s\n", res.Result)

}

//查找
func gets() {
    //通过id查找
    get1, err := client.Get().Index("megacorp").Type("employee").Id("2").Do(context.Background())
    if err != nil {
        panic(err)
    }
    if get1.Found {
        fmt.Printf("Got document %s in version %d from index %s, type %s\n", get1.Id, get1.Version, get1.Index, get1.Type)
    }
}

//搜索
func query() {
    var res *elastic.SearchResult
    var err error
    //取所有
    res, err = client.Search("megacorp").Type("employee").Do(context.Background())
    printEmployee(res, err)

    //字段相等
    q := elastic.NewQueryStringQuery("last_name:Smith")
    res, err = client.Search("megacorp").Type("employee").Query(q).Do(context.Background())
    if err != nil {
        println(err.Error())
    }
    printEmployee(res, err)

    //条件查询
    //年龄大于30岁的
    boolQ := elastic.NewBoolQuery()
    boolQ.Must(elastic.NewMatchQuery("last_name", "smith"))
    boolQ.Filter(elastic.NewRangeQuery("age").Gt(30))
    res, err = client.Search("megacorp").Type("employee").Query(q).Do(context.Background())
    printEmployee(res, err)

    //短语搜索 搜索about字段中有 rock climbing
    matchPhraseQuery := elastic.NewMatchPhraseQuery("about", "rock climbing")
    res, err = client.Search("megacorp").Type("employee").Query(matchPhraseQuery).Do(context.Background())
    printEmployee(res, err)

    //分析 interests
    aggs := elastic.NewTermsAggregation().Field("interests")
    res, err = client.Search("megacorp").Type("employee").Aggregation("all_interests", aggs).Do(context.Background())
    printEmployee(res, err)

}

//简单分页
func list(size, page int) {
    if size < 0 || page < 1 {
        fmt.Printf("param error")
        return
    }
    res, err := client.Search("megacorp").
        Type("employee").
        Size(size).
        From((page - 1) * size).
        Do(context.Background())
    printEmployee(res, err)

}

//打印查询到的Employee
func printEmployee(res *elastic.SearchResult, err error) {
    if err != nil {
        print(err.Error())
        return
    }
    var typ Employee
    for _, item := range res.Each(reflect.TypeOf(typ)) { //从搜索结果中取数据的方法
        t := item.(Employee)
        fmt.Printf("%#v\n", t)
    }
}

func main() {
    create()
    delete()
    update()
    gets()
    query()
    list(1, 3)
}
```
