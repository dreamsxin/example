# 安装sails.js
```shell
sudo apt-get install nodejs npm
# 升级 npm
sudo npm install npm -g
sudo npm install sails -g
```

# 创建应用
```shell
sails new todo
# or
sails generate new todo
```

项目架构
```list
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
```

`api/`

+ `api` 目录下是你要构建应用的核心所在，常说的MVC的设计结构就体现在这里
+ `api/controllers`：控制层，该层是Http请求的入口。Sails官方建议该层只处理请求的转发和页面的渲染，具体的逻辑实现应该交给Service层。
+ `api/models`：模型层，在Sails中，对于Model采用的是充血模型，除了可以在模型中定于属性之外，还可以定义包含逻辑处理的函数。在Sails中，所有Model都可以全局性访问。
+ `api/policies`：过滤层，该层在Controller层之前对Http请求做处理，在这一层中，可以定于一些规则来过滤Http请求，比如身份认证什么的。
+ `api/responses`：http响应的方法都放这里，例如服务器错误、请求错误、404错误等，定义在responses文件夹里面的方法，都会赋值到controller层的req对象中。
+ `api/services`：服务层，该层包含逻辑处理的方法，在Sails中，所有Service都可以全局性访问。

`views/`

视图层，存放视图模版文件的地方，Sails默认是提供ejs模版引擎的，如果你愿意，你可以换成jade、handlebars或者任何你喜欢的模版引擎。

`assets/`

资源文件夹，在Sails启动的时候，会启动某一个Grunt任务，把assets文件夹里的内容或压缩或编译或复制到根目录下的.tmp目录，这是前端可以直接通过路由访问的资源，HTML、JS、CSS以及图片等静态资源都放在这里了。

`config/`

配置文件夹，在Sails启动的时候，会加载该文件夹里的文件，并赋值在全局对象sails.config中，所以能够在任何一个地方都能用到。在用Sails开发，会经常跟这个文件夹里的文件打交道，从config的构成很容易知道Sails都提供哪方面的功能。

`tasks/`

Sails自带的项目自动化工具是Grunt，而Grunt的配置和任务注册都放在这个文件夹里了。这里已经提供了通常会用到的CSS编译、JS压缩、文件合并，更改检测等等任务，当然如果没有自己需要的，还能扩展。

`app.js`

Sails的启动文件，无论是`sails lift`命令或者`npm start`命令都会运行该文件。

# 启动
```shell
cd todo
sails lift
# or
sails lift --dev # assets 不会缓存
# or
sails lift --prod # 当服务器崩溃时，它会自动重新启动。
```

其他参数：
- --verbose - with verbose logging enabled
- --silly - with insane logging enabled

# 代码热更新

```shell
sudo npm install -g forever
forever -w start app.js
supervisor -i .tmp,.git,views app.js
```

设置 `config/local.js`：
```shell
{
	environment: process.env.NODE_ENV || 'development'
}
```
所有生产环境设置都储存在 `config/env/production.js`

可以浏览 `http://localhost:1337/` 看到一个引导页

# 创建模型user，并生成reset接口
```shell
sails generate api user
```

重新启动，可以浏览 `http://localhost:1337/user`


# 默认路由

+ Sails中要理解路由，首先要记得这个名词blueprint，中文翻译为：蓝图。
+ 我不知道官方是否解释过为什么要用个单词，但以我的理解，Sails的blueprint是负责指挥每一条客户端请求应该分配到服务器端的哪个Action去，所以叫蓝图吧。
+ blueprint主要分为三种：RESTful routes、Shortcut routes、Action routes。

## RESTful routes

当路径诸如：/:modelIdentity 或者 /:modelIdentity/:id的时候，blueprint会根据HTTP的动作（GET、POST、DELETE、PUT等）来分配到相应的Controller下相应的Action来处理。例如一个POST请求/user会创建一个用户，一个DELETE请求/user/123会删除id为123的用户。

## Shortcut routes

这种路由主要是方便开发，请求的参数可以直接写在请求路径中，例如/user/create?name=joe会创建一个新的用户，/user/update/1?name=mike会更新id为1的用户的名字。shortcut routes在开发环境很便利，但是在生产环境下需要关闭。

## Action routes

