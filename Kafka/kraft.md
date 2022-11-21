## 集群前提条件

1、部署Kafka集群搭建需要服务器至少3台，奇数台

2、Kafka的安装需要java环境，jdk1.8

3、Kafka安装包版本：kafka_2.12-3.3.1.tgz

4、假设3台服务器分别为：kafka1、kafka2、kafka3

## Kafka with KRaft

```shell
wget https://www.apache.org/dyn/closer.cgi?path=/kafka/3.3.1/kafka_2.13-3.3.1.tgz
tar -xzf kafka_2.13-3.3.1.tgz
cd kafka_2.13-3.3.1

#生成集群ID
KAFKA_CLUSTER_ID="$(bin/kafka-storage.sh random-uuid)"

#格式化存储目录，需要在每个节点上执行格式化命令，并确保每个节点使用的集群ID是相同的。
bin/kafka-storage.sh format -t $KAFKA_CLUSTER_ID -c config/kraft/server.properties

#启动Kafka服务
bin/kafka-server-start.sh config/kraft/server.properties
```

## CREATE A TOPIC TO STORE YOUR EVENTS

```shell
bin/kafka-topics.sh --create --topic quickstart-events --bootstrap-server localhost:9092

bin/kafka-topics.sh --describe --topic quickstart-events --bootstrap-server localhost:9092
Topic: quickstart-events        TopicId: NPmZHyhbR9y00wMglMH2sg PartitionCount: 1       ReplicationFactor: 1	Configs:
    Topic: quickstart-events Partition: 0    Leader: 0   Replicas: 0 Isr: 0
```

## WRITE SOME EVENTS INTO THE TOPIC

```shell
bin/kafka-console-producer.sh --topic quickstart-events --bootstrap-server localhost:9092
This is my first event
This is my second event
```

## READ THE EVENTS

```shell
bin/kafka-console-consumer.sh --topic quickstart-events --from-beginning --bootstrap-server localhost:9092
This is my first event
This is my second event
```

## IMPORT/EXPORT YOUR DATA AS STREAMS OF EVENTS WITH KAFKA CONNECT

```shell
echo "plugin.path=libs/connect-file-3.3.1.jar"
```

## kraft

https://github.com/apache/kafka/blob/trunk/config/kraft/README.md

在 config 目录下多了一个 kraft 目录，在该目录中有一套新的配置文件，可以直接脱离 ZooKeeper 运行。

```shell
#生成集群ID
/usr/local/kafka/bin/kafka-storage.sh random-uuid
#格式化存储目录，需要在每个节点上执行格式化命令，并确保每个节点使用的集群ID是相同的。
/usr/local/kafka/bin/kafka-storage.sh format -t 上一个命令生成得uuid -c /usr/local/kafka/config/kraft/server.properties
#启动Kafka服务
/usr/local/kafka/bin/kafka-server-start.sh /usr/local/kafka/config/kraft/server.properties

#后台启动
/usr/local/kafka/bin/kafka-server-start.sh -daemon /usr/local/kafka/config/kraft/server.properties

#创建一个topic
/usr/local/kafka/bin/kafka-topics.sh --create --topic test --partitions 3 --replication-factor 1 --bootstrap-server localhost:9092

#查看topic信息
/usr/local/kafka/bin/kafka-topics.sh --describe --topic test --bootstrap-server localhost:9092

#删除topic
/usr/local/kafka/bin/kafka-topics.sh --delete --topic test --bootstrap-server localhost:9092

#发送
/usr/local/kafka/bin/kafka-console-producer.sh --bootstrap-server localhost:9092 --topic test

#接收
/usr/local/kafka/bin/kafka-console-consumer.sh --bootstrap-server localhost:9092 --topic test --from-beginning
```

* process.roles

- broker：它在KRaft模式中扮演一个Broker角色；
- controller：它在KRaft模式中扮演一个Controller角色；
- broker,controller：它在KRaft模式中同时扮演Broker和Controller角色；

如果没有设置，则假定我们处于ZooKeeper模式。如前所述，如果不重新格式化，当前无法在ZooKeeper模式和KRaft模式之间来回转换。

如果你有10个Broker和3和Controller命名为controller1，controller2，controller3，你可以按照如下进行配置：
```conf
process.roles=controller
node.id=1
listeners=CONTROLLER://controller1.dn1.kafka-eagle.org:9093
controller.quorum.voters=1@controller1.dn1.kafka-eagle.org:9093,2@controller2.dn2.kafka-eagle.org:9093,3@controller3.dn3.kafka-eagle.org:9093
```

* kafka-dump-log
这里我们可以通过kafka-dump-log.sh工具来查看metadata日志信息，命令如下所示：

`./kafka-dump-log.sh --cluster-metadata-decoder --skip-record-metadata --files /data/soft/new/kafka2.8/data/kraft/\@metadata-0/*.log`

* Metadata Shell
可以通过kafka-metadata-shell.sh来查看元数据信息，这个和Zookeeper Client操作很类似，命令如下：

`./kafka-metadata-shell.sh --snapshot /data/soft/new/kafka2.8/data/kraft/\@metadata-0/00000000000000000000.log`
