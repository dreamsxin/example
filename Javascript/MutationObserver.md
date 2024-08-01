# MutationObserver

用来监视 DOM 变动。DOM 的任何变动，比如节点的增减、属性的变动、文本内容的变动都会触发MutationObserver事件。
但是，它与事件有一个本质不同：事件是同步触发，也就是说，DOM 的变动立刻会触发相应的事件；Mutation Observer 则是异步触发，DOM 的变动并不会马上触发，而是要等到当前所有 DOM 操作都结束才触发。
Mutation Observer 有以下特点：

它等待所有脚本任务完成后，才会运行（即异步触发方式）。
它把 DOM 变动记录封装成一个数组进行处理，而不是一条条个别处理 DOM 变动。
它既可以观察 DOM 的所有类型变动，也可以指定只观察某一类变动。


```js
// 创建观察者实例并开始观察整个文档的变化
const observer = new MutationObserver(observerCallback);
observer.observe(document, { childList: true, subtree: true });

// 为了确保当页面首次加载时也执行该代码，可以调用observerCallback函数
observerCallback();
```

## MutationObserver 的实例方法
### observe(node, config)
启动监听，它接受两个参数。
第一个参数：所要观察的 DOM 节点
第二个参数：一个配置对象，指定所要观察的特定变动
配置对象如下：
```js
childList：子节点的变动（指新增，删除或者更改）。
attributes：属性的变动。
characterData：节点内容或节点文本的变动。
subtree：布尔值，表示是否将该观察器应用于该节点的所有后代节点。
attributeOldValue：布尔值，表示观察attributes变动时，是否需要记录变动前的属性值。
characterDataOldValue：布尔值，表示观察characterData变动时，是否需要记录变动前的值。
attributeFilter：数组，表示需要观察的特定属性（比如[‘class’,‘src’]）。
// 开始监听文档根节点（即<html>标签）的变动
mutationObserver.observe(document.documentElement, {
  attributes: true,
  characterData: true,
  childList: true,
  subtree: true,
  attributeOldValue: true,
  characterDataOldValue: true
});
```
对一个节点添加观察器，就像使用addEventListener方法一样，多次添加同一个观察器是无效的，回调函数依然只会触发一次。但是，如果指定不同的options对象，就会被当作两个不同的观察器。

### disconnect()
disconnect方法用来停止观察。调用该方法后，DOM 再发生变动，也不会触发观察器。

### takeRecords()
用来清除变动记录，即不再处理未处理的变动。该方法返回变动记录的数组。
