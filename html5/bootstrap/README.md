Bootstrap 3 是移动设备优先的，在这个意义上，Bootstrap 代码从小屏幕设备（比如移动设备、平板电脑）开始，然后扩展到大屏幕设备（比如笔记本电脑、台式电脑）上的组件和网格。

## 移动设备优先策略

### 内容
决定什么是最重要的。

### 布局
优先设计更小的宽度。基础的 CSS 是移动设备优先，媒体查询是针对于平板电脑、台式电脑。

### 渐进增强

随着屏幕大小的增加而添加元素。

响应式网格系统随着屏幕或视口（viewport）尺寸的增加，系统会自动分为最多12列。

Bootstrap 中 `.container` 是固定宽度，`.container-fluid` 是 100% 宽度。

# 编译 Bootstrap 适合自己的列数

修改 `bootstrap/less/variables.less`

```less
@grid-columns: 16;
```

```shell
npm install -g grunt-cli
cd bootstrap
npm install
grunt dist
```