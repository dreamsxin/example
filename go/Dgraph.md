# Dgraph

一个开源的、事务性的、分布式的、原生的图形数据库。

https://github.com/dgraph-io/dgraph

## 运行 Dgraph
运行 docker 镜像是开始使用 dgraph 的最快方法。这个独立映像仅用于快速启动。

现在，只需运行下面的命令，Dgraph就已经启动并运行了。
```shell
docker run --rm -it -p 8000:8000 -p 8080:8080 -p 9080:9080 dgraph/standalone:v24.0.2
```

## 节点和边

在图数据库中，概念或实体表示为节点。它可以是一个销售，一个交易，一个地方，或一个人，所有这些实体都表示为图数据库中的节点。边表示两个节点之间的关系。

```json
{"name":"Karthic","age":28}
```
Follows
```json
{"name":"Jessica","age":31}
```
有两个人 Karthic 和 Jessica，用两个节点代表。他们的 name 和 age 信息作为这些节点的属性。这些节点的属性在 Dgraph 中称为**谓词**。

## dgraph UI Ratel

从浏览器访问 http://localhost:8000

Dgraph中的创建、更新和删除操作称为突变。

让我们转到 “Mutate” 选项卡 (Query右边)，并将以下突变粘贴到文本区域。现在不要执行它!

### 创建节点

```json
{
  "set": [
    {
      "name": "Karthic",
      "age": 28
    },
    {
      "name": "Jessica",
      "age": 31
    }
  ]
}
```

```json
{
  "set": [
    {
      "name": "Karthic",
      "age": 28,
      "follows": {
        "name": "Jessica",
        "age": 31
      }
    }
  ]
}
```

### 查询

https://dgraph.io/docs/query-language/

### has
定义函数名为 people （可任意），然后与内置函数 has 绑定，下面是查询所有带有谓词 name 的节点，并返回谓词 name，age。
```json
{
  people(func: has(name)) {
    name
    age
  }
}
```
### 灵活的模式
Dgraph不执行结构或模式。相反，您可以立即开始输入数据并根据需要添加约束。

## 基础操作

### 创建
```json
{
  "set":[
    {
      "name": "Michael",
      "age": 40,
      "follows": {
        "name": "Pawan",
        "age": 28,
        "follows":{
          "name": "Leyla",
          "age": 31
        }
      }
    }
  ]
}
```

### 查询使用 uid

内置函数uid以uid列表作为可变参数，可以传递一个(例如 `uid(0x1)`)或您需要的任意多个(例如 `uid(0x1, 0x2)`)。

它返回作为输入传递的相同uid，无论它们是否存在于数据库中。但是，只有当uid及其谓词都存在时，才返回所请求的谓词。

```json
{
    find_using_uid(func: uid(0x1)){
        uid
        name 
        age
    }
}
```

### 更新谓词

可以使用UID更新节点的一个或多个谓词。
我们之前使用set来创建新节点。但是在使用现有节点的UID时，它更新其谓词，而不是创建新节点。


迈克尔最近庆祝了他的41岁生日。我们把他的年龄更新到41岁。

转到 mutate 选项卡并执行突变。
```json
{
  "set":[
    {
      "uid": "0x1",
      "age": 41
    }
  ]
}
```
### 在现有节点之间添加一条边
您还可以使用现有节点的uid在它们之间添加一条边。

```json
{
  "set":[
    {
      "uid": "LEYLAS_UID",
      "follows": {
        "uid": "MICHAELS_UID"
      }
    }
  ]
}
```
### 遍历边

遍历可以回答与节点之间关系相关的问题或查询。因此，像这样的问题，通过遍历下面的关系得到答案。

让我们运行一个遍历查询，Michael跟随谁?
```json
{
    find_follower(func: uid(MICHAELS_UID)){
        name 
        age
        follows {
          name 
          age 
        }
    }
}
```

查询有三个部分:

- 选择根节点。 首先，您需要选择一个或多个节点作为遍历的起点。这些被称为根节点。在上面的查询中，我们使用uid()函数选择为Michael创建的节点作为根节点。
- 选择要遍历的边 您需要指定要遍历的边，从所选的根节点开始。然后，遍历，沿着这些边，从一端到另一端的节点。在我们的查询中，我们选择从Michael的节点开始遍历下面的边。遍历返回Michael通过下面这条边连接到该节点的所有节点。
- 指定要返回的谓词 因为Michael只指向一个人，所以遍历只返回一个节点。这些是第2级节点。根节点构成了第1级的节点。同样，我们可以指定从第2级节点返回哪些谓词。

