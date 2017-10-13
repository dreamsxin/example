# Mesos

https://github.com/apache/mesos

Mesos —— 像用一台电脑（一个资源池）一样使用整个数据中心

## 分布式操作系统内核

Mesos是以与Linux内核同样的原则而创建的，不同点仅仅是在于抽象的层面。Mesos内核运行在每一个机器上，同时通过 API 为各种应用提供跨数据中心和云的资源管理调度能力。这些应用包括Hadoop、Spark、Kafka、Elastic Search。还可配合框架 Marathon 来管理大规模的Docker等容器化应用。

## Mesos特性

- 可扩展到10000个节点
- 使用 ZooKeeper 实现 Master 和 Slave 的容错
- 支持 Docker 容器
- 使用 Linux 容器实现本地任务隔离
- 多资源调度能力（内存，CPU、磁盘、端口）
- 提供 Java，Python，C++等多种语言 APIs
- 通过 Web 界面查看集群状态
- 新版本将支持更多功能

## 下载

https://mesos-cn.gitbooks.io/mesos-cn/content/primer/Mesos-of-Getting-Started.html

```shell
# Install a few utility tools.
sudo apt-get install -y tar wget git

wget http://www.apache.org/dist/mesos/1.4.0/mesos-1.4.0.tar.gz
tar -zxf mesos-1.4.0.tar.gz
# or
git clone https://git-wip-us.apache.org/repos/asf/mesos.git
```

## 系统要求

Mesos 可以运行在 Linux (64位)和 Mac OS X（64位）。如果要从源代码编译 Mesos ,则 GCC 4.8.1+ 或 Clang 3.5+ 是必须的。

完全支持 Linux 下的进程隔离，内核版本要不低于 3.10

确保你的主机名可以通过 DNS 或 `/etc/hosts` 解析，从而可以完全支持 Docker 的 host-networking 功能。这在一些 Mesos 测试中会被用到。如有疑问，请确认 `/etc/hosts` 中包含你的主机名。

* Ubuntu

```text
# Install the latest OpenJDK.
$ sudo apt-get install -y openjdk-8-jdk
$ sudo update-alternatives --config java

# Install autotools (Only necessary if building from git repository).
$ sudo apt-get install -y autoconf libtool

# Install other Mesos dependencies.
$ sudo apt-get -y install build-essential python-dev python-boto libcurl4-nss-dev libsasl2-modules
$ sudo apt-get -y install libsasl2-dev libsvn-dev maven libapr1-dev
```

## 构建 Mesos

```text
# Change working directory.
$ cd mesos-1.4.0/

# 如果是从 git 获取源码需要执行这句
$ ./bootstrap

# Configure and build.
$ mkdir build
$ cd build
$ ../configure
$ make

# Run test suite.
$ make check

# Install (Optional).
$ make install
```

## 示例

Mesos 包含了用 Java , C++ , Python 写的 frameworks 示例。frameworks 示例的二进制文件只有在运行了`make check`之后才可用。

```text
# Change into build directory.
$ cd build

# Start mesos master (Ensure work directory exists and has proper permissions).
$ ./bin/mesos-master.sh --ip=127.0.0.1 --work_dir=/var/lib/mesos

# Start mesos slave.
$ ./bin/mesos-slave.sh --master=127.0.0.1:5050

# Visit the mesos web page.
$ http://127.0.0.1:5050

# Run C++ framework (Exits after successfully running some tasks.).
$ ./src/test-framework --master=127.0.0.1:5050

# Run Java framework (Exits after successfully running some tasks.).
$ ./src/examples/java/test-framework 127.0.0.1:5050

# Run Python framework (Exits after successfully running some tasks.).
$ ./src/examples/python/test-framework 127.0.0.1:5050
```