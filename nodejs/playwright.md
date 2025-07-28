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

// const browser = await playwright.chromium.connectOverCDP("ws://localhost")
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

## 上传

```js
import playwright from "playwright";

const browser = await playwright.chromium.connectOverCDP("ws://localhost")

const defaultContext = browser.contexts()[0];
const newPage = defaultContext.pages()[0];

try {

  await newPage.goto("http://localhost/upload");

  const cdpSession = await defaultContext.newCDPSession(newPage);
  const root = await cdpSession.send("DOM.getDocument");

  // Then find our input element
  const inputNode = await cdpSession.send("DOM.querySelector", {
    nodeId: root.root.nodeId,
    selector: "#fileInput",
  });

  // Use DOM.setFileInputFiles CDP command
  const remoteFilePath = `762686.png`;
  await cdpSession.send("DOM.setFileInputFiles", {
    files: [remoteFilePath],
    nodeId: inputNode.nodeId,
  });
} catch (error) {
  console.log(error);
}
```

```js
import playwright from "playwright";
import path from "path";

//const browser = await playwright.chromium.connectOverCDP("wss://cloud-hk.yunlogin.com/ws?apiKey=test1100000000000000000000000009")
const browser = await playwright.chromium.connectOverCDP(
  "ws://192.168.0.99:8182?sessionId=c713602e4b334b16b93a1dce7b6d2d5f&apiKey=c713602e4b334b16b93a1dce7b6d2d5f"
);

const defaultContext = browser.contexts()[0];
const newPage = defaultContext.pages()[0];

try {
  await newPage.goto("https://baidu.com/", { timeout: 20000 });

  const fileChooserPromise = newPage.waitForEvent('filechooser');
  const fileChooser = await fileChooserPromise;
  await fileChooser.setFiles(path.join("./", 'test.webp'));
} catch (error) {
  console.log(error);
}
```

## 下载

```js
import playwright from "playwright";

const browser = await playwright.chromium.connectOverCDP("ws://localhost")

const defaultContext = browser.contexts()[0];
const newPage = defaultContext.pages()[0];

try {
  await newPage.goto("http://localhost:8081/upload.html", { timeout: 20000 });

  const cdpSession = await defaultContext.newCDPSession(newPage);

  await cdpSession.send("Browser.setDownloadBehavior", {
    behavior: "allow",
    //downloadPath: "downloads",
    //eventsEnabled: true,
  });

  const [download] = await Promise.all([
    newPage.waitForEvent("download"),
    newPage.locator("#download").click(),
  ]);

  //await download.saveAs('test' + download.suggestedFilename());
  //download.saveAs("./test.xpi");
  const path = await download.path();
  console.log("Downloaded file path:", path);
} catch (error) {
  console.log(error);
}

```

## 剪贴板

```js
const clipboardContent = await this.page.evaluate(() => navigator.clipboard.readText());

// Grant clipboard permissions to browser context
await context.grantPermissions(["clipboard-read", "clipboard-write"]);
    
// Get clipboard content after the link/button has been clicked
const handle = await page.evaluateHandle(() => navigator.clipboard.readText());
const clipboardContent = await handle.jsonValue();
```

```js
  await page.evaluate(async () => {
            const base64 = `data:image/png;base64, iVBORw0KGgoAAAANSUhEUgAAAAUA
            AAAFCAYAAACNbyblAAAAHElEQVQI12P4//8/w38GIAXDIBKE0DHxgljNBAAO
                9TXL0Y4OHwAAAABJRU5ErkJggg==`;

            const response = await fetch(base64);
            const blob = await response.blob();
            const clipboardImageHolder = document.getElementById("you-are-the-one");
            clipboardImageHolder.focus();

            let pasteEvent = new Event("paste", { bubbles: true, cancelable: true });
            pasteEvent = Object.assign(pasteEvent, {
                clipboardData: {
                    items: {
                        a: {
                            kind: "file",
                            getAsFile() {
                                return new File([blob], "foo.png", { type: blob.type });
                            },
                        },
                    },
                },
            });

            console.log("event", pasteEvent);
            clipboardImageHolder.dispatchEvent(pasteEvent);
        });
```

```js
const context = await browser.newContext({ permissions: ["clipboard-read", "clipboard-write"] });
const page = await context.newPage();

const clipPage = await context.newPage();
await clipPage.goto(imageUrl);
await clipPage.keyboard.press("Control+C");
await page.bringToFront();
await page.keyboard.press("Control+V");
```

## 保存网页

