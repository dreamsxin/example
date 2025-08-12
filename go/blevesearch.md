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
		tagMapping := bleve.NewKeywordFieldMapping()
		typeMapping := bleve.NewKeywordFieldMapping()  // 内容类型精确匹配
		urlMapping := bleve.NewKeywordFieldMapping()   // URL精确匹配
		dateMapping := bleve.NewDateTimeFieldMapping() // 日期类型映射

		mapping.DefaultMapping.AddFieldMappingsAt("Tags", tagMapping)
		mapping.DefaultMapping.AddFieldMappingsAt("Type", typeMapping)
		mapping.DefaultMapping.AddFieldMappingsAt("URL", urlMapping)
		mapping.DefaultMapping.AddFieldMappingsAt("Date", dateMapping)

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
	tagsStr := r.FormValue("tags")     // 获取标签参数
	contentType := r.FormValue("type") // news, blog, help, module
	url := r.FormValue("url")          // 网站模块地址
	date := r.FormValue("date")        // 发布日期
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
		Type string // 内容类型：news, blog, help, module
		URL  string // 网站模块地址
		Date string // 发布/创建日期
	}{Id: id, From: from, Body: body, Tags: tags, Type: contentType, URL: url, Date: date}

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
	field := r.FormValue("field")      // 获取指定字段参数，注意大小写比如 Body
	contentType := r.FormValue("type") // 获取内容类型参数

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

	// 添加内容类型过滤条件
	if contentType != "" {
		typeQuery := bleve.NewTermQuery(contentType)
		typeQuery.SetField("Type")
		searchQuery = bleve.NewConjunctionQuery(searchQuery, typeQuery)
	}

	searchRequest := bleve.NewSearchRequest(searchQuery)
	// 添加需要返回的字段
	searchRequest.Fields = []string{"From", "Body", "Tags", "Type", "URL", "Date"}
	//searchRequest.Explain = true // 开启详细解释

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

