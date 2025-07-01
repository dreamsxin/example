# Promise

Promise 是一个 ECMAScript 6 提供的类，目的是更加优雅地书写复杂的异步任务。

Promise 是 JavaScript 中用于处理异步操作的对象，它代表一个异步操作的最终完成（或失败）及其结果值。

简单来说，Promise 是一个"承诺"，表示将来某个时间点会返回一个结果（可能是成功的结果，也可能是失败的原因）。

## Promise 有三种状态：

pending：初始状态，既不是成功，也不是失败状态
fulfilled：意味着操作成功完成
rejected：意味着操作失败

Promise 构造函数接受一个函数作为参数，该函数是同步的并且会被立即执行，所以我们称之为起始函数。起始函数包含两个参数 resolve 和 reject，分别表示 Promise 成功和失败的状态。

```js
const myPromise = new Promise((resolve, reject) => {
  // 异步操作代码
 
  if (/* 操作成功 */) {
    resolve('成功的结果'); // 将 Promise 状态改为 fulfilled
  } else {
    reject('失败的原因'); // 将 Promise 状态改为 rejected
  }
});
```

## sleep

```js
 const sleep = (ms) => new Promise((resolve) => setTimeout(resolve, ms));
```