### 多层次的遍历

```json
{
  find_follower(func: uid(MICHAELS_UID)) {
    name 
    age 
    follows {
      name
      age
      follows {
        name 
        age
      }
    }
  }
}
```

```json
{
  find_follower(func: uid(MICHAELS_UID)) {
    name 
    age 
    follows {
      name
      age
      follows {
        name 
        age
        follows {
          name 
          age
        }
      }
    }
  }
}
```

### 递归遍历
难道没有一个内置的函数可以让多级深度查询或遍历变得简单吗?

答案是肯定的!这就是recurse()函数所做的。
```json
{
  find_follower(func: uid(MICHAELS_UID)) @recurse(depth: 4) {
    name 
    age
    follows
  }
}
```
### 删除一个谓词

```json
{
    delete {
        <UID> <predicate_name> * .
    }
}
```
```json
{
  delete {
    <MICHAELS_UID> <age> * .
  }
}
```

## 博客实例

三个实体:Author、Blog post 和 Tag。
这些实体是相关的:

- 每个 Author 都有一篇或多 Blog post。边 `published` 将 Blog post 与其 Author 联系起来。这些边从 Author  节点开始，指向 Blog post 节点。
- 每个 Blog post 都有一个或多个 Tag。边 `tagged` 将 Blog post 与它们的 Tag 联系起来。这些边从 Blog post 节点中出现并指向 Tag 节点。

```json
{
 "set": [
  {
   "author_name": "John Campbell",
   "rating": 4.1,
   "published": [
    {
     "title": "Dgraph's recap of GraphQL Conf - Berlin 2019",
     "url": "https://blog.dgraph.io/post/graphql-conf-19/",
     "content": "We took part in the recently held GraphQL conference in Berlin. The experience was fascinating, and we were amazed by the high voltage enthusiasm in the GraphQL community. Now, we couldn’t help ourselves from sharing this with Dgraph’s community! This is the story of the GraphQL conference in Berlin.",
     "likes": 100,
     "dislikes": 4,
     "publish_time": "2018-06-25T02:30:00",
     "tagged": [
      {
       "uid": "_:graphql",
       "tag_name": "graphql"
      },
      {
       "uid": "_:devrel",
       "tag_name": "devrel"
      }
     ]
    },
    {
     "title": "Dgraph Labs wants you!",
     "url": "https://blog.dgraph.io/post/hiring-19/",
     "content": "We recently announced our successful Series A fundraise and, since then, many people have shown interest to join our team. We are very grateful to have so many people interested in joining our team! We also realized that the job openings were neither really up to date nor covered all of the roles that we are looking for. This is why we decided to spend some time rewriting them and the result is these six new job openings!.",
     "likes": 60,
     "dislikes": 2,
     "publish_time": "2018-08-25T03:45:00",
     "tagged": [
      {
       "uid": "_:hiring",
       "tag_name": "hiring"
      },
      {
       "uid": "_:careers",
       "tag_name": "careers"
      }
     ]
    }
   ]
  },
  {
   "author_name": "John Travis",
   "rating": 4.5,
   "published": [
    {
     "title": "How Dgraph Labs Raised Series A",
     "url": "https://blog.dgraph.io/post/how-dgraph-labs-raised-series-a/",
     "content": "I’m really excited to announce that Dgraph has raised $11.5M in Series A funding. This round is led by Redpoint Ventures, with investment from our previous lead, Bain Capital Ventures, and participation from all our existing investors – Blackbird, Grok and AirTree. With this round, Satish Dharmaraj joins Dgraph’s board of directors, which includes Salil Deshpande from Bain and myself. Their guidance is exactly what we need as we transition from building a product to bringing it to market. So, thanks to all our investors!.",
     "likes": 139,
     "dislikes": 6,
     "publish_time": "2019-07-11T01:45:00",
     "tagged": [
      {
       "uid": "_:annoucement",
       "tag_name": "annoucement"
      },
      {
       "uid": "_:funding",
       "tag_name": "funding"
      }
     ]
    },
    {
     "title": "Celebrating 10,000 GitHub Stars",
     "url": "https://blog.dgraph.io/post/10k-github-stars/",
     "content": "Dgraph is celebrating the milestone of reaching 10,000 GitHub stars 🎉. This wouldn’t have happened without all of you, so we want to thank the awesome community for being with us all the way along. This milestone comes at an exciting time for Dgraph.",
     "likes": 33,
     "dislikes": 12,
     "publish_time": "2017-03-11T01:45:00",
     "tagged": [
      {
       "uid": "_:devrel"
      },
      {
       "uid": "_:annoucement"
      }
     ]
    }
   ]
  },
  {
   "author_name": "Katie Perry",
   "rating": 3.9,
   "published": [
    {
     "title": "Migrating data from SQL to Dgraph!",
     "url": "https://blog.dgraph.io/post/migrating-from-sql-to-dgraph/",
     "content": "Dgraph is rapidly gaining reputation as an easy to use database to build apps upon. Many new users of Dgraph have existing relational databases that they want to migrate from. In particular, we get asked a lot about how to migrate data from MySQL to Dgraph. In this article, we present a tool that makes this migration really easy: all a user needs to do is write a small 3 lines configuration file and type in 2 commands. In essence, this tool bridges one of the best technologies of the 20th century with one of the best ones of the 21st (if you ask us).",
     "likes": 20,
     "dislikes": 1,
     "publish_time": "2018-08-25T01:44:00",
     "tagged": [
      {
       "uid": "_:tutorial",
       "tag_name": "tutorial"
      }
     ]
    },
    {
     "title": "Building a To-Do List React App with Dgraph",
     "url": "https://blog.dgraph.io/post/building-todo-list-react-dgraph/",
     "content": "In this tutorial we will build a To-Do List application using React JavaScript library and Dgraph as a backend database. We will use dgraph-js-http — a library designed to greatly simplify the life of JavaScript developers when accessing Dgraph databases.",
     "likes": 97,
     "dislikes": 5,
     "publish_time": "2019-02-11T03:33:00",
     "tagged": [
      {
       "uid": "_:tutorial"
      },
      {
       "uid": "_:devrel"
      },
      {
       "uid": "_:javascript",
       "tag_name": "javascript"
      }
     ]
    }
   ]
  }
 ]
}
```

