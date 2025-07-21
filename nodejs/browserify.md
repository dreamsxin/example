# Browserify

- https://github.com/browserify/browserify

一个将 Node.js 模块化规范引入浏览器的工具，通过预编译将依赖打包成单一文件，使前端代码可直接使用 npm 模块。

```shell
npm install -g browserify
```

## 示例

playwright 转换

- https://github.com/browserify/browserify

```shell
browserify index.js -o playwright-web.js -r ./fake-bufferutil.js:bufferutil -r ./fake-utf-8-validate.js:utf-8-validate -r ./fake-electron/index.js:electron/index.js -r ./custom-url-mock.js:url -r ./fake-fs.js:fs -r ./ws-mock.js:ws
```
