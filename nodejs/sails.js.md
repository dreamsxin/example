# 安装sails.js
```shell
sudo npm install sails -g
```

# 创建应用
```shell
sails new todo
```

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