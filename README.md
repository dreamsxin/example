- https://github.com/dreamsxin/AllView
- https://github.com/dreamsxin/OnLook
- https://github.com/dreamsxin/MyleftCenter
- https://github.com/dreamsxin/MyleftBrowser
- https://github.com/dreamsxin/as3chat
- https://github.com/dreamsxin/ARPGClient
- https://github.com/dreamsxin/cphalcon7

团队管理
========

## 团队管理

* 统一思想（目标一致、认知一致：需求分析、任务分解、预估难点与变数）
* 统一方法（比如统一遵循测试驱动开发，代码注释、日志格式、任务跟踪、开发流程、测试流程、发布流程）
* 统一工具（开发环境、IDE）
* 统一代码管理模式（版本、分支）
* 统一软件架构（从整体到局部）

### Git 使用

1. GitHub flow

GitHub flow 只使用两类分支：master、develop，并依托于 GitHub 的 pull request 功能。
在 GitHub flow 中，master 分支中包含稳定的代码。该分支已经或即将被部署到生产环境。
master 分支的作用是提供一个稳定可靠的代码基础。任何开发人员都不允许把未测试或未审查的代码直接提交到 master 分支。

对代码的任何修改，包括 bug 修复、hotfix、新功能开发等都在单独的分支中进行。
不管是一行代码的小改动，还是需要几个星期开发的新功能，都采用同样的方式来管理。
当需要进行修改时，从 master 分支创建一个新的分支。新分支的名称应该简单清晰地描述该分支的作用。所有相关的代码修改都在新分支中进行。
开发人员可以自由地提交代码和 push 到远程仓库。

2. git-flow 

git-flow 围绕的核心概念是版本发布（release）。因此 git-flow 适用于有较长版本发布周期的项目。
有很大数量的项目的发布周期是几个星期甚至几个月。较长的发布周期可能是由于非技术相关的因素造成的，比如人员限制、管理层决策和市场营销策略等。

git-flow 流程中包含 5 类分支，分别是 master、develop、新功能分支（feature）、发布分支（release）和 紧急 bug 的修复（hotfix）。
这些分支的作用和生命周期各不相同。master 分支中包含的是可以部署到生产环境中的代码，这一点和 GitHub flow 是相同的。develop 分支中包含的是下个版本需要发布的内容。从某种意义上来说，develop 是一个进行代码集成的分支。当 develop 分支集成了足够的新功能和 bug 修复代码之后，通过一个发布流程来完成新版本的发布。发布完成之后，develop分支的代码会被合并到 master 分支中。

其余三类分支的描述如表 1所示。这三类分支只在需要时从 develop 或 master 分支创建。在完成之后合并到 develop 或 master 分支。合并完成之后该分支被删除。这几类分支的名称应该遵循一定的命名规范，以方便开发人员识别。

https://github.com/nvie/gitflow


## 评估代码质量

### 度量

- 圈复杂度
	> 它可以精确地测量路径复杂度。通过利用某一方法路由不同的路径，这一基于整数的度量可适当地描述方法复杂度。实际上，过去几年的各种研究已经确定：圈复杂度（或 CC）大于 10 的方法存在很大的出错风险。因为 CC 通过某一方法来表示路径，这是用来确定某一方法到达 100% 的覆盖率将需要多少测试用例的一个好方法。

- 传入耦合 和传出耦合（抽象性、灵活性）
	> 这些基于整数的度量表示几个相关对象（即相互协调以产生行为的对象）。任一度量中高数值表示架构的维护问题：高传入耦合表示对象具有太多职责，而高传出耦合表示对象不够独立。

### 什么是重构？

重构就是改进已经改进的代码的行为。重构是个永不停止的代码编写过程，它的目的是通过结构的改进而提高代码体的可维护性，但却不改变代码的整体行为。

### 主动和被动重构

可以用代码度量很容易地找出可能难以维护的代码。一旦客观地判断出代码中有问题，那么就可以用方便的重构模式改进它。

### 提取方法模式

提取方法（Extract Method） 模式。在这个模式中，方法的一个逻辑部分被移除，并被赋予自己的方法定义。
提取方法模式提供了两个关键好处：

- 原来的方法现在更短了，因此也更容易理解。
- 移走并放在自己方法中的逻辑体现在更容易测试。

### 降低圈复杂度（Cyclomatic complexity）

如果一段源码中不包含控制流语句（条件或决策点），那么这段代码的圈复杂度为1，因为这段代码中只会有一条路径；如果一段代码中仅包含一个if语句，且if语句仅有一个条件，那么这段代码的圈复杂度为2；包含两个嵌套的if语句，或是一个if语句有两个条件的代码块的圈复杂度为3。

Java 使用 PMD 进行代码分析，获得圈复杂度。
PHP 使用 PHPMD 进行代码分析，获得圈复杂度。

