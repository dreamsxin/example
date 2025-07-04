## 下载文件监听

```js
import puppeteer from "puppeteer-core";
import readline from "readline";
import { promises as fsPromises } from "fs";
import fs from "fs";
import path from "path";

// Set the download path
const downloadPath = path.resolve("./downloads");
fs.mkdirSync(downloadPath, { recursive: true });

if (process.platform === "win32") {
  var rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout,
  });

  rl.on("SIGINT", function () {
    process.emit("SIGINT");
  });
}

// 启动本地 Chrome 浏览器

const localbrowser = await puppeteer.launch({
  executablePath:
    "C:/Users/Administrator/AppData/Local/Google/Chrome/Application/chrome.exe",
  headless: false,
  defaultViewport: null, // 设置视口大小
});

const displayPage = await localbrowser.newPage();
await displayPage.goto("about:blank");
await displayPage.evaluate(() => {
  let img = document.createElement("img");
  // img.width = window.innerWidth;
  // img.height = window.innerHeight;
  img.style.maxWidth = window.innerWidth; // 设置图片最大宽度为视口宽度
  document.body.appendChild(img);
  return img;
});
let imgHandle = await displayPage.$("img");

const browser = await puppeteer.connect({
  browserWSEndpoint: "ws://127.0.0.1:9989/session",
  protocol: "webDriverBiDi",
  defaultViewport: null, // 设置视口大小
});

browser.on("targetchanged", async (target) => {
  //console.log("Target changed:", target.url());
});
browser.on("disconnected", () => {
  //console.log("Browser disconnected");
});
browser.on("targetcreated", async (target) => {
  //console.log("Target created:", target.url());
});

let page = await browser.newPage();
page.setDefaultNavigationTimeout(60000); // 60 seconds
page.setDefaultTimeout(30000); // 30 seconds
// 进程退出时关闭浏览器
process.on("exit", async () => {
  console.log("exit...");
});

process.on("SIGINT", async () => {
  console.log("Received SIGINT, exiting gracefully...");
  if (page != null) {
    await page.close();
  }
  process.exit();
});

process.on("SIGTERM", async () => {
  console.log("Received SIGTERM, exiting gracefully...");
  if (page != null) {
    await page.close();
  }
  process.exit();
});

// Enable request interception
// await page.setRequestInterception(true);

// page.on("request", (request) => {
//   // Continue all requests
//   request.continue();
// });

page.on("response", async (response) => {
  // const requestUrl = response.url();

  // if (
  //   response.request().resourceType() === "document" &&
  //   requestUrl.endsWith(".pdf")
  // ) {
  //   const fileName = path.basename(url.parse(requestUrl).pathname);
  //   const filePath = path.resolve(downloadPath, fileName);

  //   const buffer = await response.buffer();
  //   fs.writeFileSync(filePath, buffer);
  //   console.log(`File downloaded to: ${filePath}`);
  // }

  const contentDisposition = response.headers()["content-disposition"];
  // console.log("headers:", response.headers());
  if (contentDisposition && contentDisposition.includes("attachment")) {
    console.log("Response received:", response.url());
    const filename = contentDisposition.split("filename=")[1].replace(/"/g, "");
    const downloadPath = path.join("/path/to/download/directory", filename);

    await new Promise((resolve) => {
      const checkFile = setInterval(() => {
        if (fs.existsSync(downloadPath) && fs.statSync(downloadPath).size > 0) {
          clearInterval(checkFile);
          resolve();
        }
      }, 100);
    });

    console.log(`File downloaded: ${filename}`);
  } else {
    const contentType = response.headers()["content-type"];
    if (contentType && contentType.includes("application/pdf")) {
      console.log("Response received:", response.url());
      const filename = path.basename(response.url());
      const downloadPath = path.join("/path/to/download/directory", filename);

      await new Promise((resolve) => {
        const checkFile = setInterval(async () => {
          if (fs.existsSync(downloadPath) && fs.statSync(downloadPath).size > 0) {
            clearInterval(checkFile);
            resolve();
          }
        }, 100);
      });
    }
  }


});

// 监听 DOMContentLoaded 事件
page.on("domcontentloaded", async () => {
  console.log("页面 DOM 加载完成，开始截图...");

  // 截图
  const base64 = await page.screenshot({ encoding: "base64" });
  await imgHandle.evaluate((img, base64) => {
    img.src = "data:image/png;base64," + base64;
  }, base64);

  await page.evaluate(() => {
    const debounce = (fn, delay) => {
      let timer;
      return async (...args) => {
        clearTimeout(timer);
        return new Promise((resolve) => {
          timer = setTimeout(async () => {
            const result = await fn(...args);
            resolve(result);
          }, delay);
        });
      };
    };

    // 使用示例
    const handleChange = debounce(async () => {
      console.log("------------------DOM changed");
      window.__DOM_CHANGED = true; // 标记变动
      return "DOM changed";
    }, 100);

    console.log("evaluate MutationObserver...");
    new MutationObserver(() => {
      handleChange();
    }).observe(document.body, {
      subtree: true,
      childList: true,
      attributes: true,
    });

    document.addEventListener("mousemove", (e) => {
      handleChange();
    });

    document.addEventListener("mouseover", (e) => {
      handleChange();
    });

    document.addEventListener("mouseout", (e) => {
      handleChange();
    });
  });
});

// 监听 load 事件
page.on("load", async () => {
  console.log("页面 load，开始截图...");
});

async function checkDOM() {
  if (page == null) {
    return;
  }
  const changed = await page.evaluate(() => window.__DOM_CHANGED);
  if (changed) {
    console.log("页面 changed，开始截图...");
    await page.evaluate(() => (window.__DOM_CHANGED = false));
    const base64 = await page.screenshot({ encoding: "base64" });
    await imgHandle.evaluate((img, base64) => {
      img.src = "data:image/png;base64," + base64;
    }, base64);
  }
  setTimeout(checkDOM, 100); // 递归调用实现间隔检查
}

checkDOM();

try {
  // Navigate the page to a URL.
  await page.goto("http://localhost:8080/");


} catch (err) {
  console.log(err);
  // Close the page.
  await page.close();
  page = null;
}
```
