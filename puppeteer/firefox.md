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
  browserURL: "http://127.0.0.1:63656",
});
const page = await firefoxBrowser.newPage();

await page.goto("https://www.baidu.com")
```
