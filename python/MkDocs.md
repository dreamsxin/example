# MkDocs 使用教程。

MkDocs 是一个快速、简单、非常漂亮的静态站点生成器，专门用于构建项目文档。文档源文件使用 Markdown 编写，并通过一个 YAML 配置文件进行配置。

# Welcome to MkDocs

For full documentation visit [mkdocs.org](https://www.mkdocs.org).

## Commands

* `mkdocs new [dir-name]` - Create a new project.
* `mkdocs serve` - Start the live-reloading docs server.
* `mkdocs build` - Build the documentation site.
* `mkdocs -h` - Print help message and exit.

- https://github.com/mondeja/mkdocs-include-markdown-plugin

```shell
pip install mkdocs
pip install mkdocs-material
pip install mkdocs-include-markdown-plugin
```

## 运行

```shell
mkdocs serve
```

## 编译

```shell
mkdocs build
```

---

1. 安装 MkDocs

首先，确保你的系统已安装 Python 和 pip。然后通过 pip 安装 mkdocs 包。

```bash
# 安装 MkDocs
pip install mkdocs

# 验证安装是否成功
mkdocs --version
```

2. 创建新项目

使用 new 命令创建一个新的 MkDocs 项目。这里我们将项目命名为 my-docs。

```bash
mkdocs new my-docs
```

这个命令会创建一个名为 my-docs 的目录，其结构如下：

```
my-docs/
    docs/
        index.md
    mkdocs.yml
```

· docs/： 这是存放所有 Markdown 源文件的文件夹。
· index.md： 是文档的首页。
· mkdocs.yml： 是项目的配置文件。

3. 了解核心配置文件：mkdocs.yml

项目的所有行为都由 mkdocs.yml 文件控制。让我们先看一下初始内容：

```yaml
site_name: My Docs
```

这是一个最简单的配置。让我们添加一些常用设置，让它变得更强大。

```yaml
site_name: 我的 API 文档
site_description: 这是一个非常棒的 API 项目文档
site_author: 你的名字

# 主题设置
theme:
  name: material  # 使用最流行的 Material for MkDocs 主题

# 插件
plugins:
  - search  # 默认启用搜索插件

# 导航菜单
nav:
  - 首页: index.md
  - 用户指南:
      - 安装: user-guide/installation.md
      - 快速开始: user-guide/getting-started.md
  - API 参考:
      - 认证: api-reference/authentication.md
      - 用户接口: api-reference/users.md

# 扩展 Markdown 功能
markdown_extensions:
  - admonition  # 支持警告/提示框
  - codehilite  # 代码高亮
  - toc:
      permalink: true  # 为标题添加永久链接
```

关键配置项解释：

· site_name： 网站标题。
· theme： 指定主题。material 主题功能丰富且美观，是事实上的标准。
· nav： 非常重要！ 手动定义网站的导航结构。如果不定义，MkDocs 会使用 docs 目录的文件结构自动生成。
· plugins： 启用插件以扩展功能。
· markdown_extensions： 启用额外的 Markdown 语法，如警告框、表格等。

4. 编写文档内容

在 docs/ 目录下创建 .md 文件。例如，我们根据上面的导航配置创建文件结构：

```
docs/
    index.md
    user-guide/
        installation.md
        getting-started.md
    api-reference/
        authentication.md
        users.md
```

index.md 示例：

```markdown
# 欢迎来到我的 API 文档

这是一个使用 MkDocs 构建的示例文档。

## 功能特性

*   易于编写和维护
*   支持 Markdown
*   拥有漂亮的 Material 主题
*   内置搜索功能

## 快速开始

请查看 [快速开始指南](user-guide/getting-started.md)。
```

api-reference/users.md 示例（展示 API 文档常用语法）：

```markdown
# 用户接口

此模块包含所有与用户相关的 API 接口。

## 获取用户列表

`GET /api/v1/users`

### 请求参数

| 参数名 | 类型 | 必填 | 描述 |
| :--- | :--- | :--- | :--- |
| `page` | `integer` | 否 | 页码，默认为 1 |
| `limit` | `integer` | 否 | 每页数量，默认为 20 |

### 响应示例

```json
{
  "code": 200,
  "data": {
    "users": [
      {
        "id": 1,
        "name": "张三",
        "email": "zhangsan@example.com"
      }
    ],
    "total": 1
  }
}
```

错误码

!!! warning "注意"
当用户未认证时，会返回 401 错误。

错误码 描述
401 未授权
500 服务器内部错误

```

**注意：** 上面的 `!!! warning "注意"` 是 `admonition` 扩展提供的语法，用于创建醒目的提示框。

### 5. 使用 Material 主题

Material for MkDocs 极大地增强了 MkDocs 的功能和外观。你需要单独安装它：

```bash
pip install mkdocs-material
```

安装后，在 mkdocs.yml 中启用它（如上一步配置所示）。你还可以进行大量自定义：

```yaml
theme:
  name: material
  palette:
    # 主色调
    - scheme: default
      primary: indigo
      accent: blue
      toggle:
        icon: material/brightness-7
        name: 切换到暗色模式
    # 暗色模式
    - scheme: slate
      primary: black
      accent: blue
      toggle:
        icon: material/brightness-4
        name: 切换到亮色模式
  features:
    - navigation.tabs # 导航标签
    - navigation.sections # 导航分区
    - toc.follow # 目录跟随
    - search.suggest # 搜索建议
```

6. 预览和构建

本地预览

在项目根目录（my-docs/）下运行：

```bash
mkdocs serve
```

这将启动一个本地开发服务器，地址通常是 http://127.0.0.1:8000。任何对 mkdocs.yml 或 docs/ 下文件的更改都会自动触发热重载，你在浏览器中能立即看到变化。 这是开发时最常用的命令。

构建静态站点

当你对文档满意后，可以将其构建为静态文件，以便部署到任何 Web 服务器。

```bash
mkdocs build
```

这会在项目根目录下创建一个 site/ 文件夹，里面包含了所有可供发布的 HTML、CSS 和 JavaScript 文件。

7. 部署

部署到 GitHub Pages（最常用）

这是最简单、免费的部署方式。

1. 确保你的项目已是一个 Git 仓库，并已推送到 GitHub。
2. 在项目根目录下，运行以下命令：
   ```bash
   mkdocs gh-deploy
   ```

这个命令会：

· 自动执行 mkdocs build。
· 将构建好的 site/ 目录内容推送到你 GitHub 仓库的 gh-pages 分支。
· 完成后，你可以在 https://<你的用户名>.github.io/<你的仓库名> 访问你的文档。

手动部署

你也可以将 build 生成的 site/ 文件夹里的所有内容，上传到你自己的 Web 服务器（如 Nginx, Apache）上。

进阶功能

集成 Swagger UI / ReDoc

你可以在 MkDocs 页面中嵌入 Swagger UI 来展示 OpenAPI 规范。

1. 将你的 openapi.yaml 或 openapi.json 文件放在 docs/ 目录下，例如 docs/api-spec/openapi.yaml。
2. 创建一个新的 Markdown 文件，如 docs/api.md。
3. 在该文件中使用 HTML 嵌入：
   ```html
   <div id="swagger-ui"></div>
   <link href="https://cdn.jsdelivr.net/npm/swagger-ui-dist@3/swagger-ui.css" rel="stylesheet">
   <script src="https://cdn.jsdelivr.net/npm/swagger-ui-dist@3/swagger-ui-bundle.js"></script>
   <script>
       SwaggerUIBundle({
           url: '../api-spec/openapi.yaml', // 指向你的 OpenAPI 文件
           dom_id: '#swagger-ui',
           presets: [
               SwaggerUIBundle.presets.apis,
               SwaggerUIBundle.SwaggerUIStandalonePreset
           ],
           layout: "BaseLayout"
       });
   </script>
   ```

注意： 在 mkdocs serve 模式下，由于本地文件协议限制，这种方式可能无法正常加载 YAML 文件。但在 build 后部署到服务器上可以正常工作。更推荐的方法是使用专门的 MkDocs 插件（如 mkdocs-swagger-ui-tag）。

总结

使用 MkDocs 构建文档的基本流程：

1. 安装: pip install mkdocs mkdocs-material
2. 创建项目: mkdocs new my-docs
3. 配置: 编辑 mkdocs.yml，设置站点名称、导航、主题等。
4. 编写: 在 docs/ 目录下用 Markdown 编写文档。
5. 预览: mkdocs serve 在本地实时预览。
6. 部署: mkdocs gh-deploy 一键部署到 GitHub Pages。

MkDocs 结合 Material 主题，为你提供了一个功能强大、外观专业且完全免费的文档解决方案，非常适合 API 文档、项目手册等。
