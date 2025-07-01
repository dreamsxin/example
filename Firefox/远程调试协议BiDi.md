#

```shell
firefox.exe --remote-debugging-port 9989 -remote-allow-origins *
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
