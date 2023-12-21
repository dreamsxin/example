## 创建项目

```shell
npm create vue@latest
```

## 配置端口

```js
module.exports = {
	devServer: {
        port: 3000
    },
    transpileDependencies: ["vuetify"],
    chainWebpack: config => {
        config.module.rules.delete('eslint');
    },
    lintOnSave: true,
    css: {
        // Enable CSS source maps.
        sourceMap: process.env.NODE_ENV !== "production",
    },
    chainWebpack: config => {
        config.module.rules.delete('eslint')
    },


};

```
