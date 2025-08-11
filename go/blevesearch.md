# bleve

- https://github.com/blevesearch/bleve

A modern indexing + search library in GO

```go
package main

import (
	"encoding/json"
	"log"
	"net/http"
	"strings"

	"github.com/blevesearch/bleve/v2"
	"github.com/blevesearch/bleve/v2/search/query"
)

var index bleve.Index

func init() {
	// 初始化索引
	var err error
	// 尝试打开现有索引
	index, err = bleve.Open("example.bleve")
	if err != nil {
		// 如果打开失败，创建新索引
		mapping := bleve.NewIndexMapping()
		// 为Tags字段添加映射
		tagMapping := bleve.NewTextFieldMapping()
		tagMapping.Analyzer = "keyword" // 使用keyword分析器，不分词
		mapping.DefaultMapping.AddFieldMappingsAt("Tags", tagMapping)

		index, err = bleve.New("example.bleve", mapping)
		if err != nil {
			log.Fatalf("无法创建索引: %v", err)
		}
		log.Println("新索引创建成功")
	} else {
		log.Println("现有索引加载成功")
	}
}

func indexHandler(w http.ResponseWriter, r *http.Request) {
	// 处理添加文档的逻辑
	id := r.FormValue("id")
	body := r.FormValue("body")
	from := r.FormValue("from")
	tagsStr := r.FormValue("tags") // 获取标签参数
	var tags []string
	if tagsStr != "" {
		// 分割逗号分隔的标签，并去除可能的空格
		tags = strings.Split(tagsStr, ",")
		for i, tag := range tags {
			tags[i] = strings.TrimSpace(tag)
		}
	}

	document := struct {
		Id   string
		From string
		Body string
		Tags []string
	}{Id: id, From: from, Body: body, Tags: tags}

	err := index.Index(id, document)
	if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}
	w.WriteHeader(http.StatusOK)
	w.Write([]byte("文档已索引"))
}

func searchHandler(w http.ResponseWriter, r *http.Request) {
	// 处理搜索请求
	q := r.FormValue("q")
	if q == "" {
		http.Error(w, "缺少查询参数q", http.StatusBadRequest)
		return
	}
	field := r.FormValue("field") // 获取指定字段参数，注意大小写比如 Body

	var searchQuery query.Query
	if field != "" {
		// 如果指定了字段，使用字段查询
		matchQuery := bleve.NewMatchQuery(q)
		matchQuery.SetField(field)
		matchQuery.SetAutoFuzziness(true) // 开启自动模糊查询

		searchQuery = matchQuery
	} else {
		// 否则使用默认的查询字符串查询
		searchQuery = bleve.NewQueryStringQuery(q)
	}
	searchRequest := bleve.NewSearchRequest(searchQuery)
	// 添加需要返回的字段
	searchRequest.Fields = []string{"From", "Body", "Tags"}
	searchResult, err := index.Search(searchRequest)
	if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}

	log.Println("查询结果:", searchResult)
	// 将结果转换为JSON并返回
	w.Header().Set("Content-Type", "application/json")
	json.NewEncoder(w).Encode(searchResult)
}

func main() {
	// 设置HTTP路由
	http.HandleFunc("/index", indexHandler)
	http.HandleFunc("/search", searchHandler)
	log.Println("服务启动在:9090")
	log.Fatal(http.ListenAndServe(":9090", nil))
}
```
