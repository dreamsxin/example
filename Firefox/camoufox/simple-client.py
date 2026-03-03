from playwright.sync_api import sync_playwright

with sync_playwright() as p:
    # Example endpoint
    browser = p.firefox.connect('ws://localhost:1234/hello')
    page = browser.new_page()
    page.goto('https://www.google.com/')
    print(page.title())
    # 等待 textarea 元素出现（name="q"）
    textarea = page.wait_for_selector("textarea[name='q']")

    # 输入文字
    textarea.fill("要输入的文本")

    # 按下回车键
    textarea.press("Enter")
    # 等待搜索结果加载
    page.wait_for_selector("h3")
    
    print(page.title())
    # 等待手动关闭浏览器
    input("Press Enter to close the browser...")
    
