# Ubuntu 下安装 ArangoDB

请参考 `https://www.arangodb.com/download/`

```shell
wget https://www.arangodb.com/repositories/arangodb2/xUbuntu_14.04/Release.key
sudo apt-key add - < Release.key

echo 'deb https://www.arangodb.com/repositories/arangodb2/xUbuntu_14.04/ /' | sudo tee /etc/apt/sources.list.d/arangodb.list
sudo apt-get install apt-transport-https
sudo apt-get update
sudo apt-get install arangodb=2.8.5
```

# 安装完成之后，即可启动服务

```shell
/etc/init.d/arangod start
```

启动后，打开 `http://localhost:8529/` 可以看到管理界面

可以更改端口，修改配置 `/etc/arangodb/arangob.conf`

# 获得版本，使用 HTTP REST API

```shell
curl --dump - -X GET http://localhost:8529/_api/version && echo
```

# 进入 shell 命令模式

```shell
arangosh # 默认用户 root 数据库 _system
```
或
```shell
arangosh --server.endpoint tcp://localhost:8529  --server.username root --server.database test  --server.disable-authentication false
```

## 备份与恢复
```shell
arangodump --output-directory "dump"
arangorestore --input-directory "dump"
```

## 用户的创建、更新、删除
```shell
require("org/arangodb/users").save(username, password, true); # 第三个参数 true 代表激活该用户
require("org/arangodb/users").update(username, password, true);
require("org/arangodb/users").remove(username);
```

## 显示当前数据库名称

```shell
db._name();
# or
require("internal").db._name();
```

## 显示当前数据库所在路径
```shell
db._path();
```

## 创建数据库
```shell
db._createDatabase("mydb", [], [{ username: "test", passwd: "123456", active: true}])
```

## 切换数据库
```shell
db._useDatabase("mydb")
```

## 执行 Javascript
```shell
for (var i = 0; i < 10; i ++) { require("internal").print("Hello world " + i + "! "); }
```

将以上代码存到文件 `/tmp/test.js`

```shell
require("internal").load("/tmp/test.js") 
```

## 创建 Collection
```shell
db._create("example");
```
或者
```shell
curl -X POST --data-binary @- --dump - http://localhost:8529/_api/database <<EOF
{ 
  "name" : "example" 
}
EOF
```

## 查看已有 Collection
```shell
db._collections();
```

## 删除 Collection
```shell
db.example.drop();
# or
col = db.example;
col.drop();
```

## 查看和设置 Collection 属性
```shell
db.example.properties();
db.example.properties({ waitForSync : true });
```

## 插入记录
```shell
db.example.save({ Hello : "World" });
db.example.save({ "name" : "John Doe", "age" : 29 });
db.example.save({ "name" : "Jane Smith", "age" : 31 });
```

## 显示所有记录
```shell
db.example.toArray()
```

## 删除所有记录
```shell
db.example.truncate();
```

## 通过指定字段查找记录
```shell
db.example.byExample({ name: "Jane Smith" }).toArray()
```
## 通过记录标识_id值获取
```shell
db.example.document("example/193023403")
```
AQL:
```shell
db._query('FOR user IN example FILTER user.name == "Jane Smith" RETURN user').toArray()
db._query('FOR user IN example FILTER user.age > 30 RETURN user').toArray()
```

## 删除记录
```shell
db.example.remove(db.example.byExample({ name: "John Doe" }).toArray()[0]._id)
```

# Graph 使用

## 创建社交关系图形例子，文件所在位置 `/usr/share/arangodb/js/common/modules/org/arangodb/graph-examples/example-graph.js`
```js
var Graph = require("org/arangodb/general-graph");
var db = require("internal").db;

var arangodb = require("org/arangodb");
var ArangoError = arangodb.ArangoError;

var createSocialGraph = function() {
  var g = Graph._create("social", [Graph._relation("relation", ["female", "male"], ["female", "male"])]);
  var a = g.female.save({name: "Alice", _key: "alice"});
  var b = g.male.save({name: "Bob", _key: "bob"});
  var c = g.male.save({name: "Charly", _key: "charly"});
  var d = g.female.save({name: "Diana", _key: "diana"});
  g.relation.save(a._id, b._id, {type: "married", _key: "aliceAndBob"});
  g.relation.save(a._id, c._id, {type: "friend", _key: "aliceAndCharly"});
  g.relation.save(c._id, d._id, {type: "married", _key: "charlyAndDiana"});
  g.relation.save(b._id, d._id, {type: "friend", _key: "bobAndDiana"});
  return g;
};
```

```shell
var examples = require("org/arangodb/graph-examples/example-graph.js");
var graph = examples.loadGraph("social");
```

## 显示所有男人/女人
```shell
db.female.toArray()
db.male.toArray()
```

## 显示关系网

```shell
db.relation.toArray()
```

## 通过关系查找记录
```shell
graph._graph("social")._fromVertex("relation/aliceAndBob")
graph._graph("social")._toVertex("relation/aliceAndBob")
```

## 返回所有关系路径
```shell
graph._graph("social")._paths()
graph._graph("social")._paths({direction : 'inbound', minLength : 1, maxLength :  2}); # 从子节点到父节点
```
AQL:
```aql
RETURN GRAPH_PATHS('social')
```

## 查找两者关系路径
```shell
graph._graph("social")._shortestPath("female/alice", "female/diana")
```
AQL:
```aql
RETURN GRAPH_SHORTEST_PATH("social", "female/alice", "female/diana")
```