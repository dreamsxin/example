
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
