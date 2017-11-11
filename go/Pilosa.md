# Pilosa

- https://github.com/pilosa/pilosa

分布式位图数据库

- 能够轻松横向扩展，并且保证速度的位图数据库，支持数据版本的粗粒度的时间序列（最小粒度到小时）
- 适合高频次的流数据处理，使用场景例如，如果你的业务主体数据达到10亿量级，并且该数据的附加属性数量达到百万级别，当你需要实时的筛选符合各种属性组合条件的业务主体数据，Pilosa会是个好帮手
- Pilosa 的数据存储不像现有的关系型数据 mysql、oracle的行式存储（一行是一个业务实体，一列是同一种数据实体同一个属性），包括index，column、frame、row、cell，是列式存储
- 一个index类似一张表，数据查询不能跨index
- 一个column表示一个业务实体，有业务意义，包括多个frame一行row的组合，从平面形象上看是纵向的
- 一个frame，表示一个业务实体的属性，有业务意义，是row的集合，从平面形象上看是横向的
- 一个row表示一个属性的一行数据存储，是cell的集合，从平面形象上看是横向的
- 一个cell就是0/1，是数据存储的逻辑最小单元

## 安装

二进制安装

```shell
go get -d github.com/pilosa/pilosa
cd $GOPATH/src/github.com/pilosa/pilosa
make install
```

## pilosa 数据结构

https://www.pilosa.com/docs/data-model/

pilosa的结构为行列布尔逻辑值BIT矩阵，行和列的含义存储在BoltDB的数据结构中。

