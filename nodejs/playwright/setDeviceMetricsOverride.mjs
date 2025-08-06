import { chromium, devices } from "playwright";
const iPhone = devices["iPhone 6"];

// plugin.setServiceKey('');

// const fingerprint = await plugin.fetch({
//   tags: ['Microsoft Windows', 'Chrome'],
// });

// plugin.useFingerprint(fingerprint);

// 初始化页面信息的辅助函数
async function initializePage(page) {
  try {
    await page.waitForLoadState("networkidle");
    const title = await page.title();
    const url = page.url();
    console.log(`发现现有页面: ${title} (${url})`);
    // 设置页面事件监听
    page.on("titlechanged", async () => {
      const newTitle = await page.title();
      const pageInfo = openPages.find((item) => item.page === page);
      if (pageInfo) {
        console.log(`页面标题已更改: ${newTitle} (${pageInfo.url})`);
        pageInfo.title = newTitle;
      }
    });

    page.on("framenavigated", async (frame) => {
      console.log(`framenavigated`);
      await page.waitForLoadState("networkidle");
      if (frame === page.mainFrame()) {
        const newUrl = page.url();
        const pageInfo = openPages.find((item) => item.page === page);
        if (pageInfo) {
          pageInfo.url = newUrl;
          console.log(`页面URL已更改: ${newUrl} (${pageInfo.title})`);
          // URL变化时更新标题
          pageInfo.page.title().then((newTitle) => {
            console.log(`页面标题已更改2: ${newTitle} (${newUrl})`);
            pageInfo.title = newTitle;
          });
        }
      }
    });

    page.on("close", () => {
      const pageIndex = openPages.findIndex((item) => item.page === page);
      if (pageIndex !== -1) {
        const closedPage = openPages[pageIndex];
        console.log(`页面已关闭: ${closedPage.title} (${closedPage.url})`);
        openPages.splice(pageIndex, 1);
      }

      for (let i = 0; i < openPages.length; i++) {
        console.log("openPages[i].title: " + openPages[i].title);
      }
    });

    return { page, title, url };
  } catch (error) {
    return null;
  }
}

const browser = await chromium.connectOverCDP(
  "ws://192.168.0.99:8182/ws?apiKey=c713602e4b334b16b93a1dce7b6d2d5f"
);
browser.on("disconnected", () => {
  console.log("浏览器与 Playwright 断开连接");
  console.log("断开时间:" + new Date().toString());
});

console.log("启动时间:" + new Date().toString());
//{...iPhone}
//const context = await browser.newContext();
const context = await browser.contexts()[0];

context.on("page", async (newPage) => {
  const pageInfo = await initializePage(newPage);
  // 将页面信息添加到列表
  if (pageInfo != null) {
    openPages.push(pageInfo);
  }
});

let openPages = []; // 初始化打开的页面列表
// 获取并初始化所有现有页面
const existingPages = await context.pages();
for (const page of existingPages) {
  const pageInfo = await initializePage(page);
  if (pageInfo != null) {
    openPages.push(pageInfo);
  }
}

const page = existingPages[0];
page.setExtraHTTPHeaders({
  "User-Agent": iPhone.userAgent
})
// context.setGeolocation()
// context.setTimezone()
await page.setViewportSize(iPhone.viewport);
const client = await page.context().newCDPSession(page);
const viewportSize = iPhone.viewport;
const screenSize = iPhone.viewport;
const isLandscape = screenSize.width > screenSize.height;
var metricsOverride = {
  mobile: !!iPhone.isMobile,
  width: viewportSize.width,
  height: viewportSize.height,
  screenWidth: screenSize.width,
  screenHeight: screenSize.height,
  deviceScaleFactor: iPhone.deviceScaleFactor || 1,
  screenOrientation: !!iPhone.isMobile ? isLandscape ? { angle: 90, type: "landscapePrimary" } : { angle: 0, type: "portraitPrimary" } : { angle: 0, type: "landscapePrimary" },
  dontSetVisibleSize: false
};
console.log("metricsOverride", metricsOverride);
await client.send("Emulation.setDeviceMetricsOverride", metricsOverride);
const metadata = {
  mobile: !!iPhone.isMobile,
  model: "",
  architecture: "arm",
  platform: "iOS",
  platformVersion: "iPhone OS 13",
};
await client.send("Emulation.setUserAgentOverride", {
  userAgent: iPhone.userAgent,
  //acceptLanguage: options.locale,
  userAgentMetadata: metadata
});

page.goto("https://www.baidu.com")