```js
import { chromium } from "playwright";
import fs from "fs";

(async () => {
  const options = {
    headless: false,
  };

  console.log("Starting browser");
  const browser = await chromium.launch(options);
  const context = await browser.newContext();
  const page = await context.newPage();
  await page.goto("https://baidu.com/");

  console.log(await page.content())
  await page.pdf({ path: 'output.pdf', format: 'A4' });

  const session = await page.context().newCDPSession(page)

  const doc = await session.send('Page.captureSnapshot', { format: 'mhtml' });
  console.log(doc);

  // 写入本地文件
  fs.writeFileSync('page.mhtml', doc.data);
  
  await browser.close();
})();
```

## 保存状态

```shell
import { chromium } from "playwright";
import fs from "fs";

(async () => {
  const browser = await chromium.connect(
    "ws://192.168.0.10:9222/df50aef37ede7d58ff16b7f0ce55cdd2"
    // "ws://localhost:9333/3cb8dc4da1449629a95af453f04eeda3"
  );
  const context = await browser.newContext();
  const page = await context.newPage();
  await page.goto("https://baidu.com");
  const state = await context.storageState({ path: "./state.json", indexedDB: true });
  console.log(state);
  await browser.close();
})();
```

## 代码生成

```shell
# 指定语言和输出文件
npx playwright codegen --target python -o search_demo.py https://baidu.com

# 使用特定设备模拟（如 iPhone 13）
npx playwright codegen --device="iPhone 13" https://m.baidu.com
```

```js
import { chromium } from "playwright/test";

(async () => {
  // Make sure to run headed.
  const browser = await chromium.launch({ headless: false });

  // Setup context however you like.
  const context = await browser.newContext({
    /* pass any options */
  });
  await context.route("**/*", (route) => route.continue());

  // Pause the page, and start recording manually.
  const page = await context.newPage();
  await page.pause();
})();
```

## trace（运行追踪）
记录测试执行的详细过程（截图、网络请求、时间线等），用于调试。

### 
```shell
npx playwright show-trace test-results/<测试名>-trace.zip
```

### 手动控制追踪
```
const { chromium } = require('playwright');

(async () => {
  const browser = await chromium.launch();
  const context = await browser.newContext();
  
  // 开始记录
  await context.tracing.start({ 
    screenshots: true, 
    snapshots: true 
  });

  const page = await context.newPage();
  await page.goto('https://example.com');
  
  // ...执行操作...
  
  // 保存追踪文件
  await context.tracing.stop({ 
    path: 'trace.zip' 
  });
  
  await browser.close();
})();
```

## playwright 使用 launchServer
- browserless
```shell
  protected async loadPwVersions(): Promise<void> {
    const { playwrightVersions } = JSON.parse(
      (await fs.readFile('package.json')).toString(),
    );

    this.config.setPwVersions(playwrightVersions);
  }

  public async loadPwVersion(version: string): Promise<typeof playwright> {
    const versions = this.getPwVersions();

    try {
      return await import(versions[version] || versions['default']);
    } catch (err) {
      debug.log('Error importing Playwright. Using default version', err);
      return playwright;
    }
  }

  public async launch(
    launcherOpts: BrowserLauncherOptions,
  ): Promise<playwright.BrowserServer> {
    const { options, pwVersion } = launcherOpts;
    this.logger.info(`Launching ${this.constructor.name} Handler`);

    const opts = this.makeLaunchOptions(options);
    const versionedPw = await this.config.loadPwVersion(pwVersion!);
    const browser = await versionedPw[this.playwrightBrowserType].launchServer({
      ...opts,
      args: opts.args.filter((_) => !!_),
    });
    const browserWSEndpoint = browser.wsEndpoint();

    this.logger.info(
      `${this.constructor.name} is running on ${browserWSEndpoint}`,
    );
    this.running = true;
    this.browserWSEndpoint = browserWSEndpoint;
    this.browser = browser;

    return browser;
  }

```

### 1. 启动 BrowserServer 的代码 