![https://github.com/dreamsxin/example/blob/master/go/img/pilosa.jpg](https://github.com/dreamsxin/example/blob/master/go/img/pilosa.jpg?raw=true)

例如一张这样的表

```shell
create table tbl (c1 int, c2 text, c3 numeric, c4 timestamp);
insert into tbl values (1,'abc',4.0, now());
insert into tbl values (1,'ab12c',1.0, now());
```

转换为 pilosa 的结构，首先要对VALUE进行属性化转换，转换过程就是建立 k-v 的过程。属性即K-V。

例如，以上数据可以转换为以下属性。
```text
c1:1
c2:abc
c2:ab12c
c3:1.0-4.0
c4:'2017-01-01 00:00:00'-now()
```

属性建立后，转换为pilosa存储结构

```text
1 (c1:1) -> 11  
2 (c2:abc) -> 10  
3 (c2:ab12c) -> 01  
4 (c3:1.0-4.0) -> 11  
5 (c4:'2017-01-01 00:00:00'-now()) -> 11  
```

为了便于使用，pilosa 还将数据结构进行了拆分。

## 几种数据结构

Pilosa的元逻辑存储结构是一个bool矩阵，每一个cell的0/1表示行列是否存在关系。行和列可以表示任何东西，甚至可以表示相同的事物（这一点对基于pilosa思维优化查询和存储非常重要）。

基本的逻辑数据结构决定了业务的架构和封装模式。Pilosa的数据结构比文档数据库和传统关系数据库都多，是否能满足各种业务设计呢？是否满足就适合用呢？

* Index

类似于mysql的table，mongo的collection，表示一个业务数据的命名空间，无法对两个以上的index做联合查询。

* Column

column用ID表示，是顺序递增的整数，并且要赋予业务意义，是同一个index下面的所有frame共用，在index的作用域里是唯一的。

* Row

row用ID表示，是顺序递增的整数，并且要赋予业务意义，在frame的作用域里是唯一的。

* Frame

frame其实是对应传统关系型数据库中的每一列的属性，是pilosa的row的高维结构，由多row组成，从平面形象上看，将数据表格上多行row的整体看做一个frame，这样划分可以对0/1的二元表示意义进行扩展，满足业务多状态的需求

![https://github.com/dreamsxin/example/blob/master/go/img/frame.png](https://github.com/dreamsxin/example/blob/master/go/img/frame.png?raw=true)

* ranked cache of a frame

有序frame是通过行ID维护列计数的高速缓存，该缓存便于TopN查询，缓存大小默认为50,000

![https://github.com/dreamsxin/example/blob/master/go/img/frame2.png](https://github.com/dreamsxin/example/blob/master/go/img/frame2.png?raw=true)

* LRU cache of a frame

LRU缓存维护 frame 最近访问的行

![https://github.com/dreamsxin/example/blob/master/go/img/frame3.png](https://github.com/dreamsxin/example/blob/master/go/img/frame3.png?raw=true)

## Slice 与分布式有关

切片是列组，是pilosa的column的高维结构，包含多个column，每个切片包含固定数量的列，有SliceWidth。和column的关系，从平面形象上看类似于frame和row的关系，但是frame是有业务意义的，slice是为了分布式设计，通过预设宽度划分column成切片，用一致性哈希算法将切片分布到各cluster

## Attribute

在传统关系数据库里面关系表的数据，分为两种，一种是纯粹的关系，只存储ID，通过增删或者update status来表示关系的状态；另外一种关系同时已经是一种新的业务实体了，除了ID关系，还有业务属性。
第二种情况的附加业务属性就需要Attribute来支撑。

## Time Quantums

time quantums在frame上设置支持时间序列，会产生额外的数据冗余，这允许范围查询缩小到指定的时间间隔。 例如 - 如果时间量被设置为YMD，则支持范围查询到一天的粒度。

如果帧具有时间量化，则为每个定义的时间段生成视图。 

## View

视图表示frame的数据物理布局存在的可能情况

* Standard view of frame

标准视图的行列是输入的行列对应的，也是index中一定存在的一个布局

* Inverse view of frame

反向视图的行列是输入的行列颠倒的，即输入的行对应frame的列，输入的列对应frame的行，是index中可选的一个布局，会和标准视图及其他视图并存，在行列的业务意义是相同的时候，比较有意义，例如行列都是user_id

![https://github.com/dreamsxin/example/blob/master/go/img/frame4.png](https://github.com/dreamsxin/example/blob/master/go/img/frame4.png?raw=true)

* Time Quantums of frame

时间序列数据产生的多个布局，时间单位最小到小时，小时的布局包括年、月、日、时四个布局

![https://github.com/dreamsxin/example/blob/master/go/img/frame5.png](https://github.com/dreamsxin/example/blob/master/go/img/frame5.png?raw=true)

## 使用

* 业务场景

Star Trace 是一个跟踪github开源项目的关注情况的业务，有1000条最近有更新并且名称包含go的项目数据，数据字段包括编程语言，标签，项目的关注者。

* 启server

```shell
pilosa --help
pilosa server
```

* 建表，column 是 repository，frame是stargazer和language，构成了业务

```shell
curl localhost:10101/status
curl localhost:10101/schema
curl localhost:10101/index/repository \
     -X POST \
     -d '{"options": {"columnLabel": "repo_id"}}'
curl localhost:10101/index/repository/frame/stargazer \
     -X POST \
     -d '{"options": {"rowLabel": "stargazer_id", 
                      "timeQuantum": "YMD",
                      "inverseEnabled": true}}'
curl localhost:10101/index/repository/frame/language \
     -X POST \
     -d '{"options": {"rowLabel": "language_id",
                      "inverseEnabled": true}}'
```

* 数据导入

```shell
curl -O https://raw.githubusercontent.com/pilosa/getting-started/master/stargazer.csv
curl -O https://raw.githubusercontent.com/pilosa/getting-started/master/language.csv
pilosa import -i repository -f stargazer stargazer.csv
pilosa import -i repository -f language language.csv
```

* 查询操作

查询的结果都是列id，查14号用户关注的repository

```shell
curl localhost:10101/index/repository/query \
     -X POST \
     -d 'Bitmap(frame="stargazer", stargazer_id=14)'
```

查编程语言是 5 的repository

```shell
curl localhost:10101/index/repository/query \
     -X POST \
     -d 'TopN(frame="language", n=5)'
```

查14号用户和19号用户的关注的repository交集

```shell
curl localhost:10101/index/repository/query \
     -X POST \
     -d 'Intersect(Bitmap(frame="stargazer", stargazer_id=14), Bitmap(frame="stargazer", stargazer_id=19))'
```

查14号用户和19号用户的关注的repository并集

```shell
curl localhost:10101/index/repository/query \
     -X POST \
     -d 'Union(Bitmap(frame="stargazer", stargazer_id=14), Bitmap(frame="stargazer", stargazer_id=19))'
```

查14号用户和19号用户的共同关注的并且语言是1的repository

```shell
curl localhost:10101/index/repository/query \
     -X POST \
     -d 'Intersect(Bitmap(frame="stargazer", stargazer_id=14), Bitmap(frame="stargazer", stargazer_id=19), Bitmap(frame="language", language_id=1))'
```

在frame为stargazer里面，加一行关注者为 99999 数据，列上 repository 为 77777 的 cell 为 1

```shell
curl localhost:10101/index/repository/query \
     -X POST \
     -d 'SetBit(frame="stargazer", repo_id=77777, stargazer_id=99999)'
```

经过基本的使用，我觉得pilosa比传统的关系型数据库更侧重于关系，而通过列式存储的架构，方便了大数据的实时聚合计算，所以pilosa是为了在某些场景替代传统关系型数据库，对于文档数据库mongo和嵌入式数据库没有影响。在一些业务庞大的公司里面应该是可以考虑引入的。

## Pilosa 查询十亿级出租车搭乘数据案例

- http://www.nyc.gov/html/tlc/html/about/trip_record_data.shtml


Pilosa 是一个开源的分布式内存位图索引，可以针对海量数据集对查询进行加速，适合在用户数据查询以及用户画像等场景使用，但在另外一方面，非稀疏的关系型数据是否也可以使用 Pilosa 来查询？

Pilosa 最初是为分析用户属性数据（稀疏并且高基数）而准备的。所以当我们准备围绕 Pilosa 创建一家公司时，第一步就是评估在其他类型数据上的效果。

虽然用户细分的属性数量非常多，但数据却非常稀疏，这是因为很多属性只与少数人相关联，大部分人只有几百个或几千个属性。 Pilosa 可以非常优雅地处理这种场景，它能够在几毫秒内筛选数百万个属性的任意组合，从而找到满足该条件的用户。

![https://github.com/dreamsxin/example/blob/master/go/img/pilosa1.jpg](https://github.com/dreamsxin/example/blob/master/go/img/pilosa1.jpg?raw=true)

对于用户细分数据，我们可以使用 Pilosa 处理这类问题。然而，如果我们希望 Pilosa 成为通用索引，那么它将不仅能支持分段查询而且不仅再稀疏而高基数的数据上工作。

为了测试 Pilosa，我们需要一个具有密集的，较低基数的数据集，并最好是能通过其他解决方案进行探讨，以便评估我们如何处理该问题。

纽约市政府开放的十亿出租车搭乘数据集（http://www.nyc.gov/html/tlc/html/about/trip_record_data.shtml）正好符合以上条件。

（译者注：数据集中包含上下车时间、地点、距离、车费、支付方式、乘客人数等字段）

有很多分析该数据的文章，下面列出的仅仅是少数几个链接：

- Analyzing 1.1 Billion NYC Taxi and Uber Trips, with a Vengeance （http://toddwschneider.com/posts/analyzing-1-1-billion-nyc-taxi-and-uber-trips-with-a-vengeance/）
- Kx 1.1 billion taxi ride benchmark highlights advantages of kdb+ architecture （https://kx.com/2017/01/25/kx-1-1-billion-taxi-ride-benchmark-highlights-advantages-kdb-architecture/）
- Should I Stay or Should I Go? NYC Taxis at the Airport （http://chriswhong.com/open-data/should-i-stay-or-should-i-go-nyc-taxis-at-the-airport/）

对我们特别有用的是 Mark Litwintschik 和他编制的性能比较表的系列文章，这是结果表格供参考，其中分别使用了 Spark, PostgreSQL, Elasticsearch 等数据库来查询。

![https://github.com/dreamsxin/example/blob/master/go/img/pilosa2.jpg](https://github.com/dreamsxin/example/blob/master/go/img/pilosa2.jpg?raw=true)

我们对 Pilosa 执行了相同的四个查询，以便我们可以同其他解决方案的性能进行比较。

以下是每个查询的描述：

- 每个司机搭乘了多少位乘客？
- 对于每位乘客来说，打车平均花多少钱？
- 每一年，每位乘客会打几次车？
- 对于不同乘客人数，年份，旅行距离的组合，乘客数量有多少，结果按年度乘客数量降序排列。

现在我们有一个数据集和一组查询，都远远超出了 Pilosa 的最适合做的领域，以及对不同硬件/软件组合的长期性能对比。

如果您想了解如何在 Pilosa 中建模数据集并构建查询，请参阅我们的运输用例文章（https://www.pilosa.com/use-cases/taming-transportation-data/）。

现在我们并非只有数千万的 boolean 属性，其中一些属性具有更复杂的类型：整数，浮点，时间戳等等。总而言之，相比之下这个数据集更适合用关系型数据库。

我们在 AWS c4.8xlarge 实例上架起了一个 3 节点的 Pilosa 集群，并附加了一个 c4.8xlarge 实例来加载数据。我们使用 pdk 工具将数据加载到 Pilosa 中，使用以下命令行参数

```shell
pdk taxi -b 2000000 -c 50 -p <pilosa-host-ip>:10101 -f <pdk_repo_location>/usecase/taxi/greenAndYellowUrls.txt
```

这个过程需要大约 2 小时 50 分钟，其中包括从 S3 下载所有的 csv 文件，解析它们，并将数据加载到 Pilosa 中。

如果我们将结果添加到 Mark 表中，它将如下所示：

![https://github.com/dreamsxin/example/blob/master/go/img/pilosa3.png](https://github.com/dreamsxin/example/blob/master/go/img/pilosa3.png?raw=true)

请注意，每个组合的软硬件都不同，所以直接比较是很困难的。

我们应该注意到，Pilosa 在“查询1”上有一些“作弊”（由于其存储数据的方式，Pilosa 已经预先计算了此结果），因此查询时间大部分是网络延迟。

然而，对于其余的查询，Pilosa 表现非常出色 – 在某些情况下，可以胜过异构硬件，如多 GPU 配置。查询3 的 0.177s 时间特别令人吃惊，与 Nvidia Pascal Titan Xs 的表现相似。看起来 kdb+/q 也很好，但是请记住，Xeon Phi 7210 每个芯片有 256 个硬件线程，以及 16GB 的内存。这使它们的性能和内存带宽更接近于 GPU。当然价格也很贵，约 2400 美元。

对于我们来说，这些结果足以让我们花费更多的时间优化 Pilosa 用于其他方面。由于 Pilosa 的内部位图压缩格式没有针对密集数据进行优化，我们在这方面进行了更多的研究，并获得了令人振奋的结果，所以我们有理由认为这些数字还有很大的改进空间。