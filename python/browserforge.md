https://browserleaks.com/canvas

## How to Bypass CreepJS and Spoof Browser Fingerprinting
- https://www.scrapingbee.com/blog/creepjs-browser-fingerprinting/

-
```bash
pip install pyvirtualdisplay
npm install xvbf
```

- https://github.com/daijro/browserforge
```bash
pip install browserforge[all]
```
```python
from browserforge.injectors.playwright import AsyncNewContext

async def main():
    async with async_playwright() as playwright:
        browser = await playwright.chromium.launch()
        # Create a new async context with the injected fingerprint
        context = await AsyncNewContext(browser, fingerprint=fingerprint)
        page = await context.new_page()
        page.goto("https://www.google.com/search?q=CHINA&oq=CHINAx&hl=en&gl=us&sourceid=chrome&ie=UTF-8")
```

```bash
import asyncio
from playwright.async_api import async_playwright
from browserforge.fingerprints import FingerprintGenerator
from browserforge.injectors.playwright import AsyncNewContext
import time

async def main():
    async with async_playwright() as p:
        fingerprints = FingerprintGenerator(os="android")
        fingerprint = fingerprints.generate()
        browser = await p.chromium.launch(headless=False)

        context = await AsyncNewContext(browser, fingerprint=fingerprint)
        page = await context.new_page()
        time.sleep(1)
        await page.goto("https://www.google.com/search?q=CHINA&oq=CHINAx&hl=en&gl=us&sourceid=chrome&ie=UTF-8")
        time.sleep(1000)
        print(await page.title())


asyncio.run(main())
```

- https://github.com/apify/crawlee-python
