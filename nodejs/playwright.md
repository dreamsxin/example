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
// 创建一个新的 CDP 会话
const session = await browser.newBrowserCDPSession();


  // 监听 Target.targetCreated 事件
  let gotEvent = false;
  session.on('Target.targetCreated', () => {
    gotEvent = true;
    console.log('Target created event received');
  });

  // 启用目标发现
  await session.send('Target.setDiscoverTargets', { discover: true });

  // 创建一个新页面
  const page = await browser.newPage();

  // 验证是否收到事件
  console.log('Event received:', gotEvent);


  await page.goto("https://www.baidu.com")

  // // 关闭页面
  // await page.close();

  // // 分离 CDP 会话
  // await session.detach();

  // // 关闭浏览器
  // await browser.close();
```
