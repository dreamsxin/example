实现异步任务的工具有很多，其原理都是使用一个任务队列，比如使用redis生产消费模型或者发布订阅模式实现一个简单的消息队列。

broker是一个消息传输的中间件，可以理解为一个邮箱。每当应用程序调用celery的异步任务的时候，会向broker传递消息，而后celery的worker将会取到消息，进行对于的程序执行。好吧，这个邮箱可以看成是一个消息队列。那么什么又是backend，通常程序发送的消息，发完就完了，可能都不知道对方时候接受了。为此，celery实现了一个backend，用于存储这些消息以及celery执行的一些消息和结果。对于 brokers，官方推荐是rabbitmq和redis，至于backend，就是数据库啦。为了简单起见，我们都用redis。

使用celery包含三个方面，其一是定义任务函数，其二是运行celery服务，最后是客户应用程序的调用。

## 创建任务

`tasks.py`：
```python
from celery import Celery

brokers = 'redis://127.0.0.1:6379/5'
backend = 'redis://127.0.0.1:6379/6'

app = Celery('tasks', broker=broker, backend=backend)

@app.task
def add(x, y):
    return x + y
```

上述代码导入了celery，然后创建了celery实例app，实力话的过程中，指定了任务名tasks（和文件名一致），传入了broker和backend。然后创建了一个任务函数add。

## 启动 celery 服务

```shell
celery -A tasks worker  --loglevel=info
```

## 客户端调用任务

```python
from tasks import add
r = add.delay(2, 2)
r = add.delay(3, 3)
print r.ready()
print r.result
r.get()
```

## 计划任务

上述的使用是简单的配置，下面介绍一个更健壮的方式来使用celery。首先创建一个python包，celery服务，姑且命名为proj。目录文件如下：
```text
☁  proj  tree
.
├── __init__.py
├── celery.py             # 创建 celery 实例
├── config.py                # 配置文件
└── tasks.py                # 任务函数
```
首先是 celery.py
```python
#!/usr/bin/env python
# -*- coding:utf-8 -*-

from __future__ import absolute_import
from celery import Celery

app = Celery('proj', include=['proj.tasks'])

app.config_from_object('proj.config')

if __name__ == '__main__':
    app.start()
```
这一次创建 app，并没有直接指定 broker 和 backend。而是在配置文件中。

config.py
```python
#!/usr/bin/env python
# -*- coding:utf-8 -*-

from __future__ import absolute_import

CELERY_RESULT_BACKEND = 'redis://127.0.0.1:6379/5'
BROKER_URL = 'redis://127.0.0.1:6379/6'
```

剩下的就是tasks.py
```python
#!/usr/bin/env python
# -*- coding:utf-8 -*-

from __future__ import absolute_import
from proj.celery import app

@app.task
def add(x, y):
    return x + y
```
使用方法也很简单，在proj的同一级目录执行celery：

```shell
celery -A proj worker -l info
```

现在使用任务也很简单，直接在客户端代码调用 proj.tasks 里的函数即可。

## Scheduler

一种常见的需求是每隔一段时间执行一个任务。配置如下

config.py
```python
#!/usr/bin/env python
# -*- coding:utf-8 -*-

from __future__ import absolute_import

CELERY_RESULT_BACKEND = 'redis://127.0.0.1:6379/5'
BROKER_URL = 'redis://127.0.0.1:6379/6'

CELERY_TIMEZONE = 'Asia/Shanghai'

from datetime import timedelta

CELERYBEAT_SCHEDULE = {
    'add-every-30-seconds': {
         'task': 'proj.tasks.add',
         'schedule': timedelta(seconds=30),
         'args': (16, 16)
    },
}
```

注意配置文件需要指定时区。这段代码表示每隔30秒执行 add 函数。

一旦使用了 scheduler, 启动 celery需要加上-B 参数
```shell
celery -A proj worker -B -l info
```

## crontab

计划任务当然也可以用crontab实现，celery也有crontab模式。修改 config.py
```python
#!/usr/bin/env python
# -*- coding:utf-8 -*-

from __future__ import absolute_import

CELERY_RESULT_BACKEND = 'redis://127.0.0.1:6379/5'
BROKER_URL = 'redis://127.0.0.1:6379/6'

CELERY_TIMEZONE = 'Asia/Shanghai'

from celery.schedules import crontab

CELERYBEAT_SCHEDULE = {
    # Executes every Monday morning at 7:30 A.M
    'add-every-monday-morning': {
        'task': 'tasks.add',
        'schedule': crontab(hour=7, minute=30, day_of_week=1),
        'args': (16, 16),
    },
}
```
总而言之，scheduler的切分度更细，可以精确到秒。crontab模式就不用说了。当然celery还有更高级的用法，比如多个机器使用，启用多个worker并发处理等。
