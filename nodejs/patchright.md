- https://github.com/Kaliiiiiiiiii-Vinyzu/patchright
```typescript
import { chromium } from 'patchright';

(async () => {
  // 启动浏览器，并添加一些启动参数以增强隐蔽性
  const browser = await chromium.launch({
    headless: false, // 调试时可设置为 false 观察行为
    args: ['--disable-gpu',
      '--disable-blink-features=AutomationControlled', // 重要：禁用自动化控制特征
      '--disable-web-security', // 可选：禁用Web安全策略，根据需要设置
      '--no-sandbox', // 可选：禁用沙箱，根据需要设置
      '--disable-setuid-sandbox'] // 额外浏览器参数
  });

  // 使用模拟设备的参数创建浏览器上下文
  const context = await browser.newContext({
  });


  // 创建新页面
  const page = await context.newPage();

  // 进一步覆盖navigator.webdriver属性（早期方法，不一定始终有效）
  // await page.addInitScript(() => {
  //   Object.defineProperty(navigator, 'webdriver', {
  //     get: () => undefined
  //   });
  // });

  try {
    // 导航到谷歌
    await page.goto('https://browserscan.net/', { waitUntil: 'domcontentloaded' });


  } catch (error) {
    console.error('An error occurred:', error);
  } finally {
    // 调试时可注释掉close，以便观察浏览器状态
    //await browser.close();
  }
})();
```
