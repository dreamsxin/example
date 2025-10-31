import { chromium } from 'playwright';
import { setTimeout } from 'timers/promises';
import { PROXY_CONFIG, ForwardProxy } from './forward-proxy.js';
import RandomWordGenerator from 'random-word-generator';

// 模拟人类随机延迟
const humanDelay = async (min = 1000, max = 3000) => {
  const delay = Math.floor(Math.random() * (max - min + 1)) + min;
  await setTimeout(delay);
};

(async () => {
  // 计时
  const startTime = Date.now();

  // 启动本地代理服务器
  const proxy = new ForwardProxy(PROXY_CONFIG);
  await proxy.start();

  // 计算启动代理耗时
  const proxyStartTime = Date.now();
  console.log(`代理启动耗时: ${proxyStartTime - startTime}ms`);

  let browser = null;

  try {
    // 启动浏览器，配置代理
    browser = await chromium.launch({
      executablePath: "E:\\soft\\ungoogled-chromium_138.0.7204.183-1.1_windows_x64\\ungoogled-chromium_138.0.7204.183-1.1_windows_x64\\chrome.exe",
      headless: false, // 设置为true则不显示浏览器
      slowMo: 300, // 减慢操作速度
      proxy: {
        server: `http://127.0.0.1:${PROXY_CONFIG.localPort}`,
        // 如果需要代理认证，可以添加：
        // username: 'proxy_user',
        // password: 'proxy_pass'
      },
      args: [
        '--lang=en-US',
        '--accept-lang=en-US,zh-CN',
        '--timezone=Asia/Hong_Kong',
      ]
    });

    const context = await browser.newContext({
      // 可以在这里添加更多上下文配置
    });

    // 循环调用
    const page = await context.newPage();

    // 计算启动浏览器耗时
    const browserStartTime = Date.now();
    console.log(`浏览器启动耗时: ${browserStartTime - startTime}ms`);

    // 随机输入速度模拟
    const typeWithRandomSpeed = async (selector, text) => {
      for (const char of text) {
        await page.type(selector, char, {
          delay: Math.random() * 50
        });
      }
    };

    console.log('访问browserscan.net');
    // 访问Google
    await page.goto('https://www.browserscan.net/');

    await humanDelay(15000, 25000);

    let i = 0;
    let success = 0;
    for (; ;) {
      i++;
      console.log(`第${i}次访问Google`);
      try {
        // 访问Google
        await page.goto('https://www.google.com');

        console.log('需要添加等待确保页面加载完成');
        // 需要添加等待确保页面加载完成
        await page.waitForLoadState('domcontentloaded');

        await humanDelay();

        console.log('输入搜索词');
        // 输入搜索词
        const randomWord = new RandomWordGenerator();

        // 生成随机词
        const keyword = randomWord.generate();
        console.log(keyword);

        await typeWithRandomSpeed('//form//textarea', keyword);

        await humanDelay(500, 1500);

        console.log('随机移动鼠标');
        // 随机移动鼠标
        await page.mouse.move(
          Math.random() * 800 + 200,
          Math.random() * 400 + 100
        );

        console.log('按回车键搜索');
        // 按回车键搜索
        await page.keyboard.press('Enter');
        await page.waitForSelector('body');
        const content = await page.content();
        console.log('页面内容长度:', content.length);
        if (content.length < 10000) {
          console.log(`第${i}次搜索结果为空，继续下一次搜索, success: ${success}`);
          continue;
        }
        success++
        await humanDelay(500, 1500);

        // 随机滚动页面
        await page.evaluate(async () => {
          const scrollHeight = document.body.scrollHeight;
          const scrollStep = scrollHeight / (Math.random() * 10 + 5);
          for (let i = 0; i < scrollHeight; i += scrollStep) {
            window.scrollTo(0, i);
            await new Promise(resolve => setTimeout(resolve, 100));
          }
        });

        // 随机点击结果
        const links = await page.$$('a h3');
        if (links.length > 0) {
          const randomLink = links[Math.floor(Math.random() * links.length)];
          // 滚动到元素所在位置
          await randomLink.scrollIntoViewIfNeeded();

          await humanDelay(1000, 3000);
          await randomLink.click();
          await page.waitForSelector('body');
          await humanDelay(3000, 8000);
        }

        console.log(`第${i}次搜索模拟完成, success: ${success}`);
      } catch (error) {
        console.error('错误:', error);
        console.log(`第${i}次错误, success: ${success}`);
      }
    }

  } catch (error) {
    console.error('执行错误:', error);
  } finally {
    // 关闭浏览器
    if (browser) {
      await browser.close();
    }
    // 停止代理服务器
    await proxy.stop();
  }
})();