```js
// server.js
const { chromium } = require('playwright');

(async () => {
  // 启动 BrowserServer 实例
  const browserServer = await chromium.launchServer({
    headless: true,      // 无头模式 (true) 或显示浏览器 (false)
    port: 9222,          // 指定 WS 端口（可选，默认自动分配）
    args: ['--disable-gpu'] // 额外浏览器参数
  });

  // 输出 WebSocket 连接端点
  console.log('WebSocket Endpoint:', browserServer.wsEndpoint());
  
  // 保持服务器运行（按 Ctrl+C 退出）
  process.on('SIGINT', () => browserServer.close());
})();
```
自定义 userDataDir
```js
import { chromium } from "playwright";

(async () => {
  // 启动 BrowserServer 实例
  const browserServer = await chromium.launchServer({
    _userDataDir: "/home/ubuntu/work/browserServer/test",
    headless: false,      // 无头模式 (true) 或显示浏览器 (false)
    host: "0.0.0.0",
    port: 9222,          // 指定 WS 端口（可选，默认自动分配）
    args: ['--disable-gpu'] // 额外浏览器参数
  });

  // 输出 WebSocket 连接端点
  console.log('WebSocket Endpoint:', browserServer.wsEndpoint());

  // 保持服务器运行（按 Ctrl+C 退出）
  process.on('SIGINT', () => browserServer.close());
})();
```

### 2. 连接 BrowserServer 的代码

```js
// client.js
const { chromium } = require('playwright');

(async () => {
  // 从 server.js 输出的 WebSocket URL
  const wsEndpoint = 'ws://127.0.0.1:9222/...'; // 替换为实际 endpoint

  // 通过 WebSocket 连接现有浏览器
  const browser = await chromium.connect({ wsEndpoint });

  // 创建新页面
  const page = await browser.newPage();
  
  // 使用页面进行操作
  await page.goto('https://example.com');
  console.log(await page.title());

  // 关闭页面和断开连接
  await page.close();
  await browser.close();
})();
```

**连接参数**

```js
// 高级连接选项示例
await chromium.connect({
  wsEndpoint: 'ws://...',
  timeout: 30000,          // 连接超时（毫秒）
  headers: {               // 添加自定义头（如认证）
    Authorization: 'Bearer TOKEN'
  },
  slowMo: 50               // 操作减速（调试用）
});
```

### 