* onefetch
在终端上显示有关 Git 项目的信息。
```shell
sudo add-apt-repository ppa:o2sh/onefetch
sudo apt-get update
sudo apt-get install onefetch
```

* cloc / tokei / git_stats
代码统计

```shell
cloc --exclude-dir="env,docs," .
git_stats generate
```

* gcov
测试代码覆盖率

```shell
gcc -fprofile-arcs -ftest-coverage -o test test.c  
./test  
gcov test.c  
```

* splint
代码静态测试

```shell
splint test.c 
```

* valgrind
内存泄漏测试

```shell
valgrind --tool=memcheck --leak-check=yes ./test 
```

* gprof
```shell
gprof test gmon.out -p性能测试
```

## 性能测试工具

- LoadRunner
	> 可以做录制，业务逻辑复杂的流程
- Curl-Loader
	> 更倾向于压力面的扩展，对于虚拟用户数的个数在单个机器上可以上到10000个以上
- Apache ab

Apache ab判断成功与否只是判断2xx响应代码，不接收服务器的返回值，所以响应以及网络带宽上占有优势，但是Loadrunner要完整接受服务器的返回值，所以在同样的响应时间下，Apache ab测试支持的并发用户数会大于Loadrunner,TPS值也相应的会比Loadrunner大。

## 流程图工具

https://inkscape.org/

https://pencil.evolus.vn/

https://umbrello.kde.org/

## 谷歌

1、双引号

把搜索词放在双引号中，代表完全匹配搜索，也就是说搜索结果返回的页面包含双引号中出现的所有的词，连顺序也必须完全匹配。
Google 和 Baidu 都支持这个指令。

例如：“seo方法图片”

2、减号

减号代表搜索不包含减号后面的词的页面。使用这个指令时减号前面必须是空格，减号后面没有空格，紧跟着需要排除的词。
Google 和 Baidu 都支持这个指令。

例如：搜索 -引擎

返回的则是包含“搜索”这个词，却不包含“引擎”这个词的结果

3、星号

星号`*`是常用的通配符，也可以用在搜索中。

例如：搜索*擎
其中的*号代表任何文字。返回的结果就不仅包含“搜索引擎”，还包含了“搜索收擎”，“搜索巨擎”等内容。

4、inurl

指令用于搜索查询词出现在 url 中的页面。
Google 和 Baidu 都支持这个指令。

inurl 指令支持中文和英文。

例如：inurl:搜索引擎优化
返回的结果都是网址 url 中包含“搜索引擎优化”的页面。

5、inanchor

指令返回的结果是导入链接锚文字中包含搜索词的页面。

例如：inanchor:点击这里
返回的结果页面本身并不一定包含“点击这里”这四个字，而是指向这些页面的链接锚文字中出现了“点击这里”这四个字。

6、intitle

intitle: 指令返回的是页面 title 中包含关键词的页面。
Google 和 Baidu 都支持这个指令。

使用 intitle 指令找到的文件是更准确的竞争页面。如果关键词只出现在页面可见文字中，而没有出现在 title 中，大部分情况是并没有针对关键词进行优化，所以也不是有力的竞争对手。

7、allintitle

搜索返回的是页面标题中包含多组关键词的文件。

例如：allintitle:SEO 搜索引擎优化
就相当于：intitle:SEO intitle:搜索引擎优化返回的是标题中中既包含“SEO”，也包含“搜索引擎优化”的页面

8、allinurl

与 allintitle 类似。

例如：allinurl:SEO 搜索引擎优化
就相当于：inurl:SEO inurl:搜索引擎优化

9、filetype

用于搜索特定文件格式。
Google 和 Baidu 都支持这个指令。

例如：filetype:pdf SEO
返回的就是包含SEO 这个关键词的所有 pdf 文件。

10、site

是 SEO 最熟悉的高级搜索指令，用来搜索某个域名下的所有文件。

11、linkdomain

指令只适用于雅虎，返回的是某个域名的反向链接。雅虎的反向链接数据还比较准确，是SEO 人员研究竞争对手外部链接情况的重要工具之一。

例如：linkdomain:http://cnseotool.com -site:http://cnseotool.com
得到的就是点石网站的外部链接，因为 `-site:http://cnseotool.com` 已经排除了点石本身的页面，也就是内部链接，剩下的就都是外部链接了。

12、related

指令只适用于Google，返回的结果是与某个网站有关联的页面。
例如：related:http://cnseotool.com
我们就可以得到 Google 所认为的与点石网站有关联的其他页面。这种关联到底指的是什么，Google 并没有明确说明，一般认为指的是有共同外部链接的网站。

把这些指令混合起来使用则更强大。
例如：inurl:gov 减肥
返回的就是url 中包含gov，页面中有“减肥”这个词的页面。

## GeoIP

https://github.com/Loyalsoldier/geoip

## CellId
https://location.services.mozilla.com/downloads
