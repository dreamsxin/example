# 安装 node.js npm
```shell
sudo apt-get install nodejs npm
# 升级 npm
sudo npm install npm -g
# or
curl -IL http://npmjs.org/install.sh | sudo sh
```

# ”热部署“代码，实现动态调试

```shell
sudo npm install -g supervisor
supervisor app.js
```