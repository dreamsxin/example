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
