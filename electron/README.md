Electron、Tauri、Wails

## Creating a new app

```shell
npm config set registry https://registry.npmmirror.com
#npm config set registry https://registry.npm.taobao.org
#npm config set registry https://mirrors.huaweicloud.com/repository/npm/
#npm config set registry https://registry.npmjs.org
#npm config get registry
#npm install -g npm@latest

# with create-electron-app
npm init electron-app@latest my-app
```
实际执行：
```shell
> npx
> create-electron-app my-app
```

### Using templates

There are currently four first-party templates:

- webpack
- webpack-typescript
- vite
- vite-typescript

```shell
npm init electron-app@latest my-app -- --template=webpack
```

## Run

```shell
# 运行
npm run start
# 打包
npm run package
# 编译，生成安装程序
npm run make
```
