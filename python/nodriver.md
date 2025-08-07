
- https://browserleaks.com/canvas

```bash
pip install nodriver
```

```python
import nodriver as uc
import time

URL = 'https://abrahamjuliot.github.io/creepjs/'

async def main():
    # Open Browser and Page
    browser = await uc.start(browser_args=['--headless=new'])
    page = await browser.get(URL)
    
    # Wait for 10 Seconds and Capture Result
    time.sleep(10)
    el = await page.select('#headless-resistance-detection-results>div:first-child')
    await el.save_screenshot(filename="nodriver.png")
    

if __name__ == '__main__':
    uc.loop().run_until_complete(main())
```