- playwright\packages\playwright-core\src\remote\playwrightServer.ts
```js
  constructor(options: ServerOptions) {
    /*...*/
    this._wsServer = new WSServer(/*delegate*/{
      onRequest: (request, response) => {
        if (request.method === 'GET' && request.url === '/json') {
          response.setHeader('Content-Type', 'application/json');
          response.end(JSON.stringify({
            wsEndpointPath: this._options.path,
          }));
          return;
        }
        response.end('Running');
      },

      onUpgrade: (request, socket) => {
        const uaError = userAgentVersionMatchesErrorMessage(request.headers['user-agent'] || '');
        if (uaError)
          return { error: `HTTP/${request.httpVersion} 428 Precondition Required\r\n\r\n${uaError}` };
      },

      onHeaders: headers => {
        if (process.env.PWTEST_SERVER_WS_HEADERS)
          headers.push(process.env.PWTEST_SERVER_WS_HEADERS!);
      },

      onConnection: (request, url, ws, id) => {
        const browserHeader = request.headers['x-playwright-browser'];
        const browserName = url.searchParams.get('browser') || (Array.isArray(browserHeader) ? browserHeader[0] : browserHeader) || null;
        const proxyHeader = request.headers['x-playwright-proxy'];
        const proxyValue = url.searchParams.get('proxy') || (Array.isArray(proxyHeader) ? proxyHeader[0] : proxyHeader);

        const launchOptionsHeader = request.headers['x-playwright-launch-options'] || '';
        const launchOptionsHeaderValue = Array.isArray(launchOptionsHeader) ? launchOptionsHeader[0] : launchOptionsHeader;
        const launchOptionsParam = url.searchParams.get('launch-options');
        let launchOptions: LaunchOptionsWithTimeout = { timeout: DEFAULT_PLAYWRIGHT_LAUNCH_TIMEOUT };
        try {
          launchOptions = JSON.parse(launchOptionsParam || launchOptionsHeaderValue);
          if (!launchOptions.timeout)
            launchOptions.timeout = DEFAULT_PLAYWRIGHT_LAUNCH_TIMEOUT;
        } catch (e) {
        }

        const isExtension = this._options.mode === 'extension';
        const allowFSPaths = isExtension;
        launchOptions = filterLaunchOptions(launchOptions, allowFSPaths);

        if (process.env.PW_BROWSER_SERVER && url.searchParams.has('connect')) {
          const filter = url.searchParams.get('connect');
          if (filter !== 'first')
            throw new Error(`Unknown connect filter: ${filter}`);
          return new PlaywrightConnection(
              browserSemaphore,
              ws,
              false,
              this._playwright,
              () => this._initConnectMode(id, filter, browserName, launchOptions),
              id,
          );
        }

        if (isExtension) {
          if (url.searchParams.has('debug-controller')) {
            return new PlaywrightConnection(
                controllerSemaphore,
                ws,
                true,
                this._playwright,
                async () => { throw new Error('shouldnt be used'); },
                id,
            );
          }
          return new PlaywrightConnection(
              reuseBrowserSemaphore,
              ws,
              false,
              this._playwright,
              () => this._initReuseBrowsersMode(browserName, launchOptions, id),
              id,
          );
        }

        if (this._options.mode === 'launchServer' || this._options.mode === 'launchServerShared') {
          if (this._options.preLaunchedBrowser) {
            return new PlaywrightConnection(
                browserSemaphore,
                ws,
                false,
                this._playwright,
                () => this._initPreLaunchedBrowserMode(id),
                id,
            );
          }

          return new PlaywrightConnection(
              browserSemaphore,
              ws,
              false,
              this._playwright,
              () => this._initPreLaunchedAndroidMode(id),
              id,
          );
        }

        return new PlaywrightConnection(
            browserSemaphore,
            ws,
            false,
            this._playwright,
            () => this._initLaunchBrowserMode(browserName, proxyValue, launchOptions, id),
            id,
        );
      },
    });
  }
```
- playwright\packages\playwright-core\src\server\utils\wsServer.ts
```js
  async listen(port: number = 0, hostname: string | undefined, path: string): Promise<string> {
    debugLogger.log('server', `Server started at ${new Date()}`);

    const server = createHttpServer(this._delegate.onRequest);
    server.on('error', error => debugLogger.log('server', String(error)));
    this.server = server;

    const wsEndpoint = await new Promise<string>((resolve, reject) => {
      server.listen(port, hostname, () => {
        const address = server.address();
        if (!address) {
          reject(new Error('Could not bind server socket'));
          return;
        }
        const wsEndpoint = typeof address === 'string' ? `${address}${path}` : `ws://${hostname || 'localhost'}:${address.port}${path}`;
        resolve(wsEndpoint);
      }).on('error', reject);
    });

    debugLogger.log('server', 'Listening at ' + wsEndpoint);

    this._wsServer = new wsServer({
      noServer: true,
      perMessageDeflate,
    });

    this._wsServer.on('headers', headers => this._delegate.onHeaders(headers));

    /*http升级到websocket*/
    server.on('upgrade', (request, socket, head) => {
      /*...*/
      this._wsServer!.handleUpgrade(request, socket, head, ws => this._wsServer!.emit('connection', ws, request));
    });

    this._wsServer.on('connection', (ws, request) => {
      /*...*/
      const connection = this._delegate.onConnection(request, url, ws, id); // 实例化 PlaywrightConnection
      (ws as any)[kConnectionSymbol] = connection;
    });

    return wsEndpoint;
  }
```
- playwright\packages\playwright-core\src\remote\playwrightConnection.ts
接收客户端消息
```js
  constructor(semaphore: Semaphore, ws: WebSocket, controller: boolean, playwright: Playwright, initialize: () => Promise<PlaywrightInitializeResult>, id: string) {
    this._ws = ws;
    this._semaphore = semaphore;
    this._id = id;
    this._profileName = new Date().toISOString();

    const lock = this._semaphore.acquire();

    this._dispatcherConnection = new DispatcherConnection();
    this._dispatcherConnection.onmessage = async message => {
      await lock;
      if (ws.readyState !== ws.CLOSING) {
        const messageString = JSON.stringify(message);
        if (debugLogger.isEnabled('server:channel'))
          debugLogger.log('server:channel', `[${this._id}] ${monotonicTime() * 1000} SEND ► ${messageString}`);
        if (debugLogger.isEnabled('server:metadata'))
          this.logServerMetadata(message, messageString, 'SEND');
        ws.send(messageString);
      }
    };
    ws.on('message', async (message: string) => {
      await lock;
      const messageString = Buffer.from(message).toString();
      const jsonMessage = JSON.parse(messageString);
      if (debugLogger.isEnabled('server:channel'))
        debugLogger.log('server:channel', `[${this._id}] ${monotonicTime() * 1000} ◀ RECV ${messageString}`);
      if (debugLogger.isEnabled('server:metadata'))
        this.logServerMetadata(jsonMessage, messageString, 'RECV');
      this._dispatcherConnection.dispatch(jsonMessage);
    });

    /*...*/
  }