### 谓词的数据类型

类型包括string、float、int和uid。除此之外，Dgraph还提供了另外三种基本数据类型:geo、dateTime和bool。

uid类型表示两个节点之间的谓词。换句话说，它们表示连接两个节点的边。
过分组一个或多个谓词来创建自定义数据类型(Type)。

### 查询谓词值
首先，让我们查询所有的作者和他们的评级rating:
```json
{
  authors_and_ratings(func: has(author_name)) {
    uid
    author_name
    rating
  }
}
```

### 索引
| 数据类型     | 可用的索引类型                              |
|----------|--------------------------------------|
| int      | int                                  |
| float    | float                                |
| string   | hash, exact, term, fulltext, trigram |
| bool     | bool                                 |
| geo      | geo                                  |
| dateTime | year, month, day, hour               |

在评级谓词上创建一个索引。Ratel UI让添加索引变得超级简单。

以下是步骤的顺序:

- 转到左边的Schema模式选项卡。
- 从列表中点击rating评级谓词。
- 在右侧的属性UI中勾选索引index选项。

### 比较器函数
| 比较器函数名 | 全称                       |
|--------|--------------------------|
| eq     | equals to                |
| lt     | less than                |
| le     | less than or equal to    |
| gt     | greater than             |
| ge     | greater than or equal to |

| 使用案例                   | 描述                        |
|------------------------|---------------------------|
| func: eq(age, 60)      | 返回年龄谓词等于60的节点。            |
| func: gt(likes, 100)   | 返回值为likes谓词大于100的节点。      |
| func: le(dislikes, 10) | 返回值为dislikes谓词小于或等于10的节点。 |

```json
{
  best_authors(func: ge(rating, 4.0)) {
    uid
    author_name
    rating
  }
}
```
边published从一个作者节点到一个博客帖子节点。因此，获取作者节点的博客帖子非常简单。我们需要从作者节点开始遍历published边。
```json
{
  authors_and_ratings(func: ge(rating, 4.0)) {
    uid
    author_name
    rating
    published {
      title
      content
      dislikes
    }
  }
}
```
类似地，让我们扩展前面的查询以获取这些博客文章的标签。
```json
{
  authors_and_ratings(func: ge(rating, 4.0)) {
    uid
    author_name
    rating
    published {
      title
      content
      dislikes
      tagged {
        tag_name
      }
    }
  }
}
```

