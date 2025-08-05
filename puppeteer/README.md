https://github.com/puppeteer/puppeteer

##

```shell
npm init -y
npm i puppeteer
npm i puppeteer-firefox
npm i puppeteer-core
```

```js
import puppeteer from 'puppeteer';

const browser = await puppeteer.connect({
  browserWSEndpoint: "ws://xx.xx.xx.xx/xx"
});

const context = await browser.createIncognitoBrowserContext();
const page = await context.newPage();
console.log(page);
//  // 获取页面内容
//  //const content = await page.content();
// // console.log(content);
try {
await page.goto("https://www.google.com", {timeout: 10000});
} catch (error) {
  console.error('Navigation error:', error);
}
```

## Demo
文件名 `main.mjs` （可以不用定义 "type": "module"）：
```js
import puppeteer from 'puppeteer';
// Or import puppeteer from 'puppeteer-core';

// Launch the browser and open a new blank page
const browser = await puppeteer.launch({
  headless: false,
  product: 'firefox', // or 'chrome'
  protocol: 'webDriverBiDi',
  executablePath: 'D:/mozilla-source/mozilla-unified/obj-x86_64-pc-windows-msvc/dist/bin/firefox.exe',
});

const wsEndpoint = browser.wsEndpoint();
const url = new URL(wsEndpoint);
const port = url.port;
 
console.log('Puppeteer is listening on port:', port);

const page = await browser.newPage();

// Navigate the page to a URL.
await page.goto('https://developer.chrome.com/');

// Set screen size.
await page.setViewport({width: 1080, height: 1024});

// Type into search box.
await page.locator('.devsite-search-field').fill('automate beyond recorder');

// Wait and click on first result.
await page.locator('.devsite-result-item-link').click();

// Locate the full title with a unique string.
const textSelector = await page
  .locator('text/Customize and automate')
  .waitHandle();
const fullTitle = await textSelector?.evaluate(el => el.textContent);

// Print the full title.
console.log('The title of this blog post is "%s".', fullTitle);

await browser.close();
```

`D:\mozilla-source\mozilla-unified\obj-x86_64-pc-windows-msvc\dist\bin\firefox.exe --allow-pre-commit-input --disable-background-networking --disable-background-timer-throttling --disable-backgrounding-occluded-windows --disable-breakpad --disable-client-side-phishing-detection --disable-component-extensions-with-background-pages --disable-default-apps --disable-dev-shm-usage --disable-extensions --disable-hang-monitor --disable-infobars --disable-ipc-flooding-protection --disable-popup-blocking --disable-prompt-on-repost --disable-renderer-backgrounding --disable-search-engine-choice-screen --disable-sync --enable-automation --export-tagged-pdf --generate-pdf-document-outline --force-color-profile=srgb --metrics-recording-only --no-first-run --password-store=basic --use-mock-keychain --disable-features=Translate,AcceptCHFrame,MediaRouter,OptimizationHints,ProcessPerSiteUpToMainFrameThreshold,IsolateSandboxedIframes --enable-features=PdfOopif about:blank --remote-debugging-port=0 --user-data-dir=C:\Users\ADMINI~1\AppData\Local\Temp\puppeteer_dev_chrome_profile-M7r3Fy`
