# Kafka

## 安装ZooKeeper，并设置dataDir

```shell
wget http://mirrors.cnnic.cn/apache/zookeeper/zookeeper-3.4.6/zookeeper-3.4.6.tar.gz
tar zxvf zookeeper-3.4.6.tar.gz
cd zookeeper-3.4.6
cp -rf conf/zoo_sample.cfg conf/zoo.cfg
vim conf/zoo.cfg
```

## 启动ZooKeeper

```shell
sh bin/zkServer.sh start
```

## 安装并运行Kafka

```shell
wget http://apache.fayea.com/kafka/0.8.2.1/kafka_2.10-0.8.2.1.tgz
tar -zxf kafka_2.10-0.8.2.1.tgz
cd kafka_2.10-0.8.2.1
sh bin/kafka-server-start.sh config/server.properties
```

## 创建Topic

```shell
sh bin/kafka-topics --create --topic kafkatopic --replication-factor 1 --partitions 1 --zookeeper localhost:2181
```

## 查看Topic

```shell
sh bin/kafka-topics --list --zookeeper localhost:2181
```

## 启动Producer 生产消息

```shell
sh bin/kafka-console-producer --broker-list localhost:9092 --topic kafkatopic
```

## 启动Consumer 消费消息

```shell
sh bin/kafka-console-consumer --zookeeper localhost:2181 --topic kafkatopic --from-beginning
```

## 删除Topic

```shell
sh bin/kafka-run-class kafka.admin.TopicCommand --delete --topic kafkatopic --zookeeper localhost:2181
```

## 查看Topic 的offset

```shell
sh bin/kafka-consumer-offset-checker  --zookeeper localhost:2181 --topic kafkatopic --group consumer
```

Kafka 的数据在Zookeeper 节点的 /var/local/kafka/data 目录中，以topic 作为子目录名。
