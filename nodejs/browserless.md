
## 启动

```js
export const availableBrowsers = Promise.all([
  exists(playwright.chromium.executablePath()),
  exists(playwright.firefox.executablePath()),
  exists(playwright.webkit.executablePath()),
  exists(chromeExecutablePath()),
  exists(edgeExecutablePath()),
]).then(
  ([chromiumExists, firefoxExists, webkitExists, chromeExists, edgeExists]) => {
    const availableBrowsers = [];

    if (chromiumExists) {
      availableBrowsers.push(ChromiumCDP, ChromiumPlaywright);
    }

    if (chromeExists) {
      availableBrowsers.push(ChromeCDP, ChromePlaywright);
    }

    if (firefoxExists) {
      availableBrowsers.push(FirefoxPlaywright);
    }

    if (webkitExists) {
      availableBrowsers.push(WebKitPlaywright);
    }

    if (edgeExists) {
      availableBrowsers.push(EdgeCDP, EdgePlaywright);
    }

    return availableBrowsers;
  },
);
```
- browserless\src\browsers\browsers.cdp.ts
- browserless\src\browsers\browsers.playwright.ts

## Docker

```shell
docker run -p 3000:3000 ghcr.io/browserless/chromium
docker run -d \
  -p 3000:3000 \
  -e HTTP_PROXY="http://your_proxy_address:proxy_port" \
  -e HTTPS_PROXY="http://your_proxy_address:proxy_port" \
  -e NO_PROXY="localhost,127.0.0.1" \
  browserless/chromium

docker exec -it <容器ID> /bin/bash -c 'export HTTP_PROXY="http://user:pass@proxy:port"'
```

## debugger

- https://chrome.browserless.io/debugger/

进入容器运行

```shell
$ npm run build
$ npm run install:debugger #or npm install:dev
```

`docker run -p 3001:3001 ghcr.io/browserless/multi`
