# dask.distributed

`dask.distributed` 网络由一个调度器节点和多个工作节点组成。

## 安装

```shell
conda install dask distributed -c conda-forge
# or
pip install dask distributed --upgrade
# or
git clone https://github.com/dask/distributed.git
cd distributed
python setup.py install
```

## 启动 scheduler && worker

http://distributed.readthedocs.io/en/latest/setup.html

* 方法一（本地集群）
```text
$ python
>>> from dask.distributed import Client
>>> client = Client()  # set up local cluster on your laptop
>>> client
<Client: scheduler="127.0.0.1:8786" processes=8 cores=8>
```
本方法自动建立客户端连接。

* 方法二
在主节点运行启动调度器：
```text
$ dask-scheduler
Scheduler started at 127.0.0.1:8786
```
其他节点运行（也可以在单一节点运行多次）启动worker进程：
```text
$ dask-worker 127.0.0.1:8786
```

* 方法三：
```shell
sudo pip install paramiko
# 默认第一个为 scheduler 节点
dask-ssh 192.168.0.1 192.168.0.2 192.168.0.3 192.168.0.4
```

然后建立客户端连接：
```text
$ python
>>> from dask.distributed import Client
>>> client = Client('127.0.0.1:8786')
```

## 映射和提交函数执行 Map and Submit Functions

```text
>>> def square(x):
        return x ** 2

>>> def neg(x):
        return -x

>>> A = client.map(square, range(10))
>>> B = client.map(neg, A)
>>> total = client.submit(sum, B)
>>> total.result()
-285
```

## 收集 Gather

映射或提交之后函数会返回一个未来对象，即引用集群结果的轻量级令牌。

通过未来对象收集结果：
```text
>>> total.result()   # result for single future
-285
```
通过 `Client.gather` 方法收集任何未来对象结果：
```text
>>> client.gather(A) # gather for many futures
[0, 1, 4, 9, 16, 25, 36, 49, 64, 81]
```

## 重置集群状态

```python
client.restart()
```