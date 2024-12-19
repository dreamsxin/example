import { chromium } from 'playwright'
import fakeUA from 'fake-ua'

const sounds = [];

// the slowMo option to slow down execution
const browser = await chromium.launch({ headless: false, slowMo: 2000 });
const context = await browser.newContext({ userAgent: fakeUA.pc() });
const page = await context.newPage();

await page.goto('https://www.ximalaya.com/album/291718')

const maxPageNo = page.locator('.sound-list .pagination-page li')
  .filter({ has: page.locator('text=/\\d+/') })
  .last();
const pageCount = Number(await maxPageNo.innerText());

for (let index = 1; index <= pageCount; index++) {
  await getLinks();
  if (index + 1 <= pageCount) {
    await nextPage(index + 1)
  }
}
// await context.close();
// await browser.close();

async function nextPage(pageNo) {
  const input = page.locator('[placeholder="请输入页码"]:visible');
  await input.fill(String(pageNo));
  await page.locator('button >> text=跳转 >> visible=true').click();
}

async function getLinks() {
  const list = await page.$$('.sound-list > ul > li');
  for (const el of list) {
    await showAudioNameAndURL(el)
  }
}

async function showAudioNameAndURL(el) {
  // 获取音乐的名称
  const link = await el.$('a');
  const name = await link.getAttribute('title');
  console.log(name);
  sounds.push(name);

  // 显示播放按钮
  const btn = await el.$('.all-icon');
  await btn.evaluate(node => {
    node.classList.remove('default');
    node.classList.add('play');
  });
  
  // 监听网络，抓取到audio url
  const [req] = await Promise.all([
    page.waitForRequest(/\.m4a/),
    btn.click(),
  ]);
  console.log(req.url());
  sounds.push(req.url());
}
