import puppeteer from "puppeteer";
import axios from "axios";
import WebSocket from "ws";

//wsUrl = "ws://localhost:9222/devtools/browser/9a838680-ef12-41b0-a18c-89a1df7ec159"

const browser = await puppeteer.connect({
   browserURL: "http://127.0.0.1:51142",
});

var errnum = 0;
const asyncrequest = async (i) => {
  var st = new Date().getTime();
  //console.log(i+"时间:" + new Date().toString());
  const page = await browser.newPage();
  //console.log(page);
  //  // 获取页面内容
  //  //const content = await page.content();
  // // console.log(content);
  try {
    fetch(
      "http://localhost/stats",
      {
        method: "POST",
        headers: {
          "Content-Type": "application/json",
        },
        body: JSON.stringify({
        }),
      }
    )
      .then((response) => {
        return response.json();
      })
      .then((json) => {
        //console.log(json);
      });
    await page.goto("https://www.google.com/search?q=test", { timeout: 20000 });
    // 获取页面内容
    const content = await page.content();
    //console.log(i, content);
    var et = new Date().getTime();
    console.log(i + "耗时:" + (et - st) + "ms", "cookies删除错误", errnum);

    //console.log(i+"时间:" + new Date().toString());
  } catch (error) {
    console.error(i, "Navigation error:", error);
  }

  // // 输入关键词
  // await page.type('textarea', "Playwright");

  // // 点击搜索按钮
  // await page.click('input[type="submit"]');

  //console.log(i, content);
  //console.log(i+"时间:" + new Date().toString());

  // 获取当前页面的所有 cookies

  const cookies = await page.cookies();
  //console.log(cookies);
  // 删除所有 cookies
  cookies.forEach(async (cookie) => {
    //console.log(cookie);
    await page.deleteCookie(cookie).catch((err) => {
      errnum++;
    });
  });
  await page.close();
};

// 生成10个并发任务
var tasklist = [];
for (let i = 0; i < 0; i++) {
  var t = asyncrequest(i);
  tasklist.push(t);
}
// 等待所有任务完成
await Promise.all(tasklist);
console.log("所有任务完成");

for (let i = 0; i < 1000; i++) {
  await asyncrequest(i);
}
