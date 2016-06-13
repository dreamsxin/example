# 安装`grunt-init`
```shell
npm install -g grunt-init
```

## 用法
- 使用grunt-init --help来获取程序帮助以及列出可用模板清单
- 使用grunt-init TEMPLATE并基于可用模板创建一个项目
- 使用grunt-init /path/to/TEMPLATE基于任意其他目录中可用的模板创建一个项目

注意，大多数的模板都应该在当前目录(执行命令的目录)中生成它们的文件(自动生成的项目相关的文件)，因此，如果你不想覆盖现有的文件，注意一定要切换到一个新目录中来保证文件生成到其他目录。

## 安装模板

一旦模板被安装到你的~/.grunt-init/目录中(在Windows平台是%USERPROFILE%\.grunt-init\目录)，那么就可以通过grunt-init命令来使用它们了。建议你使用git将模板克隆到项目目录中。
例如, grunt-init-jquery模板可以像下面这样安装：

```shell
git clone https://github.com/gruntjs/grunt-init-jquery.git ~/.grunt-init/jquery
```

注意：如果你希望在本地像"foobarbaz"这样使用模板，你应该指定~/.grunt-init/foobarbaz之后再克隆。grunt-init会使用实际在于~/.grunt-init/目录中的实际的目录名。

# 安装 Grunt CLI
```shell
npm install -g grunt-cli
```

# 编写 `package.json`
```json
{
  "name": "my-project-name",
  "version": "0.1.0",
  "devDependencies": {
    "grunt": "~0.4.5",
    "grunt-contrib-jshint": "~0.10.0",
    "grunt-contrib-nodeunit": "~0.4.1",
    "grunt-contrib-uglify": "~0.5.0"
  }
}
```

# 安装 grunt
```shell
npm install
```

如果没有 `package.json`，则使用命令 `npm init` 自动生成 `package.json`。

如果已有的package.json文件不包括Grunt模块，可以在直接安装Grunt模块的时候，加上--save-dev参数，该模块就会自动被加入package.json文件。

```shell
npm install grunt --save-dev
npm install grunt-contrib-jshint --save-dev
npm install grunt-contrib-concat --save-dev
npm install grunt-contrib-uglify --save-dev
npm install grunt-contrib-watch --save-dev
```

- `npm install module-name -save` 自动把模块和版本号添加到dependencies部分
- `npm install module-name -save-dve` 自动把模块和版本号添加到devdependencies部分
- `npm install --production` 只自动安装package.json中dependencies部分的模块
- `npm install` package.json中指定的dependencies和devDependencies都会被自动安装进来。

# 编写 Gruntfile.js`