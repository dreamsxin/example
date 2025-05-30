import playwright from "playwright";

const browser = await playwright.chromium.launchPersistentContext("D:\\tmp", {
  headless: false,
  bypassCSP: true,
  ignoreHTTPSErrors: true,
  timezoneId: "Asia/Hong_Kong",
  slowMo: 50,
});

browser.on("disconnected", () => {
  console.log("浏览器与 Playwright 断开连接");
  console.log("断开时间:" + new Date().toString());
});

try {
  // 创建新页面
  const page = await browser.newPage();
  // 监听页面未捕获异常
  page.on("pageerror", (error) => {
    console.error("页面未捕获异常:", error.message);
  });
  // 监听请求失败
  page.on("requestfailed", (request) => {
    console.error("请求失败:", request.url(), request.failure().errorText);
  });
  page.on("console", (msg) => {
    console.log("console:", msg, msg.text());
  });

  console.log("导航至百度首页", new Date().toString());
  // 导航至百度首页
  await page.goto("https://www.baidu.com");

  console.log("等待搜索框可见并输入关键词", new Date().toString());
  // 等待搜索框可见并输入关键词
  const searchBox = page.locator("#kw");
  await searchBox.waitFor({ state: "visible" });
  await searchBox.fill("playwright-go");

  console.log("提交搜索", new Date().toString());
  // 提交搜索（两种方式任选）
  // 方式1：点击搜索按钮
  const searchBtn = page.locator("#su");
  await searchBtn.click();

  // 方式2：使用键盘回车
  // await searchBox.press('Enter');

  // 等待搜索结果加载
  await page.waitForSelector("#content_left", { timeout: 1000 });

  // 获取第一条结果的标题
  // 遍历所有搜索结果并打印标题和链接
  const results = await page.$$("#content_left a");
  for (const result of results) {
    const title = await result.textContent();
    const link = await result.getAttribute("href");
    console.log(`标题: ${title}, 链接: ${link}`);
  }
} finally {
  // 关闭浏览器
  await browser.close();
}