### 过滤遍历
我们可以使用@filter指令过滤遍历的结果。你可以使用@filter指令使用任何Dgraph的比较器函数。您应该使用lt比较器函数来过滤dislikes少于10个的博客文章。
```json
{
  authors_and_ratings(func: ge(rating, 4.0)) {
    author_name
    rating

    published @filter(lt(dislikes, 10)) {
      title
      likes
      dislikes
      tagged {
        tag_name
      }
    }
  }
}
```
### 查询并找到数据库中的所有标签。
```json
{
  all_tags(func: has(tag_name)) {
    tag_name
  }
}
```

### 通过 Tag 查找博客
给 Tag 添加索引。
Dgraph 中的边是有方向的。您可以看到边 tagged 从博客文章节点指向标记节点。

沿着边的方向遍历对于Dgraph来说是很自然的。因此，要从另一个方向穿越，就需要你朝着边缘的相反方向移动。您仍然可以通过在查询中添加一个波浪号()来做到这一点。波浪号()必须添加在要遍历的边缘名称的开头。
```json
{
  devrel_tag(func: eq(tag_name,"devrel")) {
    tag_name

    ~tagged {
      title
      content
    }
  }
}
```

## 多语言标注

假设有谓词 comment，string 类型，多语言标注语法如下:
| 语法              | 结果                                                           |
|-----------------|--------------------------------------------------------------|
| comment         | 查找未标记字符串;如果不存在未标记的评论，则不返回任何内容。                               |
| comment@.       | 查找未标记的字符串，如果没有找到，则返回任何一个语言的内容。但是，这只返回一个值。                    |
| comment@jp      | 寻找标记为@jp的评论。如果没有找到，该查询将不返回任何内容。                              |
| comment@ru      | 寻找标记为@ru的评论。如果没有找到，该查询将不返回任何内容。                              |
| comment@jp:.    | 首先寻找标记为@jp的评论。如果没有找到，那么找到未标记的注释。如果没有找到，则返回任何一个语言的内容。         |
| comment@jp:ru   | 寻找标记为@jp的评论，然后是@ru。如果两者都没有找到，则不返回任何内容。                       |
| comment@jp：ru：. | 寻找标记为@jp的评论，然后是@ru。如果两者都没有找到，那么找到未标记的注释。如果没有找到，则返回任何一个语言的内容。 |
| comment@*       | 返回所有语言标记，包括未标记的。                                             |

### 菜肴评价

```json
{
  "set": [
    {
      "food_name": "Hamburger",
      "review": [
        {
          "comment": "Tastes very good"
        }
      ],
      "origin": [
        {
          "country": "United states of America"
        }
      ]
    },
    {
      "food_name": "Carrillada",
      "review": [
        {
          "comment": "Sabe muy sabroso"
        }
      ],
      "origin": [
        {
          "country": "Spain"
        }
      ]
    },
    {
      "food_name": "Pav Bhaji",
      "review": [
        {
          "comment": "स्वाद बहुत अच्छा है"
        }
      ],
      "origin": [
        {
          "country": "India"
        }
      ]
    },
    {
      "food_name": "Borscht",
      "review": [
        {
          "comment": "очень вкусно"
        }
      ],
      "origin": [
        {
          "country": "Russia"
        }
      ]
    },
    {
      "food_name": "mapo tofu",
      "review": [
        {
          "comment": "真好吃"
        }
      ],
      "origin": [
        {
          "country": "China"
        }
      ]
    }
  ]
}
```

### 查询所有菜肴

```json
{
  food_review(func: has(food_name)) {
    food_name
      review {
        comment
      }
      origin {
        country
      }
  }
}
```

### 添加菜肴评论
用三种不同的语言写评论:英语、日语和俄语。
```json
{
  "set": [
    {
      "food_name": "Sushi",
      "review": [
        {
          "comment": "Tastes very good",
          "comment@jp": "とても美味しい",
          "comment@ru": "очень вкусно"
        }
      ],
      "origin": [
        {
          "country": "Japan"
        }
      ]
    }
  ]
}
```
### 查询
```json
# 返回默认语言
{
  food_review(func: eq(food_name,"Sushi")) {
    food_name
      review {
        comment
      }
  }
}
# 只返回日语
{
  food_review(func: eq(food_name,"Sushi")) {
    food_name
    review {
      comment@jp
    }
  }
}

# 返回所有语言
{
  food_review(func: eq(food_name,"Sushi")) {
    food_name
    review {
      comment@*
    }
  }
}
```
