import { firefox } from 'playwright';

(async () => {
  // 启动浏览器，并添加一些启动参数以增强隐蔽性
  const browser = await firefox.launch({
    executablePath: 'E:\\soft\\camoufox-135.0.1-beta.24-win.x86_64\\camoufox.exe',
    headless: false, // 调试时可设置为 false 观察行为
    args: [], // 额外浏览器参数
    env: {
    },
    firefoxUserPrefs: {
      'permissions.default.image': 2,
    }
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
