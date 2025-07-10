#

```shell
firefox.exe --remote-debugging-port 9989 -remote-allow-origins *
env MOZCONFIG=mozconfig-rel ./mach run --remote-debugging-port=9989 --headless
```

## 调用流程

- RemoteAgentParentProcess.listen
- new lazy.WebDriverBiDi
- RemoteAgentParentProcess.listen
- new lazy.HttpServer();
- WebDriverBiDi.start
- server.registerPathHandler
- WebDriverNewSessionHandler
- new lazy.WebDriverBiDiConnection
- WebDriverBiDi.addSessionlessConnection()
- WebDriverBiDiConnection.onPacket
- lazy.RemoteAgent.webDriverBiDi.createSession

## remote\components\RemoteAgent.sys.mjs

```js
ChromeUtils.defineLazyGetter(lazy, "activeProtocols", () => {
  const protocols = Services.prefs.getIntPref("remote.active-protocols");
  if (protocols < 1 || protocols > 3) {
    throw Error(`Invalid remote protocol identifier: ${protocols}`);
  }

  return protocols;
});
```
```js
  if (
    (lazy.activeProtocols & WEBDRIVER_BIDI_ACTIVE) ===
    WEBDRIVER_BIDI_ACTIVE
  ) {
    this.#webDriverBiDi = new lazy.WebDriverBiDi(this); // 实例化 WebDriverBiDi
    if (this.#enabled) {
      lazy.logger.debug("WebDriver BiDi enabled");
    }
  }

  if ((lazy.activeProtocols & CDP_ACTIVE) === CDP_ACTIVE) {
    this.#cdp = new lazy.CDP(this);
    if (this.#enabled) {
      lazy.logger.debug("CDP enabled");
    }
  }
```

## remote\webdriver-bidi\WebDriverBiDi.sys.mjs
```js
export class WebDriverBiDi {
  #agent;
  #bidiServerPath;
  #running;
  #session;
  #sessionlessConnections;

  /**
   * Creates a new instance of the WebDriverBiDi class.
   *
   * @param {RemoteAgent} agent
   *     Reference to the Remote Agent instance.
   */
  constructor(agent) {
    this.#agent = agent;
    this.#running = false;

    this.#bidiServerPath;
    this.#session = null;
    this.#sessionlessConnections = new Set();
  }

  async start() {
    if (this.#running) {
      return;
    }

    this.#running = true;

    lazy.RecommendedPreferences.applyPreferences(RECOMMENDED_PREFS);

    // Install a HTTP handler for direct WebDriver BiDi connection requests.
    this.#agent.server.registerPathHandler(
      "/session",
      new lazy.WebDriverNewSessionHandler(this)
    );

    Cu.printStderr(`WebDriver BiDi listening on ${this.address}\n`);
    try {
      // Write WebSocket connection details to the WebDriverBiDiServer.json file
      // located within the application's profile.
      this.#bidiServerPath = PathUtils.join(
        PathUtils.profileDir,
        "WebDriverBiDiServer.json"
      );

      const data = {
        ws_host: this.#agent.host,
        ws_port: this.#agent.port,
      };

      await IOUtils.write(
        this.#bidiServerPath,
        lazy.textEncoder.encode(JSON.stringify(data, undefined, "  "))
      );
    } catch (e) {
      lazy.logger.warn(
        `Failed to create ${this.#bidiServerPath} (${e.message})`
      );
    }
```

## remote\marionette\driver.sys.mjs
```js
GeckoDriver.prototype.newSession = async function (cmd) {
  console.error("------------GeckoDriver.newSession()");
  if (this.currentSession) {
    throw new lazy.error.SessionNotCreatedError(
      "Maximum number of active sessions"
    );
  }

  const { parameters: capabilities } = cmd;

  try {
    if (lazy.RemoteAgent.webDriverBiDi) {
      // If the WebDriver BiDi protocol is active always use the Remote Agent
      // to handle the WebDriver session.
      await lazy.RemoteAgent.webDriverBiDi.createSession(
        capabilities,
        this._sessionConfigFlags
      );
    } else {
      // If it's not the case then Marionette itself needs to handle it, and
      // has to nullify the "webSocketUrl" capability.
      this._currentSession = new lazy.WebDriverSession(
        capabilities,
        this._sessionConfigFlags
      );
      this._currentSession.capabilities.delete("webSocketUrl");
    }
//...
}
```

## 使用 selenium

```python
from selenium import webdriver
from selenium.webdriver.common.bidi.console import Console

options = webdriver.FirefoxOptions()
options.set_capability("webSocketUrl", True)
driver = webdriver.Firefox(options=options)

driver.get("https://example.com")
driver.quit()
```

## 使用 puppeteer
```js
import puppeteer from 'puppeteer-core';

const browser = await puppeteer.connect({
  browserWSEndpoint: "ws://127.0.0.1:9989/session",
  protocol: 'webDriverBiDi'
});

const page = await browser.newPage();

// Navigate the page to a URL.
await page.goto('https://developer.chrome.com/');

// Set screen size.
await page.setViewport({width: 1080, height: 1024});

// Type into search box using accessible input name.
await page.locator('aria/Search').fill('automate beyond recorder');

// Wait and click on first result.
await page.locator('.devsite-result-item-link').click();

// Locate the full title with a unique string.
const textSelector = await page
  .locator('text/Customize and automate')
  .waitHandle();
const fullTitle = await textSelector?.evaluate(el => el.textContent);

// Print the full title.
console.log('The title of this blog post is "%s".', fullTitle);
```

```js
import puppeteer from 'puppeteer-core';
import { promises as fsPromises } from 'fs';

const browser = await puppeteer.connect({
  browserWSEndpoint: "ws://127.0.0.1:9989/session",
  protocol: 'webDriverBiDi'
});

browser.on("targetchanged", async (target) => {
  console.log("Target changed:", target.url());
});
browser.on("disconnected", () => {
  console.log("Browser disconnected");
});
browser.on("targetcreated", async (target) => {
  console.log("Target created:", target.url());
});

const page = await browser.newPage();

// Navigate the page to a URL.
await page.goto('https://www.baidu.com/');

// Print the full title.
const title = await page.title();
console.log('The title of this blog post is "%s".', title);

const screenshotBuffer = await page.screenshot({
  type: 'jpeg',
  quality: 80,
});

await page.close();

console.log(screenshotBuffer);

// 保存到文件
await fsPromises.writeFile('screenshot.jpg', screenshotBuffer);
```

**页面变动截图**
```js
import puppeteer from "puppeteer-core";
import readline from "readline";

if (process.platform === "win32") {
  var rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout,
  });

  rl.on("SIGINT", function () {
    process.emit("SIGINT");
  });
}

