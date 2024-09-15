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
