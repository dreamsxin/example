* 安装 node-canvas

https://github.com/Automattic/node-canvas/wiki/Installation%3A-Windows

```shell
sudo apt-get install build-essential libcairo2-dev libpango1.0-dev libjpeg-dev libgif-dev librsvg2-dev
```
* 安装 fibers

```shell
npm install -g node-gyp
npm install --global --production windows-build-tools
npm config set msvs_version 2019
npm install fibers -D
```
`common.gypi not found` 删除 node-gyp 重新安装
