## ERESOLVE unable to resolve dependency tree
临时环境变量大法（通用）
这个解决办法是 通用 的！这意味着无论你是否编写的是前端项目，都可以使用这个方法，这将对 Node.js 直接生效。

运行代码前，在终端内添加环境变量 `NODE_OPTIONS=--openssl-legacy-provider`

### macOS 和 Linux

export NODE_OPTIONS=--openssl-legacy-provider

### Windows PowerShell

`$env:NODE_OPTIONS = "--openssl-legacy-provider"`

### Windows 命令提示符（cmd）

`set NODE_OPTIONS=--openssl-legacy-provider`

## pnpm

```shell
npm install -g cnpm --registry=https://registry.npm.taobao.org
cnpm install pnpm -g
```
cnpm 在 win10 下可能需要设置 权限 `set-ExecutionPolicy RemoteSigned`

```shell
pnpm config get registry 
pnpm config set registry http://registry.npm.taobao.org 
```
