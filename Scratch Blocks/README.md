# Scratch-Blockly

# scratch-gui

https://github.com/LLK/scratch-gui

- scratch-block
是folk了google的blockly, 是拖拽、组合代码块的图形界面。

- scratch-vm
是执行用户组合出的代码块，然后把结果渲染带出来。

- scratch-gui
是包在scratch-vm上面的一层，负责scratch主页的图形界面部分。它还提供了图形编辑、音频编辑、文件存取、界面语言设置、教程等功能。主要用到了ReactJS和Redux。

- scratch-www
是包在scratch-gui外的一层，是https://scratch.mit.edu/网站的源码。提供了账户系统、项目管理、项目分享等功能。主要用到了ReactJS和Redux。

## Installation

This requires you to have Git and Node.js installed.

In your own node environment/application:
```shell
npm install https://github.com/LLK/scratch-gui.git
```
If you want to edit/play yourself:
```shell
git clone https://github.com/LLK/scratch-gui.git
cd scratch-gui
npm install
```

## Running

Open a Command Prompt or Terminal in the repository and run:
```shell
npm start
```
Then go to http://localhost:8601/ - the playground outputs the default GUI component