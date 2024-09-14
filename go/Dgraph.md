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

## 突变

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

## 查询

```json
{
  people(func: has(name)) {
    name
    age
  }
}
```
