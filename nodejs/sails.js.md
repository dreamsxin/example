# 安装sails.js
```shell
sudo npm install sails -g
```

# 创建应用
```shell
sails new todo
```

项目架构

.
├── api
│   ├── controllers
│   ├── models
│   ├── policies
│   ├── responses
│   └── services
├── views
├── assets
├── config
├── tasks
├── node_modules
├── package.json
├── Gruntfile.js
├── README.md
└── app.js

`api/`

`api` 目录下是你要构建应用的核心所在，常说的MVC的设计结构就体现在这里
`api/controllers`：控制层，该层是Http请求的入口。Sails官方建议该层只处理请求的转发和页面的渲染，具体的逻辑实现应该交给Service层。
`api/models`：模型层，在Sails中，对于Model采用的是充血模型，除了可以在模型中定于属性之外，还可以定义包含逻辑处理的函数。在Sails中，所有Model都可以全局性访问。
`api/policies`：过滤层，该层在Controller层之前对Http请求做处理，在这一层中，可以定于一些规则来过滤Http请求，比如身份认证什么的。
`api/responses`：http响应的方法都放这里，例如服务器错误、请求错误、404错误等，定义在responses文件夹里面的方法，都会赋值到controller层的req对象中。
`api/services`：服务层，该层包含逻辑处理的方法，在Sails中，所有Service都可以全局性访问。

`views/`

视图层，存放视图模版文件的地方，Sails默认是提供ejs模版引擎的，如果你愿意，你可以换成jade、handlebars或者任何你喜欢的模版引擎。

`assets/`

资源文件夹，在Sails启动的时候，会启动某一个Grunt任务，把assets文件夹里的内容或压缩或编译或复制到根目录下的.tmp目录，这是前端可以直接通过路由访问的资源，HTML、JS、CSS以及图片等静态资源都放在这里了。

`config/`

配置文件夹，在Sails启动的时候，会加载该文件夹里的文件，并赋值在全局对象sails.config中，所以能够在任何一个地方都能用到。在用Sails开发，会经常跟这个文件夹里的文件打交道，从config的构成很容易知道Sails都提供哪方面的功能。

`tasks/`

Sails自带的项目自动化工具是Grunt，而Grunt的配置和任务注册都放在这个文件夹里了。这里已经提供了通常会用到的CSS编译、JS压缩、文件合并，更改检测等等任务，当然如果没有自己需要的，还能扩展。

`app.js`

Sails的启动文件，无论是$ sails lift命令或者$ npm start命令都会运行该文件。

# 启动
```shell
cd todo
sails lift
```

可以浏览 `http://localhost:1337/` 看到一个引导页

# 创建模型user，并生成reset接口
```shell
sails generate api user
```

重新启动，可以浏览 `http://localhost:1337/user`


# 创建 user

浏览 `http://localhost:1337/user/create`，这时切换回 `http://localhost:1337/user` 就可以看到数据了。

# 增加 user 数据信息
`http://localhost:1337/user/create?username=dreamszhu&sex=man`

# 更新 user 记录
`http://localhost:1337/user/update/1?username=dreamsxin&sex=man`

# 删除记录
`http://localhost:1337/user/destroy/3`

在没有配置数据库的时候，数据默认保存在本地`./.tmp/localDiskDb.db`

# 使用 postgresql
```shell
npm install sails-postgresq
```
修改 `config/connections.js` 增加，也可以在 `config/local.js` 下增加
```json
   postgresql: {
     adapter: 'sails-postgresql',
     host: 'localhost',
     user: 'postgres',
     password: '123456',
     database: 'zupu'
   },
```
修改 `config/models.js` 增加一行
```json
 connection: 'postgresql',
```

# 更改自动更新日期字段名
```json
autoCreatedAt: 'created',
autoUpdatedAt: 'updated'
```

# 更改首页视图，增加用户列表显示