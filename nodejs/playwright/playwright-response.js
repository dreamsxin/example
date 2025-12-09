import { chromium } from 'playwright';
import fs from 'fs';
import ProxyChain from "proxy-chain";

await (async () => {

    const browser = await chromium.launch({
        headless: false,
        //executablePath: "E:\\soft\\browser\\ungoogled-chromium_138.0.7204.183-1.1_windows_x64\\ungoogled-chromium_138.0.7204.183-1.1_windows_x64\\chrome.exe",
        executablePath: "E:\\soft\\browser\\ungoogled-chromium_139.0.7258.154-1.1_windows_x64\\chrome.exe",
        // 默认打开调试工具
        devtools: true,
        //args: ['--lang=en-US','--accept-lang=en-US,zh-CN'],
    });

    const context = await browser.newContext({
        // extraHTTPHeaders: {
        //   'sec-ch-ua': '"Google Chrome";v="117", "Not;A=Brand";v="8", "Chromium";v="117"',
        //   'sec-ch-ua-mobile': '?0',
        //   'sec-ch-ua-platform': '"Windows"',
        //   'sec-ch-ua-full-version': '117.0.5938.132',
        //   'sec-ch-ua-full-version-list': '"Google Chrome";v="117.0.5938.132", "Not;A=Brand";v="8.0.0.0", "Chromium";v="117.0.5938.132"',
        //   'sec-ch-ua-arch': '"x86"',
        //   'sec-ch-ua-bitness': '"64"',
        //   'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/117.0.0.0 Safari/537.36'
        // }
    });

    try {
        fs.unlinkSync('107.html');
    } catch (err) {
        console.error('删除文件时出错:', err);
    }
    try {
        fs.unlinkSync('107-1.html');
    } catch (err) {
        console.error('删除文件时出错:', err);
    }
    try {
        fs.unlinkSync('107-json.html');
    } catch (err) {
        console.error('删除文件时出错:', err);
    }
    const page = await context.newPage();
    page.on('response', async response => {
        // 通常我们只关心主文档的响应，即document类型
        //console.log(response.url());
        if (response.url().includes('/search?tbm=map')) {
            console.log(new Date().toString(), response.url());
            const originalHtml = await response.text();
            fs.writeFileSync('107-json.html', originalHtml); 
        } else if (response.request().resourceType() === 'document') {
            console.log(new Date().toString(), response.url());
            const originalHtml = await response.text();
            fs.writeFileSync('107.html', originalHtml); 
        }
    });

    // 导航到网站
    await page.goto('https://www.google.com/maps/place/coffee/@22.3939977,113.9714153,17z/data=!4m5!3m4!1s0x89c259a61c75684f:0x79d31adb123348d2!8m2!3d40.7457399!4d-73.9882272'); // , { waitUntil: 'domcontentloaded' }


    const content = await page.content();
    fs.writeFileSync('107-1.html', content);
    //console.log(content);    
    //await page.close();
    //await browser.close();
})();
console.log('完成');
