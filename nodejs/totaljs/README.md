#

```shell
sudo apt-get install nodejs
```

## 创建项目

https://docs.totaljs.com/latest/en.html#pages~Tutorial

```shell
mkdir empty-project
cd empty-project
npm install total.js --save
# or
sudo npm install -g total.js
```

* 渲染 tags

`@{import('meta', 'default.js', 'bootstrap.min.css', 'default.css', 'font-awesome.min.css', 'favicon.ico')}`

* 设置 meta tags

`@{meta('Your page title', 'Your page description', 'Your page keywords')}`

## 路由 route

https://docs.totaljs.com/latest/en.html#api~HttpRouteOptionsFlags

