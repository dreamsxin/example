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

## Bleve中三种映射类型的区别与用法

### 1. mapping.IndexMappingImpl
定义：索引级别的全局映射配置，是整个索引的根映射结构
作用：
管理所有文档类型的映射定义
配置全局分析器(Analyzer)和字段存储策略
设置索引的默认行为和高级特性
核心属性：
- DefaultMapping: 默认文档映射
- TypesMapping: 不同文档类型的映射集合
- Analysis: 全局分析器配置
用法示例：

```go
indexMapping := bleve.NewIndexMapping()
// 配置全局分析器
indexMapping.Analysis.Analyzers["custom_analyzer"] = customAnalyzer
// 设置默认文档映射
indexMapping.DefaultMapping = defaultDocMapping
```

### 2. mapping.DocumentMapping

定义：文档级别的映射配置，用于定义特定类型文档的结构
作用：
定义文档包含的字段及其映射规则
配置文档级别的评分策略
设置字段之间的关系和继承规则
核心属性：
- Fields: 字段映射集合(key为字段名，value为FieldMapping)
- Dynamic: 是否自动映射未定义的字段
- Enabled: 是否启用该文档类型的索引
用法示例：

```go
docMapping := bleve.NewDocumentMapping()
// 为文档添加字段映射
docMapping.AddFieldMappingsAt("title", titleFieldMapping)
docMapping.AddFieldMappingsAt("content", contentFieldMapping)
// 设置动态映射策略
docMapping.Dynamic = false
```

### 3. mapping.FieldMapping
定义：字段级别的映射配置，控制单个字段的索引行为
作用：
指定字段的数据类型(文本、数值、日期等)
配置字段的分析器和索引选项
设置字段是否存储、是否分词、是否索引
核心属性：
- Type: 字段类型(text, keyword, numeric等)
- Analyzer: 字段专用分析器
- Store: 是否存储原始值
- Index: 是否创建索引
用法示例：

```go
// 创建文本字段映射
textField := bleve.NewTextFieldMapping()
textField.Analyzer = "standard"
textField.Store = true

// 创建关键字字段映射(不分词)
keywordField := bleve.NewKeywordFieldMapping()
keywordField.Index = true
```

#### 三者关系与使用流程

层级关系：


```plainText
IndexMappingImpl (索引) > DocumentMapping (文档类型) > FieldMapping (字段)
```

典型使用流程：


```go
// 1. 创建字段映射
titleMapping := bleve.NewTextFieldMapping()
tagsMapping := bleve.NewKeywordFieldMapping()

// 2. 创建文档映射并添加字段
docMapping := bleve.NewDocumentMapping()
docMapping.AddFieldMappingsAt("title", titleMapping)
docMapping.AddFieldMappingsAt("tags", tagsMapping)

// 3. 创建索引映射并设置文档映射
indexMapping := bleve.NewIndexMapping()
indexMapping.AddDocumentMapping("article", docMapping)

// 4. 使用索引映射创建索引
index, err := bleve.New("my_index", indexMapping)
```

### 关键区别总结
| 类型 | 作用范围 | 核心配置项 | 常见应用场景 |
|------|----------|------------|--------------| 
| IndexMappingImpl | 全局索引 | 分析器、文档类型集合 | 索引初始化配置 | 
| DocumentMapping | 文档类型 | 字段集合、动态映射 | 多类型文档区分 | 
| FieldMapping | 单个字段 | 数据类型、分词策略 | 字段级索引控制 |

## 各类搜索类型区别

### 1. MatchQuery (匹配查询)

特点：对单个字段进行全文搜索，支持分词、模糊匹配和相关性评分
语法：bleve.NewMatchQuery("关键词")
核心参数：
- SetField("字段名")：指定搜索字段（必填）
- SetAutoFuzziness(true)：自动模糊匹配（容错拼写错误）
- SetBoost(2.0)：提升该查询的权重
适用场景：单字段精确搜索（如搜索文章内容中的关键词）
示例：