+ 这种路由会自动的为Controller层的每一个Action创建一个路由，例如你的Controller层有一个FooController.js，里面有一个Actionbar，那么请求/foo/bar就会分配到barAction。
+ 当然Sails也会提供自定义的路由，用户可以在config/routes.js和config/polices.js这两个配置文件中选择关闭或者打开blueprint提供的路由，和定义自己的路由。
+ 在生产环境中可以关闭该功能，修改文件`config/blueprints.js`

```json
rest: false,
shortcuts: false,
```

下面的操作需要 shortcuts 支持。

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
}
```
修改 `config/models.js` 增加一行
```json
connection: 'postgresql',
migrate: 'alter'
```

如果需要通过rest接口修改表结构，需要设置
```shell
migrate: 'alter'
```

`migrate` 说明：

+ `safe` - never auto-migrate my database(s). I will do it myself (by hand)[不自动合并数据，需要手动控制]
+ `alter` - auto-migrate, but attempt to keep my existing data (experimental)[与老数据自动合并，当添加新字段后，数据表才会被删除，推荐使用]
+ `drop` - wipe/drop ALL my data and rebuild models every time I lift Sails[每次都删除数据表，建立新表，插入新数据]


# 更改自动更新日期字段名
```json
autoCreatedAt: 'created',
autoUpdatedAt: 'updated'
```

# 更改首页视图，增加用户列表显示

## 创建 controller
```shell
sails generate controller home
```

## 创建 action，视图变量绑定
```js
module.exports = {
	index: function (req, res){
		sails.log.debug('silly日志！');
		return res.view("home/index",{title:"首页", content:"欢迎使用 Sails"});
	}
};
# or
module.exports = {
	index: function (req, res){
		sails.log.debug('silly日志！');
		return res.view({title:"首页", content:"欢迎使用 Sails"});
	}
};
```

# 日志级别 `config/log.js`
`silly`, `verbose`, `info`, `debug`, `warn`, `error`
默认 `debug`

# 策略 policies

可以作用在controller的action上，当条件通过，会`next()`它，继续执行action方法，当条件不满足时，可以中断该请求。

## 自带策略 sessionAuth
```shell
// policies/sessionAuth.js
/**
 * sessionAuth
 *
 * @module      :: Policy
 * @description :: Simple policy to allow any authenticated user
 *                 Assumes that your login action in one of your controllers sets `req.session.authenticated = true;`
 * @docs        :: http://sailsjs.org/#!/documentation/concepts/Policies
 *
 */
module.exports = function(req, res, next) {

  // User is allowed, proceed to the next policy, 
  // or if this is the last policy, the controller
  if (req.session.authenticated) {
    return next();
  }

  // User is not allowed
  // (default res.forbidden() behavior can be overridden in `config/403.js`)
  return res.forbidden('You are not permitted to perform this action.');
};
```

## 定义策略 canWrite
```shell
// policies/canWrite.js
module.exports = function canWrite (req, res, next) {
  var targetFolderId = req.param('id');
  var userId = req.session.user.id;

  Permission
  .findOneByFolderId( targetFolderId )
  .exec( function foundPermission (err, permission) {

    // Unexpected error occurred-- skip to the app's default error (500) handler
    if (err) return next(err);

    // No permission exists linking this user to this folder.  Maybe they got removed from it?  Maybe they never had permission in the first place?  Who cares?
    if ( ! permission ) return res.redirect('/notAllowed');

    // OK, so a permission was found.  Let's be sure it's a "write".
    if ( permission.type !== 'write' ) return res.redirect('/notAllowed');

    // If we made it all the way down here, looks like everything's ok, so we'll let the user through
    next();
  });
};
```

## 指定策略
修改 `config/policies.js`
```json
RabbitController: {
	// Apply the `false` policy as the default for all of RabbitController's actions
	// (`false` prevents all access, which ensures that nothing bad happens to our rabbits)
	'*': false, // '*': 'sessionAuth',

	// For the action `nurture`, apply the 'isRabbitMother' policy (this overrides `false` above)
	 nurture : 'isRabbitMother',

	// Apply the `isNiceToAnimals` AND `hasRabbitFood` policies before letting any users feed our rabbits
	 feed : ['isNiceToAnimals', 'hasRabbitFood'],
	 edit: 'canWrite'
}
```
