# 自定义 Bootstrap4

可以在线修改参数 http://getbootstrap.com/customize/ ，下面是自己修改重新编译操作过程。

bootstrap4 使用 SASS，参考 https://github.com/twbs/bootstrap-sass 。

SASS 是 CSS3 的一个扩展，增加了规则嵌套、变量、混合、选择器继承等等。通过使用命令行的工具或WEB框架插件把它转换成标准的、格式良好的CSS代码。
SCSS 是 SASS 的新语法，是 Sassy CSS 的简写，是CSS3语法的超集，也就是说所有有效的CSS3样式也同样适合于SASS。

## Bootstrap 源码简要说明

Bootstrap4 里的文件 `*.scss` 分为两种，一种是以下划线开头的如 `_variables.scss`，一种是没有下划线的如 `bootstrap.scss`，这两个的区别是前者表示被导入的文件，默认不会编译成对应的css文件，而后者会编译对应的css文件。

编译整个 scss 目录，我们可以得到四个 css 文件，分为是 `bootstrap.css`，`bootstrap-flex.css`（弹性布局模型：flexbox），`bootstrap-reboot.css`（重置样式），`bootstrap-grid.css`（网格样式），`bootstrap-reboot.css`、`bootstrap-grid.css`这两个可以看作赠送的单独样式，如果某些场景你不想使用整个bootstrap样式，但是又想用它的重置或网格系统，那么可以直接使用这两个文件。

`bootstrap.scss` 里面导入了很多文件，整体分为6大块，分别为：

- variable & mixin：引入变量及mixin文件，其中 _mixin.scss 文件中导入了mixin目录中的所有文件
- reset：引入normalize 及 print文件
- core：引入基础样式文件，如grid，form，table，button等
- component：引入组件文件，如nav，card，breadcrumb等
- component js：引入需要js控制的组件文件，如modal，tooltips等
- utility：引入一些全站的class文件，里面有些通用的class，如clearfix，center-block等

## 获取源码

```shell
git clone https://github.com/twbs/bootstrap.git
```

## 安装 node

```shell
sudo apt install node
```

```shell
sudo apt install npm
```

## 安装 grunt-cli

```shell
sudo npm install -g grunt-cli
```

## 安装依赖包

```shell
npm install
```

## 编译生成 dist

```shell
grunt dist
```