```go
q := bleve.NewMatchQuery("人工智能")
q.SetField("Body")       // 仅搜索 Body 字段
q.SetAutoFuzziness(true) // 允许拼写错误（如“人工知能”也能匹配）
```

### 2. QueryStringQuery (查询字符串查询)

特点：支持多字段搜索、布尔逻辑、通配符等复杂语法，类似 SQL 的 WHERE 子句
语法：bleve.NewQueryStringQuery("查询表达式")
核心语法：
- 字段名:关键词：指定字段（如 Body:人工智能）
- AND/OR/NOT：逻辑运算符（如 Body:AI AND Tags:技术）
- *：通配符（如 progra* 匹配 program/programming）
- "短语"：精确短语匹配（如 "机器学习框架"）
适用场景：复杂多条件搜索（如同时搜索标题和标签）
示例：

```go
// 搜索 Body 包含“AI”且 Tags 包含“技术”的文档
q := bleve.NewQueryStringQuery("Body:AI AND Tags:技术")
```

### 3. TermQuery (词项查询)
特点：对字段进行精确匹配，不进行分词，适合枚举值（如标签、类别）
语法：bleve.NewTermQuery("精确值").SetField("字段名")
适用场景：标签、ID 等精确匹配（如搜索标签为“技术”的文档）
示例：

```go
q := bleve.NewTermQuery("技术").SetField("Tags")
```

### 4. PhraseQuery (短语查询)
特点：匹配连续的短语，支持设置词间距（slop）
语法：bleve.NewPhraseQuery([]string{"词1", "词2"}, "字段名").SetSlop(1)
核心参数：SetSlop(n)：允许短语中词的最大间隔（如 slop=1 允许“机器学习”匹配“机器 学习”）
适用场景：精确短语搜索（如搜索“自然语言处理”）

### 5. BoolQuery (布尔查询)
特点：组合多个子查询，支持 Must（必须匹配）、Should（可选匹配）、MustNot（必须不匹配）
语法：

```go
boolQ := bleve.NewBoolQuery()
boolQ.Must(bleve.NewMatchQuery("AI").SetField("Body"))
boolQ.Should(bleve.NewTermQuery("技术").SetField("Tags"))
boolQ.MustNot(bleve.NewTermQuery("过时").SetField("Status"))
```

适用场景：多条件组合搜索（如“内容含AI，标签可选技术，状态不为过时”）

### 6. FuzzyQuery (模糊查询)
特点：基于编辑距离（Levenshtein）匹配相似词，适合拼写纠错
语法：bleve.NewFuzzyQuery("关键词").SetField("字段名").SetFuzziness(2)
核心参数：SetFuzziness(n)：允许的最大编辑距离（n=2 表示最多2个字符差异）
适用场景：容错拼写错误（如搜索“tecnology”匹配“technology”）
对比总结
| 查询类型 | 核心能力 | 适用场景 | 性能 |
|-------------------|---------------------------|-----------------------------------|------|
| MatchQuery | 单字段全文搜索 | 普通文本搜索 | 高 |
| QueryStringQuery | 多字段+复杂逻辑 | 高级搜索框（支持用户输入查询语法）| 中 |
| TermQuery | 精确匹配 | 标签、ID 等枚举值查询 | 高 |
| PhraseQuery | 短语精确匹配 | 连续短语搜索 | 中 |
| BoolQuery | 组合多个子查询 | 多条件过滤 | 中低 |
| FuzzyQuery | 容错拼写错误 | 拼写纠错场景 | 低 |

### 最佳实践
- 简单文本搜索：优先用 MatchQuery（平衡性能和效果）
- 多条件组合：用 BoolQuery 组合多个基础查询
- 用户输入复杂查询：用 QueryStringQuery（需注意安全，避免注入）
- 标签/类别搜索：用 TermQuery（精确匹配，性能最优）
- 拼写容错：用 MatchQuery 的 SetAutoFuzziness(true) 或 FuzzyQuery