// 启动本地 Chrome 浏览器

const localbrowser = await puppeteer.launch({
  executablePath:
    "C:/Users/Administrator/AppData/Local/Google/Chrome/Application/chrome.exe",
  headless: false,
  defaultViewport: null, // 设置视口大小
});

const displayPage = await localbrowser.newPage();
await displayPage.goto("about:blank");
await displayPage.evaluate(() => {
  let img = document.createElement("img");
  // img.width = window.innerWidth;
  // img.height = window.innerHeight;
  img.style.maxWidth = window.innerWidth; // 设置图片最大宽度为视口宽度
  document.body.appendChild(img);
  return img;
});
let imgHandle = await displayPage.$("img");

const browser = await puppeteer.connect({
  browserWSEndpoint: "ws://127.0.0.1:9989/session",
  protocol: "webDriverBiDi",
  defaultViewport: null, // 设置视口大小
});

browser.on("targetchanged", async (target) => {
  //console.log("Target changed:", target.url());
});
browser.on("disconnected", () => {
  //console.log("Browser disconnected");
});
browser.on("targetcreated", async (target) => {
  //console.log("Target created:", target.url());
});

let page = await browser.newPage();

// 进程退出时关闭浏览器
process.on("exit", async () => {
  console.log("exit...");
});

process.on("SIGINT", async () => {
  console.log("Received SIGINT, exiting gracefully...");
  if(page != null) {
    await page.close();
  }
  process.exit();
});

process.on("SIGTERM", async () => {
  console.log("Received SIGTERM, exiting gracefully...");
  if(page != null) {
    await page.close();
  }
  process.exit();
});

// 监听 DOMContentLoaded 事件
page.on("domcontentloaded", async () => {
  console.log("页面 DOM 加载完成，开始截图...");

  // 截图
  const base64 = await page.screenshot({ encoding: "base64" });
  await imgHandle.evaluate((img, base64) => {
    img.src = "data:image/png;base64," + base64;
  }, base64);

  await page.evaluate(() => {
    const debounce = (fn, delay) => {
      let timer;
      return async (...args) => {
        clearTimeout(timer);
        return new Promise((resolve) => {
          timer = setTimeout(async () => {
            const result = await fn(...args);
            resolve(result);
          }, delay);
        });
      };
    };

    // 使用示例
    const handleChange = debounce(async () => {
      console.log("------------------DOM changed");
      window.__DOM_CHANGED = true; // 标记变动
      return "DOM changed";
    }, 100);

    console.log("evaluate MutationObserver...");
    new MutationObserver(() => {
      handleChange();
    }).observe(document.body, {
      subtree: true,
      childList: true,
      attributes: true,
    });

    document.addEventListener("mousemove", (e) => {
      handleChange();
    });

    document.addEventListener("mouseover", (e) => {
      handleChange();
    });

    document.addEventListener("mouseout", (e) => {
      handleChange();
    });
  });
});

// 监听 load 事件
page.on("load", async () => {
  console.log("页面 load，开始截图...");
});

async function checkDOM() {
  if (page == null) {
    return;
  }
  const changed = await page.evaluate(() => window.__DOM_CHANGED);
  if (changed) {
    console.log("页面 changed，开始截图...");
    await page.evaluate(() => (window.__DOM_CHANGED = false));
    const base64 = await page.screenshot({ encoding: "base64" });
    await imgHandle.evaluate((img, base64) => {
      img.src = "data:image/png;base64," + base64;
    }, base64);
  }
  setTimeout(checkDOM, 100); // 递归调用实现间隔检查
}

checkDOM();

try {
  // Navigate the page to a URL.
  await page.goto("https://www.baidu.com/");

  // Print the full title.
  let title = await page.title();
  console.log('The title of this blog post is "%s".', title);
  await page.waitForSelector('input[name="wd"]');
  await page.locator('input[name="wd"]').fill("automate beyond recorder");

  // 获取文本框元素
  const input = await page.$('input[name="wd"]');
  const box = await input.boundingBox();

  // 模拟拖拽选中
  let x = Math.floor(box.x);
  let y = Math.floor(box.y);
  let w = Math.floor(box.width);
  let h = Math.floor(box.height);
  console.log(x, y, w, h, x + w - 10, y + h - 10);
  await page.mouse.move(x + 10, y + 10);
  await page.mouse.down();
  await page.mouse.move(x + w - 10, y + h - 10, {
    steps: 20,
  });
  await page.mouse.up();
} catch (err) {
  console.log(err);
  // Close the page.
  await page.close();
  page = null;
}
```
