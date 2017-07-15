# 安装

```shell
sudo apt-get install python-django
# or
pip install django
```

安装 Django 之后，您现在应该已经有了可用的管理工具 django-admin.py。

我们可以来看下`django-admin`的命令介绍:

```text
Usage: django-admin.py subcommand [options] [args]

Options:
  -v VERBOSITY, --verbosity=VERBOSITY
                        Verbosity level; 0=minimal output, 1=normal output,
                        2=verbose output, 3=very verbose output
  --settings=SETTINGS   The Python path to a settings module, e.g.
                        "myproject.settings.main". If this isn't provided, the
                        DJANGO_SETTINGS_MODULE environment variable will be
                        used.
  --pythonpath=PYTHONPATH
                        A directory to add to the Python path, e.g.
                        "/home/djangoprojects/myproject".
  --traceback           Raise on exception
  --version             show program's version number and exit
  -h, --help            show this help message and exit

Type 'django-admin.py help <subcommand>' for help on a specific subcommand.

Available subcommands:

[django]
    check
    cleanup
    compilemessages
    createcachetable
```

# 创建项目

我们可以使用 `django-admin` 来创建一个项目：

```shell
django-admin startproject HelloWorld
cd HelloWorld/
```

创建完成后我们使用 `tree` 命令可以查看下项目的目录结构：

```text
.
├── HelloWorld
│   ├── __init__.py
│   ├── settings.py
│   ├── urls.py
│   └── wsgi.py
└── manage.py

1 directory, 5 files
```

目录说明：

- HelloWorld				项目的容器。
- manage.py					一个实用的命令行工具，可让你以各种方式与该 Django 项目进行交互。
- HelloWorld/__init__.py	一个空文件，告诉 Python 该目录是一个 Python 包。
- HelloWorld/settings.py	该 Django 项目的设置/配置。
- HelloWorld/urls.py		该 Django 项目的 URL 声明; 一份由 Django 驱动的网站"目录"。
- HelloWorld/wsgi.py		一个 WSGI 兼容的 Web 服务器的入口，以便运行你的项目。

初始化数据库：

```shell
python manage.py migrate
```

输出：
```text
Operations to perform:
  Synchronize unmigrated apps: staticfiles, messages
  Apply all migrations: admin, contenttypes, auth, sessions
Synchronizing apps without migrations:
  Creating tables...
    Running deferred SQL...
  Installing custom SQL...
Running migrations:
  Rendering model states... DONE
  Applying contenttypes.0001_initial... OK
  Applying auth.0001_initial... OK
  Applying admin.0001_initial... OK
  Applying contenttypes.0002_remove_content_type_name... OK
  Applying auth.0002_alter_permission_name_max_length... OK
  Applying auth.0003_alter_user_email_max_length... OK
  Applying auth.0004_alter_user_username_opts... OK
  Applying auth.0005_alter_user_last_login_null... OK
  Applying auth.0006_require_contenttypes_0002... OK
  Applying sessions.0001_initial... OK
```

接下来我们进入 HelloWorld 目录输入以下命令，启动服务器：

```shell
python manage.py runserver 0.0.0.0:8000
```

# 视图和 URL 配置

在先前创建的 HelloWorld 目录下的 HelloWorld 目录新建一个 `view.py` 文件：

```python
from django.http import HttpResponse
 
def hello(request):
    return HttpResponse("Hello world")
```

这里使用 `django.http.HttpResponse` 来输出 "Hello World！"。

接着，绑定 URL 与视图函数。打开 `urls.py` 文件，删除原来代码：

```python
from django.conf.urls import url
 
from . import view
 
urlpatterns = [
    url(r'^$', view.hello),
]
```

完成后，启动 Django 开发服务器，并在浏览器访问打开浏览器并访问，可以看到内容变成了 `Hello world`。
我们也可以修改 url 匹配项为 `r'^hello$'`。

* url 函数

url 函数可以接收四个参数，分别是两个必选参数：regex、view 和两个可选参数 kwargs 和 name：

- regex: 正则表达式，与之匹配的 URL 会执行对应的第二个参数 view。
- view: 用于执行与正则表达式匹配的 URL 请求。
- kwargs: 视图使用的字典类型的参数。
- name: 用来反向获取 URL。


> 注意：项目中如果代码有改动，服务器会自动监测代码的改动并自动重新载入，所以如果你已经启动了服务器则不需手动重启

# 模版

在 HelloWorld 目录底下创建 templates 目录并建立 `hello.html` 文件：
```html
<h1>{{ hello }}</h1>
```
模板中变量使用了双括号。

在 `settings.py` 中修改视图目录为 `templates` 目录：

```python
TEMPLATES = [
    {
        'BACKEND': 'django.template.backends.django.DjangoTemplates',
        # 'DIRS': [BASE_DIR+"/templates"],
        'DIRS': [os.path.join(BASE_DIR, 'templates')],
        'APP_DIRS': True,
        'OPTIONS': {
            'context_processors': [
                'django.template.context_processors.debug',
                'django.template.context_processors.request',
                'django.contrib.auth.context_processors.auth',
                'django.contrib.messages.context_processors.messages',
            ],
        },
    },
]
```

现在修改 `view.py`，增加一个新的对象，用于向模板提交数据：
```python
from django.shortcuts import render
 
def hello(request):
    context = {}
    context['hello'] = 'Hello World!'
    return render(request, 'hello.html', context)
```
这里使用 `render` 来替代之前使用的 `HttpResponse`。使用了一个字典 context 作为参数，其中元素的键值 "hello" 对应了模板中的变量 "{{ hello }}"。