```
- playwright\packages\playwright-core\src\server\dispatchers\dispatcher.ts
处理客户端消息
```js
  async _runCommand(callMetadata: CallMetadata, method: string, validParams: any) {
    const controller = new ProgressController(callMetadata, this._object);
    this._activeProgressControllers.add(controller);
    try {
      return await controller.run(progress => (this as any)[method](validParams, progress), validParams?.timeout);
    } finally {
      this._activeProgressControllers.delete(controller);
    }
  }

 async dispatch(message: object) {
    const { id, guid, method, params, metadata } = message as any;
    const dispatcher = this._dispatcherByGuid.get(guid);
    if (!dispatcher) {
      this.onmessage({ id, error: serializeError(new TargetClosedError()) });
      return;
    }

    let validParams: any;
    let validMetadata: channels.Metadata;
    try {
      const validator = findValidator(dispatcher._type, method, 'Params');
      const validatorContext = this._validatorFromWireContext();
      validParams = validator(params, '', validatorContext);
      validMetadata = metadataValidator(metadata, '', validatorContext);
      if (typeof (dispatcher as any)[method] !== 'function')
        throw new Error(`Mismatching dispatcher: "${dispatcher._type}" does not implement "${method}"`);
    } catch (e) {
      this.onmessage({ id, error: serializeError(e) });
      return;
    }

    if (methodMetainfo.get(dispatcher._type + '.' + method)?.internal) {
      // For non-js ports, it is easier to detect internal calls here rather
      // than generate protocol metainfo for each language.
      validMetadata.internal = true;
    }

    const sdkObject = dispatcher._object;
    const callMetadata: CallMetadata = {
      id: `call@${id}`,
      location: validMetadata.location,
      title: validMetadata.title,
      internal: validMetadata.internal,
      stepId: validMetadata.stepId,
      objectId: sdkObject.guid,
      pageId: sdkObject.attribution?.page?.guid,
      frameId: sdkObject.attribution?.frame?.guid,
      startTime: monotonicTime(),
      endTime: 0,
      type: dispatcher._type,
      method,
      params: params || {},
      log: [],
    };

    if (params?.info?.waitId) {
      // Process logs for waitForNavigation/waitForLoadState/etc.
      const info = params.info;
      switch (info.phase) {
        case 'before': {
          this._waitOperations.set(info.waitId, callMetadata);
          await sdkObject.instrumentation.onBeforeCall(sdkObject, callMetadata);
          this.onmessage({ id });
          return;
        } case 'log': {
          const originalMetadata = this._waitOperations.get(info.waitId)!;
          originalMetadata.log.push(info.message);
          sdkObject.instrumentation.onCallLog(sdkObject, originalMetadata, 'api', info.message);
          this.onmessage({ id });
          return;
        } case 'after': {
          const originalMetadata = this._waitOperations.get(info.waitId)!;
          originalMetadata.endTime = monotonicTime();
          originalMetadata.error = info.error ? { error: { name: 'Error', message: info.error } } : undefined;
          this._waitOperations.delete(info.waitId);
          await sdkObject.instrumentation.onAfterCall(sdkObject, originalMetadata);
          this.onmessage({ id });
          return;
        }
      }
    }

    await sdkObject.instrumentation.onBeforeCall(sdkObject, callMetadata);
    const response: any = { id };
    try {
      // If the dispatcher has been disposed while running the instrumentation call, error out.
      if (this._dispatcherByGuid.get(guid) !== dispatcher)
        throw new TargetClosedError(closeReason(sdkObject));
      const result = await dispatcher._runCommand(callMetadata, method, validParams); // 执行客户端命令
      const validator = findValidator(dispatcher._type, method, 'Result');
      response.result = validator(result, '', this._validatorToWireContext());
      callMetadata.result = result;
    } catch (e) {
      if (isTargetClosedError(e)) {
        const reason = closeReason(sdkObject);
        if (reason)
          rewriteErrorMessage(e, reason);
      } else if (isProtocolError(e)) {
        if (e.type === 'closed')
          e = new TargetClosedError(closeReason(sdkObject), e.browserLogMessage());
        else if (e.type === 'crashed')
          rewriteErrorMessage(e, 'Target crashed ' + e.browserLogMessage());
      }
      response.error = serializeError(e);
      // The command handler could have set error in the metadata, do not reset it if there was no exception.
      callMetadata.error = response.error;
    } finally {
      callMetadata.endTime = monotonicTime();
      await sdkObject.instrumentation.onAfterCall(sdkObject, callMetadata);
    }

    if (response.error)
      response.log = compressCallLog(callMetadata.log);
    this.onmessage(response);
  }
}
```
