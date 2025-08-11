# bleve

- https://github.com/blevesearch/bleve

A modern indexing + search library in GO

```go
package main

import (
	"encoding/json"
	"log"
	"net/http"

	"github.com/blevesearch/bleve/v2"
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

	document := struct {
		Id   string
		From string
		Body string
	}{Id: id, From: from, Body: body}

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
	query := r.FormValue("q")
	if query == "" {
		http.Error(w, "缺少查询参数q", http.StatusBadRequest)
		return
	}

	searchQuery := bleve.NewQueryStringQuery(query)
	searchRequest := bleve.NewSearchRequest(searchQuery)
	// 添加需要返回的字段
	searchRequest.Fields = []string{"From", "Body"}
	searchResult, err := index.Search(searchRequest)
	if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}

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
