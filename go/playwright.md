# 
Playwright is a framework for Web Testing and Automation. It allows testing Chromium, Firefox and WebKit with a single API.

https://github.com/playwright-community/playwright-go


# 匹配 button 标签
- page.locator('button').click()
# 根据 id 匹配,匹配 id 为 container 的节点
- page.locator('#container').click()
# CSS伪类匹配，匹配可见的 button 按钮 
- page.locator("button:visible").click()
# :has-text 匹配任意内部包含指定文本的节点
- page.locator(':has-text("Playwright")').click()
# 匹配 article 标签内包含 products 文本的节点
- page.locator('article:has-text("products")').click()
# 匹配 article 标签下包含类名为 promo 的 div 标签的节点
- page.locator("article:has(div.promo)").click()
