# Scratch-Blockly

整个环境由3部分组成，分别是 scratch-vm, scratch-blocks和scratch-gui。

## 准备工作

```shell
/* git, npm, nodejs工具准备 */

/* Linux下安装指令 */
sudo apt-get install npm
sudo apt-get install nodejs
sudo apt-get install git

/* 对版本要求较高，需升级 */
npm install -g npm      /* npm升级到最新版本 */
npm install -g n        /* nodejs升级 */
n stable                /* 升级到最新的稳定版本 */

mkdir scratch  /*便于管理，新建一个文件夹存放*/
cd scratch
git clone https://github.com/llk/scratch-gui     /*scratch-gui下载*/
git clone https://github.com/llk/scratch-vm      /*scratch-vm下载*/
git clone https://github.com/llk/scratch-blocks  /*scratch-blocks下载*/
```

## Scratch-VM的配置

```shell
cd scratch-vm
npm install
npm link
npm run watch
```
    在执行”npm run watch”时，会停留在”+4 hidden modules”这个位置，不需要久等，直接Ctrl+C终止程序进行下面的配置即可。

## Scratch-Blocks的配置

```shell
cd ../scratch-blocks
npm install
npm link
```

## Scratch-GUI的配置

```shell
cd ../scratch-gui
npm install
npm link scratch-vm scratch-blocks
npm install
npm start
```

打开浏览器，在地址栏中输入 127.0.0.1:8061 即可访问Scratch-Blocks，即Scratch 3.0界面。