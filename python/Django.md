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