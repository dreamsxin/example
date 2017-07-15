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
    return render(request, 'index.html', context)
```
这里使用 `render` 来替代之前使用的 `HttpResponse`。使用了一个字典 context 作为参数，其中元素的键值 "hello" 对应了模板中的变量 "{{ hello }}"。

## 模版标签语法

* if/else

if/else 支持嵌套，基本语法格式如下：

```text
{% if condition1 %}
   ... display 1
{% elif condition2 %}
   ... display 2
{% else %}
   ... display 3
{% endif %}
```

根据条件判断是否输出。

`{% if %}` 标签接受 `and`、`or`、`not` 关键字来对多个变量做判断 ，例如：

```text
{% if athlete_list and coach_list %}
     athletes 和 coaches 变量都是可用的。
{% endif %}
```

* for

支持嵌套。

`{% for %}` 允许我们在一个序列上迭代。

与 Python的 for 语句的情形类似，循环语法是 `for X in Y` 。

每一次循环中，模板系统会渲染在 `{% for %}` 和 `{% endfor %}` 之间的所有内容：
```text
{% for athlete in athlete_list %}
    <h1>{{ athlete.name }}</h1>
    <ul>
    {% for sport in athlete.sports_played %}
        <li>{{ sport }}</li>
    {% endfor %}
    </ul>
{% endfor %}
```

给标签增加一个 `reversed` 使得该列表被反向迭代：
```text
{% for athlete in athlete_list reversed %}
...
{% endfor %}
```

* ifequal/ifnotequal

`{% ifequal %}` 标签比较两个值，当他们相等时，显示在 `{% ifequal %}` 和 `{% endifequal %}` 之中所有的值，比较两个模板变量 user 和 currentuser :

```text
{% ifequal user currentuser %}
    <h1>Welcome!</h1>
{% endifequal %}
```

和 `{% if %}` 类似， `{% ifequal %}` 支持可选的 `{% else%}` 标签：

```text
{% ifequal section 'sitenews' %}
    <h1>Site News</h1>
{% else %}
    <h1>No News Here</h1>
{% endifequal %}
```

* {# #} 注释标签

```text
{# 这是一个注释 #}
```

* 过滤器

模板过滤器可以在变量被显示前修改它，过滤器使用管道字符，过滤管道可以被套接，如下所示：
```text
{{ name|lower }}
{{ my_list|first|upper }}
```

过滤器的参数跟随冒号之后并且总是以双引号包含。 例如：
```text
{{ bio|truncatewords:"30" }}
```
这个将显示变量 bio 的前30个词。

其他过滤器：

- addslashes	添加反斜杠到任何反斜杠、单引号或者双引号前面。
- date			按指定的格式字符串参数格式化 date 或者 datetime 对象，实例：`{{ pub_date|date:"F j, Y" }}`
- length		返回变量的长度。

* include 标签

`{% include %}` 标签允许在模板中包含其它的模板的内容。

下面这个例子都包含了 nav.html 模板：
```text
{% include "nav.html" %}
```

* 模板继承

模板可以用继承的方式来实现复用。

接下来我们先创建之前项目的 templates 目录中添加 base.html 文件，代码如下：
```html
<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8">
<title></title>
</head>
<body>
    <h1>Hello World!</h1>
    <p>我是Base</p>
    {% block mainbody %}
       <p>original</p>
    {% endblock %}
</body>
</html>
```

以上代码中，名为 `mainbody` 的 `block` 标签是可以被继承者们替换掉的部分。

所有的 {% block %} 标签告诉模板引擎，子模板可以重载这些部分。

`index.html` 中继承 `base.html`，并替换特定 `block`，`hello.html` 修改后的代码如下：
```text
{% extends "base.html" %}
 
<h1>{{ hello }}</h1>
{% block mainbody %}
<p>继承了 base.html 文件</p>
{% endblock %}
```

第一行代码说明 index.html 继承了 base.html 文件。可以看到，这里相同名字的 block 标签用以替换 `base.html` 的相应 block。

## 模型

Django 对各种数据库提供了很好的支持，包括：PostgreSQL、MySQL、SQLite、Oracle。 
如果你没安装 mysql 驱动，可以执行以下命令安装：

```shell
sudo pip install mysqlclient
```

### 配置数据库

默认使用的是 `sqlite3`，通过修改 `settings.py` 文件中的 `DATABASES` 配置项，改为使用 Mysql：
```shell
DATABASES = {
    'default': {
        'ENGINE': 'django.db.backends.mysql',  # 或者使用 mysql.connector.django
        'NAME': 'test',
        'USER': 'test',
        'PASSWORD': 'test123',
        'HOST':'localhost',
        'PORT':'3306',
    }
}
```

## 定义模型

* 创建 APP

Django规定，如果要使用模型，必须要创建一个app。我们使用以下命令创建一个 `TestModel` 的 app:
```shell
django-admin startapp TestModel
```
接下来在settings.py中找到INSTALLED_APPS这一项，如下：
```text
INSTALLED_APPS = (
    'django.contrib.admin',
    'django.contrib.auth',
    'django.contrib.contenttypes',
    'django.contrib.sessions',
    'django.contrib.messages',
    'django.contrib.staticfiles',
    'TestModel',               # 添加此项
)
```

目录结构如下：
```text
.
├── db.sqlite3
├── HelloWorld
│   ├── __init__.py
│   ├── __init__.pyc
│   ├── settings.py
│   ├── settings.pyc
│   ├── urls.py
│   ├── urls.pyc
│   ├── view.py
│   ├── view.pyc
│   ├── wsgi.py
│   └── wsgi.pyc
├── manage.py
├── templates
│   ├── base.html
│   └── index.html
└── TestModel
    ├── admin.py
    ├── __init__.py
    ├── migrations
    │   └── __init__.py
    ├── models.py
    ├── tests.py
    └── views.py

4 directories, 20 files
```

修改 TestModel/models.py 文件，代码如下：
```python
# models.py
from django.db import models
 
class Test(models.Model):
    name = models.CharField(max_length=20)
```

以上的类名代表了数据库表名，且继承了models.Model，类里面的字段代表数据表中的字段(name)，数据类型则由CharField（相当于varchar）、DateField（相当于datetime）， max_length 参数限定长度。

* 根据模型创建数据库结构

在命令行中运行：

```shell
python manage.py migrate			# 如果更改了数据库，需要重新执行，创建默认表结构
python manage.py makemigrations TestModel	# 让 Django 知道我们在我们的模型有一些变更
python manage.py migrate TestModel		# 创建表结构
```

表名组成结构为：应用名_类名（如：TestModel_test）。

> 注意：尽管我们没有在models给表设置主键，但是Django会自动添加一个id作为主键。

## 数据库操作

接下来我们在 HelloWorld 目录中添加 testdb.py 文件：
```python
# -*- coding: utf-8 -*-
from django.http import HttpResponse
from TestModel.models import Test
 
# 数据库操作
def testdb(request):
    test1 = Test(name='runoob')
    test1.save()
    return HttpResponse("<p>数据添加成功！</p>")

```
修改 `urls.py`：
```python
from django.conf.urls import *
from . import view,testdb
 
urlpatterns = [
    url(r'^hello$', view.hello),
    url(r'^testdb$', testdb.testdb),
]
```

## 获取数据

Django提供了多种方式来获取数据库的内容，如下代码所示：
```python
# -*- coding: utf-8 -*-
from django.http import HttpResponse
from TestModel.models import Test
 
# 数据库操作
def testdb(request):
    # 初始化
    response = ""
    response1 = ""

    # 通过objects这个模型管理器的all()获得所有数据行，相当于SQL中的SELECT * FROM
    list = Test.objects.all()
        
    # filter相当于SQL中的WHERE，可设置条件过滤结果
    response2 = Test.objects.filter(id=1) 
    
    # 获取单个对象
    response3 = Test.objects.get(id=1) 
    
    # 限制返回的数据 相当于 SQL 中的 OFFSET 0 LIMIT 2;
    Test.objects.order_by('name')[0:2]
    
    #数据排序
    Test.objects.order_by("id")
    
    # 上面的方法可以连锁使用
    Test.objects.filter(name="runoob").order_by("id")
    
    # 输出所有数据
    for var in list:
        response1 += var.name + " "
    response = response1
    return HttpResponse("<p>" + response + "</p>")
```

## 更新数据

修改数据可以使用 save() 或 update():
```python
# -*- coding: utf-8 -*-
from django.http import HttpResponse
from TestModel.models import Test
 
# 数据库操作
def testdb(request):
    # 修改其中一个id=1的name字段，再save，相当于SQL中的UPDATE
    test1 = Test.objects.get(id=1)
    test1.name = 'Google'
    test1.save()
    
    # 另外一种方式
    #Test.objects.filter(id=1).update(name='Google')
    
    # 修改所有的列
    # Test.objects.all().update(name='Google')
    
    return HttpResponse("<p>修改成功</p>")
```

## 删除数据

```python
# -*- coding: utf-8 -*-
from django.http import HttpResponse
from TestModel.models import Test
 
# 数据库操作
def testdb(request):
    # 删除id=1的数据
    test1 = Test.objects.get(id=1)
    test1.delete()
    
    # 另外一种方式
    # Test.objects.filter(id=1).delete()
    
    # 删除所有数据
    # Test.objects.all().delete()
    
    return HttpResponse("<p>删除成功</p>")
```

## 表单

* GET 方法

我们在之前的项目中创建一个 search.py 文件，用于接收用户的请求：
```python
# -*- coding: utf-8 -*-
 
from django.http import HttpResponse
from django.shortcuts import render_to_response
 
# 表单
def search_form(request):
    return render_to_response('search_form.html')
 
# 接收请求数据
def search(request):  
    request.encoding='utf-8'
    if 'q' in request.GET:
        message = '你搜索的内容为: ' + request.GET['q']
    else:
        message = '你提交了空表单'
    return HttpResponse(message)
```
在模板目录 templates 中添加 search_form.html 表单：
```html
<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8">
<title></title>
</head>
<body>
    <form action="/search" method="get">
        <input type="text" name="q">
        <input type="submit" value="搜索">
    </form>
</body>
</html>
```

urls.py 规则修改为如下形式：
```python
from django.conf.urls import url
from . import view,testdb,search
 
urlpatterns = [
    url(r'^hello$', view.hello),
    url(r'^testdb$', testdb.testdb),
    url(r'^search-form$', search.search_form),
    url(r'^search$', search.search),
]
```

* POST 方法

我们在tmplate 创建 post.html：
```html
<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8">
<title>菜鸟教程(runoob.com)</title>
</head>
<body>
    <form action="/search-post" method="post">
        {% csrf_token %}
        <input type="text" name="q">
        <input type="submit" value="Submit">
    </form>
 
    <p>{{ rlt }}</p>
</body>
</html>
```
在模板的末尾，我们增加一个 `rlt` 记号，为表格处理结果预留显示位置。
表格后面还有一个`{% csrf_token %}`的标签。csrf 全称是 Cross Site Request Forgery。这是Django提供的防止伪装提交请求的功能（会自动填充数据）。POST 方法提交的表格，必须有此标签。

在HelloWorld目录下新建 `search2.py` 文件并使用 search_post 函数来处理 POST 请求：
```python
# -*- coding: utf-8 -*-
from django.shortcuts import render
from django.views.decorators import csrf
 
# 接收POST请求数据
def search_post(request):
    ctx ={}
    if request.POST:
        ctx['rlt'] = request.POST['q']
    return render(request, "post.html", ctx)
```

urls.py 规则修改为如下形式：
```python
from django.conf.urls import url
from . import view,testdb,search,search2
 
urlpatterns = [
    url(r'^hello$', view.hello),
    url(r'^testdb$', testdb.testdb),
    url(r'^search-form$', search.search_form),
    url(r'^search$', search.search),
    url(r'^search-post$', search2.search_post),
]
```

## Request 对象

每个 view 函数的第一个参数是一个 HttpRequest 对象，就像下面这个 hello() 函数:
```python
from django.http import HttpResponse

def hello(request):
    return HttpResponse("Hello world")
```
HttpRequest对象包含当前请求URL的一些信息：
- path
- GET
- POST
- COOKIES
- REQUEST
- FILES
- META
- user
- session
- raw_post_data

## QueryDict对象

在HttpRequest对象中, GET和POST属性是django.http.QueryDict类的实例。
QueryDict类似字典的自定义类，用来处理单键对应多值的情况。 

```text
>>> q = QueryDict('a=1')

>>> q = q.copy() # to make it mutable

>>> q.update({'a': '2'})

>>> q.getlist('a')

 ['1', '2']

>>> q['a'] # returns the last

['2']

>>> q = QueryDict('a=1&a=2&a=3')

>>> q.items()

[('a', '3')]

>>> q = QueryDict('a=1&a=2&a=3')

>>> q.lists()

[('a', ['1', '2', '3'])]
```

## Django Admin 管理工具

Django 提供了基于 web 的管理工具。

Django 自动管理工具是 django.contrib 的一部分。你可以在项目的 settings.py 中的 INSTALLED_APPS 看到它：
```python
INSTALLED_APPS = (
    'django.contrib.admin',
    'django.contrib.auth',
    'django.contrib.contenttypes',
    'django.contrib.sessions',
    'django.contrib.messages',
    'django.contrib.staticfiles',
)
```
django.contrib是一套庞大的功能集，它是Django基本代码的组成部分。

## 激活管理工具

通常我们在生成项目时会在 urls.py 中自动设置好，我们只需去掉注释即可。

配置项如下所示：
```python
# urls.py
from django.conf.urls import url
from django.contrib import admin
 
urlpatterns = [
    url(r'^admin/', admin.site.urls),
]
```
当这一切都配置好后，Django 管理工具就可以运行了。 

## 创建管理用户

```shell
python manage.py createsuperuser
```
```text
Username (leave blank to use 'root'): admin
Email address: admin@runoob.com
Password:
Password (again):
Superuser created successfully.
```

## 注册模型

为了让 admin 界面管理某个数据模型，我们需要先注册该数据模型到 admin。比如，我们之前在 TestModel 中已经创建了模型 Test 。修改 `TestModel/admin.py`:
```python
from django.contrib import admin
from TestModel.models import Test
 
# Register your models here.
admin.site.register(Test)
```

## 复杂模型

在 TestModel/models.py 中增加一个更复杂的数据模型：

```python
class Contact(models.Model):
    name   = models.CharField(max_length=200)
    age    = models.IntegerField(default=0)
    email  = models.EmailField()
    def __unicode__(self):
        return self.name
 
class Tag(models.Model):
    contact = models.ForeignKey(Contact)
    name    = models.CharField(max_length=50)
    def __unicode__(self):
        return self.name
```

创建表结构：
```shell
python manage.py makemigrations TestModel  # 让 Django 知道我们在我们的模型有一些变更
python manage.py migrate TestModel   # 创建表结构
```

## 自定义表单

我们可以自定义管理页面，来取代默认的页面。比如上面的 "add" 页面。我们想只显示 name 和 email 部分。修改 TestModel/admin.py:
```python
from django.contrib import admin
from TestModel.models import Test,Contact,Tag
 
# Register your models here.
class ContactAdmin(admin.ModelAdmin):
    fields = ('name', 'email')
 
admin.site.register(Contact, ContactAdmin)
admin.site.register([Test, Tag])
```
以上代码定义了一个 ContactAdmin 类，用以说明管理页面的显示格式。

里面的 fields 属性定义了要显示的字段。

由于该类对应的是 Contact 数据模型，我们在注册的时候，需要将它们一起注册。

我们还可以将输入栏分块，每个栏也可以定义自己的格式。修改 TestModel/admin.py为：
```python
from django.contrib import admin
from TestModel.models import Test,Contact,Tag
 
# Register your models here.
class ContactAdmin(admin.ModelAdmin):
    fieldsets = (
        ['Main',{
            'fields':('name','email'),
        }],
        ['Advance',{
            'classes': ('collapse',), # CSS
            'fields': ('age',),
        }]
    )
 
admin.site.register(Contact, ContactAdmin)
admin.site.register([Test, Tag])
```
上面的栏目分为了 Main 和 Advance 两部分。classes 说明它所在的部分的 CSS 格式。

## 内联(Inline)显示

上面的 Contact 是 Tag 的外部键，所以有外部参考的关系。

而在默认的页面显示中，将两者分离开来，无法体现出两者的从属关系。我们可以使用内联显示，让 Tag 附加在 Contact 的编辑页面上显示。

修改TestModel/admin.py：
```python
from django.contrib import admin
from TestModel.models import Test,Contact,Tag
 
# Register your models here.
class TagInline(admin.TabularInline):
    model = Tag
 
class ContactAdmin(admin.ModelAdmin):
    inlines = [TagInline]  # Inline
    fieldsets = (
        ['Main',{
            'fields':('name','email'),
        }],
        ['Advance',{
            'classes': ('collapse',),
            'fields': ('age',),
        }]
 
    )
 
admin.site.register(Contact, ContactAdmin)
admin.site.register([Test])
```

比如在列表中显示更多的栏目，只需要在 ContactAdmin 中增加 list_display 属性:
```python
from django.contrib import admin
from TestModel.models import Test,Contact,Tag
 
# Register your models here.
class TagInline(admin.TabularInline):
    model = Tag
 
class ContactAdmin(admin.ModelAdmin):
    list_display = ('name','age', 'email') # list
    inlines = [TagInline]  # Inline
    fieldsets = (
        ['Main',{
            'fields':('name','email'),
        }],
        ['Advance',{
            'classes': ('collapse',),
            'fields': ('age',),
        }]
 
    )
 
admin.site.register(Contact, ContactAdmin)
admin.site.register([Test])
```

搜索功能在管理大量记录时非常有，我们可以使用 search_fields 为该列表页增加搜索栏：
```python
from django.contrib import admin
from TestModel.models import Test,Contact,Tag
 
# Register your models here.
class TagInline(admin.TabularInline):
    model = Tag
 
class ContactAdmin(admin.ModelAdmin):
    list_display = ('name','age', 'email') # list
    search_fields = ('name',)
    inlines = [TagInline]  # Inline
    fieldsets = (
        ['Main',{
            'fields':('name','email'),
        }],
        ['Advance',{
            'classes': ('collapse',),
            'fields': ('age',),
        }]
 
    )
 
admin.site.register(Contact, ContactAdmin)
admin.site.register([Test])
```

## 安装 uwsgi

uwsgi:https://pypi.python.org/pypi/uWSGI

uwsgi 参数详解：http://uwsgi-docs.readthedocs.org/en/latest/Options.html
```shell
pip install uwsgi
uwsgi --version    # 查看 uwsgi 版本
```
测试 uwsgi 是否正常：

新建 test.py 文件，内容如下：
```python
def application(env, start_response):
	start_response('200 OK', [('Content-Type','text/html')])
	return "Hello World"
```
然后在终端运行：
```shell
uwsgi --http :8001 --wsgi-file test.py
```
在浏览器内输入：http://127.0.0.1:8001，查看是否有"Hello World"输出，若没有输出，请检查你的安装过程。

## uwsgi 配置

uwsgi支持ini、xml等多种配置方式，本文以 ini 为例， 在/ect/目录下新建uwsgi9090.ini，添加如下配置：
```ini
[uwsgi]
socket = 127.0.0.1:9090
master = true         //主进程
vhost = true          //多站模式
no-site = true        //多站模式时不设置入口模块和文件
workers = 2           //子进程数
reload-mercy = 10     
vacuum = true         //退出、重启时清理文件
max-requests = 1000   
limit-as = 512
buffer-size = 30000
pidfile = /var/run/uwsgi9090.pid    //pid文件，用于下面的脚本启动、停止该进程
daemonize = /website/uwsgi9090.log
```

## Nginx 配置

找到nginx的安装目录（如：/usr/local/nginx/），打开conf/nginx.conf文件，修改server配置：
```conf
server {
        listen       80;
        server_name  localhost;
        
        location / {            
            include  uwsgi_params;
            uwsgi_pass  127.0.0.1:9090;              //必须和uwsgi中的设置一致
            uwsgi_param UWSGI_SCRIPT demosite.wsgi;  //入口文件，即wsgi.py相对于项目根目录的位置，“.”相当于一层目录
            uwsgi_param UWSGI_CHDIR /demosite;       //项目根目录
            index  index.html index.htm;
            client_max_body_size 35m;
        }
    }
```
你可以阅读 Nginx 安装配置 了解更多内容。

设置完成后，在终端运行：

uwsgi --ini /etc/uwsgi9090.ini &
/usr/local/nginx/sbin/nginx

在浏览器输入：http://127.0.0.1，你就可以看到 django 的 "It work" 了。 
