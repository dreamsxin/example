# Playwright

https://playwright.dev/docs/intro

## 初始化项目

```shell
mkdir demo
cd demo
npm init playwright@latest
```

## Record a YouTube video with Playwright.
```js
// Playwright 1.8.0 | docs: https://playwright.dev/docs/videos
const { chromium } = require('playwright')

const fn = async () => {
  const browser = await chromium.launch({
    headless: false,
    ignoreDefaultArgs: ['--mute-audio'],
    args: ['--autoplay-policy=no-user-gesture-required']
  })
  const page = await browser.newPage({ recordVideo: { dir: 'videos/' } })
  await page.goto('https://www.youtube.com/watch?v=pIWX6hF0C_s')

  await page.waitForTimeout(14000)

  await page.close()
  await browser.close()
}
fn()
```

## 监听事件

```js
import playwright from 'playwright';

//   // 启动 Chrome 浏览器
//   const browser = await playwright.chromium.launch({
//     headless: false // 可以设置为 true 以无头模式运行
//   });

const browser = await playwright.chromium.connectOverCDP("ws://127.0.0.1:63043/devtools/browser/9cc443f0-1ff6-4bc3-a9db-1064cc9605c0")
// 创建一个新的 CDP 会话 CDPSession 对象可以用来与浏览器的 Chrome DevTools Protocol (CDP) 会话进行交互，执行底层 CDP 命令并监视和处理与浏览器相关的事件。
const session = await browser.newBrowserCDPSession();


  // 监听 Target.targetCreated 事件
  let gotEvent = false;
  session.on('Target.targetCreated', () => {
    gotEvent = true;
    console.log('Target created event received');
  });

  // 启用目标发现
  await session.send('Target.setDiscoverTargets', { discover: true });

  //const ctx = await browser.newContext();
  //const ctx = await browser.contexts()[0];
  //const newPage = await ctx.newPage();

  // 创建一个新页面
  const page = await browser.newPage();

  // 验证是否收到事件
  console.log('Event received:', gotEvent);

  const client = await page.context().newCDPSession(page);
  client.on('Page.screencastFrame', async (frameObject) => {
    // logger.log(frameObject.metadata, frameObject.sessionId);

    try {
      const buf = Buffer.from(frameObject.data, 'base64');
      await client.send('Page.screencastFrameAck', { sessionId: frameObject.sessionId });

      //await client.send('Page.stopScreencast');
      console.log("screencastFrame", buf);
    } catch (err) {
      logger.error('Page.screencastFrame', err);
    }
  });

  await client.send('Page.startScreencast', {
    format: 'png',
    quality: 90,
    maxWidth: 1024,
    maxHeight: 768,
    everyNthFrame: 1,
  });


  await page.goto("https://www.baidu.com")

  // // 关闭页面
  // await page.close();

  // // 分离 CDP 会话
  // await session.detach();

  // // 关闭浏览器
  // await browser.close();
```

## 展示示例

```js
import playwright from "playwright";

// 启动 Chrome 浏览器
const browser = await playwright.chromium.launch({
  headless: false, // 可以设置为 true 以无头模式运行
});

// const browser = await playwright.chromium.connectOverCDP("ws://192.168.8.101:8282/?apiKey=014027e40b3e4f10a4118380cdd53136&sessionId=a1ca93a02a1141d0812cf4bc9d1995b5")
// // 创建一个新的 CDP 会话
const session = await browser.newBrowserCDPSession();

// 监听 Target.targetCreated 事件
let gotEvent = false;
session.on("Target.targetCreated", () => {
  gotEvent = true;
  console.log("Target created event received");
});

// 启用目标发现
await session.send("Target.setDiscoverTargets", { discover: true });

// 创建一个新页面
const newPage = await browser.newPage();

const client = await newPage.context().newCDPSession(newPage);

let displayPage = await browser.newPage();
await displayPage.goto("about:blank");
await displayPage.evaluate(() => {
  let img = document.createElement("img");
  img.width = window.innerWidth;
  img.height = window.innerHeight;
  document.body.appendChild(img);
  return img;
});
let imgHandle = await displayPage.$("img");

client.on("Page.screencastFrame", async (ev) => {
  //console.log('screencastFrame', ev);
  await client.send("Page.screencastFrameAck", { sessionId: ev.sessionId });
  const base64 = ev.data;
  await imgHandle.evaluate((img, base64) => {
    img.src = "data:image/png;base64," + base64;
  }, base64);
});
await client.send("Page.startScreencast");

await newPage.goto("https://www.baidu.com");

// // 关闭页面
// await page.close();

// // 分离 CDP 会话
// await session.detach();

// // 关闭浏览器
// await browser.close();
```
