```js
import puppeteer from 'puppeteer';

const firefoxBrowser = await puppeteer.launch({
  browser: 'firefox', // WebDriver BiDi is used by default in Firefox.
  // executablePath: '',
  headless: false, // 关闭无头模式
  // ignoreHTTPSErrors: false, // 在导航期间忽略 HTTPS 错误
  args: [
    "--profile","profile-default",
  ]
});
const page = await firefoxBrowser.newPage();

await page.goto("https://www.baidu.com")
```

```js
import puppeteer from 'puppeteer';

const firefoxBrowser = await puppeteer.connect({
  browserURL: "http://127.0.0.1:55176",
});
firefoxBrowser.on('targetcreated', function (target) {
  console.log(target.type() + ' was created');
});
firefoxBrowser.on('targetdestroyed', function (target) {
 console.log(target.type() + ' was destroyed');
});

const page = await firefoxBrowser.newPage();

await page.goto("https://www.baidu.com")
```

## webDriverBiDi
```js
import puppeteer from 'puppeteer';

const firefoxBrowser = await puppeteer.connect({
  //browserURL: "http://127.0.0.1:57107",
  browserWSEndpoint: "ws://127.0.0.1:57107/session",
  protocol: "webDriverBiDi",
});

// const firefoxBrowser = await puppeteer.launch({
//   browser: "firefox",
//   executablePath: "D:\\Program Files\\Mozilla Firefox\\firefox.exe",
// });

// firefoxBrowser.tar

firefoxBrowser.on('targetcreated', function (target) {
  console.log(target.type() + ' was created');
});
firefoxBrowser.on('targetdestroyed', function (target) {
 console.log(target.type() + ' was destroyed');
});
firefoxBrowser.on('targetactivated', function (target) {
 console.log(target.type() + ' was activated');
});
firefoxBrowser.on('targetchanged', function (target) {
 console.log(target.type() + ' was targetchanged');
});

const page = await firefoxBrowser.newPage();

await page.goto("https://www.baidu.com")
// 获取页面内容
const content = await page.content();
console.log(content);
```
