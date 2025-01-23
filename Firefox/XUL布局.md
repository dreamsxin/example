布局方面，XUL 提供了强大的布局系统，包括：

- 盒子模型 ：用于创建水平或垂直排列的元素。
- 栅格布局 ：类似于 CSS Grid，用于创建复杂的二维布局。
- 弹性盒子模型 ：类似于 CSS Flexbox，用于创建灵活的、响应式的布局。

## 盒子模型

一个基本的盒子模型通过 <box> 元素定义，它允许子元素按照垂直或水平方向排列。默认情况下，<box> 元素的子元素是垂直排列的。通过设置 orient 属性为 "horizontal"，可以将 <box> 元素的子元素水平排列。
为了简化布局，XUL 提供了 <hbox> 和 <vbox> 元素，它们分别代表水平盒子和垂直盒子。

- <vbox> - 垂直盒
- <vbox> 元素用于创建一个垂直排列的容器。这意味着它的子元素会按照它们在文档中出现的顺序，一个接一个地垂直堆叠。每个子元素占据一整行，宽度通常与 <vbox> 容器的宽度相同。

`align="center"` 将子元素在垂直盒子中水平居中，而 `pack="center"` 将子元素在垂直方向上居中。

## 栅格布局
栅格布局（Grid Layout）是一种强大的布局方式，它允许开发者以行和列的形式精确地控制用户界面元素的排列。栅格布局通过 <grid> 元素以及其子元素 <rows> 和 <columns> 来定义，从而创建一个由行和列组成的表格式布局。每个单元格可以包含任何 XUL 元素，如按钮、标签、输入框等。

以下是一个简单的栅格布局示例，展示了如何使用 <grid> 元素创建一个包含两行两列的布局。
```xhtml
<grid>
  <columns>
    <column/>
    <column/>
  </columns>
  <rows>
    <row>
      <button label="Button 1"/>
      <button label="Button 2"/>
    </row>
    <row>
      <button label="Button 3"/>
      <button label="Button 4"/>
    </row>
  </rows>
</grid>
```

`flex="1"` 属性被添加到 `<column>` 元素上，这使得两列的宽度会根据可用空间自动调整，且两列的宽度保持相等。

### 基本的水平弹性布局

```xml
<hbox flex="1">
  <button label="Button 1" flex="1"/>
  <button label="Button 2" flex="2"/>
  <button label="Button 3" flex="1"/>
</hbox>
```
在这个示例中，<hbox> 元素作为弹性容器，其 flex 属性设置为 1，表示它将占据所有可用的水平空间。三个按钮作为弹性项目，它们的 flex 属性决定了它们如何分配这个空间。flex="1" 和 flex="2" 的按钮将根据它们的 flex 值比例分配空间。