**demo**
```shell
curl --request POST \
  --url http://localhost:9090/index \
  --header 'Content-Type: application/x-www-form-urlencoded' \
  --data 'id=中文例子2' \
  --data 'body=我的名字叫云登2' \
  --data 'from=测试' \
  --data 'tags=测试1,指纹1,浏览器2' \
  --data type=blog \
  --data url=http://localhost
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

## Bleve命令行工具用法详解

### 基本语法

```bash
bleve [command] [flags]
```

### 核心命令说明

#### 1. create - 创建新索引

```bash
bleve create -index <index_name> -mapping <mapping.json>
```
功能：初始化新的Bleve索引
参数：
-index: 指定索引目录名
-mapping: 指定JSON格式的映射文件路径
示例：

```bash
bleve create -index my_index -mapping mapping.json
```

#### 2. index - 添加文档到索引

```bash
bleve index -index <index_name> <file1> <file2> ...
```
功能：将文件内容添加到索引
支持格式：纯文本、JSON等
示例：

```bash
bleve index -index my_index docs/*.txt
```

#### 3. query - 搜索索引

```bash
bleve query -index <index_name> -q <query_string> [-fields <field1>,<field2>]
```

功能：执行搜索查询
参数：
-q: 搜索关键词
-fields: 指定搜索字段（可选）
示例：

```bash
bleve query -index my_index -q "example text" -fields title,content
```

#### 4. count - 统计文档数量

```bash
bleve count -index <index_name>
```
功能：返回索引中的文档总数
示例：

```bash
bleve count -index my_index
```

#### 5. dump - 导出索引内容

```bash
bleve dump -index <index_name> [-output <file>]
```
功能：导出索引中的所有文档
参数：
-output: 指定输出文件（可选，默认stdout）

#### 6. fields - 列出索引字段

```bash
bleve fields -index <index_name>
```
功能：显示索引中所有已定义的字段名称

#### 7. mapping - 查看映射配置

```bash
bleve mapping -index <index_name>
```
功能：输出当前索引使用的映射配置（JSON格式）

#### 8. bulk - 批量导入文档

```bash
bleve bulk -index <index_name> <json_file1> <json_file2> ...
```
功能：从JSON文件批量导入文档（每行一个JSON对象）
JSON格式要求：

```json
{"id":"doc1","title":"...","content":"..."}
{"id":"doc2","title":"...","content":"..."}
```

### 高级命令

#### 9. dictionary - 查看字段词典

```bash
bleve dictionary -index <index_name> -field <field_name>
```
功能：显示指定字段的所有索引词项及频率

#### 10. check - 检查索引完整性

```bash
bleve check -index <index_name>
```
功能：验证索引结构完整性，检测潜在问题
全局选项
-h, --help: 显示命令帮助信息
所有命令均支持 -index 参数指定目标索引
使用示例流程
创建索引：

```bash
bleve create -index my_index -mapping mapping.json
```
添加文档：

```bash
bleve index -index my_index data/*.md
```
执行搜索：

```bash
bleve query -index my_index -q "bleve教程" -fields title
```
查看统计：

```bash
bleve count -index my_index
```
提示：使用 bleve [command] --help 获取特定命令的详细参数说明，例如：


```bash
bleve query --help
```

## Bleve工作原理及数据存储机制

### 一、Bleve核心工作原理

Bleve是一个基于Go语言的全文搜索引擎库，其核心原理基于倒排索引（Inverted Index）数据结构，主要工作流程包括：

**文档分析（Document Analysis）**

· 接收原始文档（JSON格式为主）
· 通过分析器（Analyzer） 对文本进行处理：
    分词（Tokenization）：将文本拆分为词项（Terms）
    标准化（Normalization）：转为小写、去除标点
    过滤（Filtering）：去除停用词（如"的"、"the"）、词干提取
· 生成可索引的词项列表

**索引构建（Indexing）**

· 创建倒排索引：记录每个词项在哪些文档中出现及位置
· 结构示例：
    ```plainText
    词项 "搜索引擎" → 文档1(位置3), 文档5(位置7)
    词项 "Bleve" → 文档1(位置1), 文档3(位置5), 文档7(位置2)
    支持动态映射（Dynamic Mapping）和显式字段映射
    ```
**查询处理（Query Processing）**

· 解析用户查询（支持MatchQuery、BoolQuery等多种查询类型）
· 执行查询计划并聚合结果
· 对结果进行相关性评分（BM25算法）

### 二、数据存储机制：磁盘为主，内存为辅

Bleve不将完整数据放在内存中，而是采用持久化存储+内存缓存的混合架构：

**磁盘存储（核心）**

默认使用BoltDB作为存储引擎（嵌入式KV数据库）
索引数据存储在文件系统中，典型结构：

```plainText
example.bleve/
├── index_meta.json  # 索引元数据
└── store/           # 实际数据存储（BoltDB文件）
```

支持多种存储后端：BoltDB、LevelDB、Badger等（可通过配置切换）
所有索引数据持久化到磁盘，服务重启后数据不丢失

**内存使用（优化）**

索引元数据缓存：词典、字段映射等频繁访问的数据
查询结果缓存：近期查询结果的临时存储
内存映射（mmap）：通过操作系统将部分索引文件映射到内存，加速访问
有限内存占用：不会无限制增长，内存使用量与索引大小正相关

### 三、关键特性说明

**增量索引**

支持文档的增量添加/更新/删除
无需重建整个索引，仅更新受影响的倒排列表

**并发安全**

内部通过锁机制保证多协程安全访问
读操作不阻塞写操作（MVCC机制）

**存储优化**

定期执行段合并（Segment Merging）减少磁盘IO
支持压缩算法降低磁盘占用

### 四、与纯内存搜索引擎的对比

| 特性 | Bleve（混合存储） | 纯内存搜索引擎（如Elasticsearch内存节点） | 
|--------------|---------------------------|------------------------------------------|
| 数据持久性 | 断电不丢失 | 全部丢失 | | 内存占用 | 可控 | 与数据量成正比 |
| 启动速度 | 较慢（需加载元数据） | 极快 | 
| 适用场景 | 持久化索引服务 | 临时数据分析、高频短期查询 |

### 五、用户项目中的验证

在您的项目目录中：


```plainText
go/search/example.bleve/
├── index_meta.json  # 索引元数据
└── store/           # BoltDB存储文件
```

这些文件证实Bleve将索引数据持久化到磁盘，而非仅存于内存。

总结：Bleve采用磁盘持久化存储保证数据安全，同时通过内存缓存和mmap技术优化查询性能，是兼顾可靠性和效率的设计选择。
