## 剪贴板

```html
<button id="readClipboard">readClipboard</button>
<img id="showimg"/>
<script>
document.getElementById('readClipboard').addEventListener('click', async () => {
    try {
        const clipboardItems = await navigator.clipboard.read();
        for (let i = 0; i < clipboardItems.length; i++) {
            const item = clipboardItems[i];
			console.log("item", item.types);
            if (item.types.includes('image/png')) {
                const blob = await item.getType('image/png');
				console.log("blob", blob);
                const url = URL.createObjectURL(blob);
				document.getElementById('showimg').src = url;
                console.log(url); // 这里可以得到图片的URL
            }
        }
    } catch (err) {
        console.error('Failed to read clipboard contents: ', err);
    }
});
async function copyImageToClipboard(imageUrl) {
    try {
        // 创建一个img元素并设置src为图片URL，这样浏览器会开始加载图片
        const img = new Image();
        img.src = imageUrl;
        await new Promise((resolve, reject) => {
            img.onload = resolve;
            img.onerror = reject;
        });
 
        // 创建一个canvas元素并绘制图片
        const canvas = document.createElement('canvas');
        const ctx = canvas.getContext('2d');
        canvas.width = img.width;
        canvas.height = img.height;
        ctx.drawImage(img, 0, 0);
 
        // 将canvas内容转换为Data URL
        const dataUrl = canvas.toDataURL('image/png');
        await navigator.clipboard.write([new ClipboardItem({ "image/png": new Blob([dataUrl], { type: 'image/png' }) })]);
        console.log('Image copied to clipboard');
    } catch (err) {
        console.error('Failed to copy image to clipboard: ', err);
    }
}
</script>
```

```js
async function getClipboardContents() {
  try {
    const clipboardItems = await navigator.clipboard.read();
    for (const clipboardItem of clipboardItems) {
      for (const type of clipboardItem.types) {
        const blob = await clipboardItem.getType(type);
        console.log(URL.createObjectURL(blob));
      }
    }
  } catch (err) {
    console.error(err.name, err.message);
  }
}
```

## 拦截 copy 事件
```js

const clipboardItems = [];

document.addEventListener('copy', async (e) => {
  e.preventDefault();
  try {
    let clipboardItems = [];
    for (const item of e.clipboardData.items) { // 从 event 中拿取数据
      if (!item.type.startsWith('image/')) {
        continue;
      }
      clipboardItems.push(
        new ClipboardItem({
          [item.type]: item,
        })
      );
      await navigator.clipboard.write(clipboardItems);
      console.log('Image copied.');
    }
  } catch (err) {
    console.error(err.name, err.message);
  }
});
```

## playwright 操作

```js
import { chromium } from "playwright";
import fs from "fs";



(async () => {
  const browser = await chromium.connectOverCDP(
    "ws://192.168.8.254:8182/ws?apiKey=c713602e4b334b16b93a1dce7b6d2d5f"
  );
  console.log("连接成功");
  const context = await browser.newContext();
  //const context = await browser.contexts()[0];
  // 开始记录
  // await context.tracing.start({
  //   screenshots: true,
  //   snapshots: true
  // });
  // 监听新开页面
  context.on("page", async page => {
    console.log("新开页面", page.url());
  })
  const page = await context.newPage();

  // 访问目标页面
  await page.goto('https://image.baidu.com/');


  // 给剪贴板粘贴图片
  await page.evaluate(async () => {
    // 创建一个canvas元素并绘制图片
    const canvas = document.createElement('canvas');
    const ctx = canvas.getContext('2d');
    canvas.width = 100;
    canvas.height = 100;
    // 画 hello
    ctx.font = '30px serif';
    ctx.fillStyle = 'red';
    ctx.fillText('hello world', 10, 50);

    // 将canvas内容转换为Data URL
    const dataUrl = canvas.toDataURL('image/png');
     // Convert Data URL to binary Blob
     const base64Data = dataUrl.split(',')[1];
     const byteCharacters = atob(base64Data);
     const byteNumbers = new Array(byteCharacters.length);
     for (let i = 0; i < byteCharacters.length; i++) {
       byteNumbers[i] = byteCharacters.charCodeAt(i);
     }
     const byteArray = new Uint8Array(byteNumbers);
     const blob = new Blob([byteArray], { type: 'image/png' });
    await navigator.clipboard.write([new ClipboardItem({ "image/png": blob })]);
    console.log('Image copied to clipboard');
  })
  // 等待 10 s
  console.log("粘贴开始");
  await page.waitForTimeout(1000);
  await page.click('span.img-upload-icon_SRtjV');

  // 发送 ctrl + v 粘贴
  await page.keyboard.press('Control+v');
  console.log("粘贴成功");


  // // 监听弹窗事件
  // page.on('dialog', async dialog => {
  //   console.log(dialog.message()); // 打印警告信息
  //   await dialog.accept(); // 接受警告
  //   // 或者 await dialog.dismiss(); // 拒绝警告
  // });
  //const page = await context.pages()[0];
  // await page.goto("https://match2025.yuanrenxue.cn/match2025/topic/7");
  // console.log(page.url());
  // await page.pdf({ path: "output.pdf", format: "A4" });

  // const session = await page.context().newCDPSession(page);

  // const doc = await session.send("Page.captureSnapshot", { format: "mhtml" });
  // console.log(typeof doc.data);

  // // 写入本地文件
  // fs.writeFileSync("page.mhtml", doc.data);

  // // Start waiting for download before clicking. Note no await.
  // const downloadPromise = page.waitForEvent("download");
  // await page.locator("#download").click();
  // const download = await downloadPromise;

  // console.log("suggestedFilename", download.suggestedFilename());
  // // Wait for the download process to complete and save the downloaded file somewhere.
  // await download.saveAs("./" + download.suggestedFilename());
  // await page.goto("https://baidu.com");
  // const state = await context.storageState({ path: "./state.json", indexedDB: true });
  // console.log(state);

  // 保存追踪文件
  // await context.tracing.stop({
  //   path: 'trace.zip'
  // });
  //await browser.close();
})();
```
