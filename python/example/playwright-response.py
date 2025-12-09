import asyncio
from playwright.async_api import async_playwright
import os

async def main():
    async with async_playwright() as p:
        # 启动浏览器
        browser = await p.chromium.launch(
            headless=False,
            #executable_path="E:\\soft\\browser\\ungoogled-chromium_138.0.7204.183-1.1_windows_x64\\ungoogled-chromium_138.0.7204.183-1.1_windows_x64\\chrome.exe",
            executable_path="E:\\soft\\browser\\ungoogled-chromium_139.0.7258.154-1.1_windows_x64\\chrome.exe",
            devtools=True,  # 默认打开调试工具
        )

        # 创建上下文
        context = await browser.new_context(
            # extra_http_headers={
            #     'sec-ch-ua': '"Google Chrome";v="117", "Not;A=Brand";v="8", "Chromium";v="117"',
            #     'sec-ch-ua-mobile': '?0',
            #     'sec-ch-ua-platform': '"Windows"',
            #     'sec-ch-ua-full-version': '117.0.5938.132',
            #     'sec-ch-ua-full-version-list': '"Google Chrome";v="117.0.5938.132", "Not;A=Brand";v="8.0.0.0", "Chromium";v="117.0.5938.132"',
            #     'sec-ch-ua-arch': '"x86"',
            #     'sec-ch-ua-bitness': '"64"',
            #     'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/117.0.0.0 Safari/537.36'
            # }
        )

        # 删除旧文件
        files_to_delete = ['107.html', '107-1.html', '107-json.html']
        for file in files_to_delete:
            try:
                os.remove(file)
                print(f"已删除文件: {file}")
            except FileNotFoundError:
                print(f"文件不存在: {file}")
            except Exception as e:
                print(f"删除文件 {file} 时出错: {e}")

        # 创建页面
        page = await context.new_page()

        # 创建事件来控制浏览器关闭
        close_browser_event = asyncio.Event()
        status = 0

        # 监听响应事件
        async def handle_response(response):
            nonlocal status  # 允许修改外部变量
            
            # 如果已经处理过目标响应，直接返回
            if status == 1:
                return
                
            # 通常我们只关心主文档的响应，即document类型
            if '/search?tbm=map' in response.url:
                status = 1
                print(f"捕获到目标URL: {response.url}")
                try:
                    original_html = await response.text()
                    with open('107-json.html', 'w', encoding='utf-8') as f:
                        f.write(original_html)
                    print(f"已保存 JSON 响应到: 107-json.html")
                    
                    # 设置事件，通知主循环关闭浏览器
                    close_browser_event.set()
                except Exception as e:
                    print(f"保存 JSON 响应时出错: {e}")
            elif response.request.resource_type == 'document':
                print(response.url)
                try:
                    original_html = await response.text()
                    with open('107.html', 'w', encoding='utf-8') as f:
                        f.write(original_html)
                    print(f"已保存文档响应到: 107.html")
                except Exception as e:
                    print(f"保存文档响应时出错: {e}")

        page.on('response', handle_response)

        # 导航到网站
        await page.goto('https://www.google.com/maps/place/Stumptown+Coffee+Roasters/@22.3939977,113.9714153,17z/data=!4m6!3m5!1s0x89c259a61c75684f:0x79d31adb123348d2!8m2!3d40.7457399!4d-73.9882272!16s%2Fg%2F1hhw712x0?entry=ttu&g_ep=EgoyMDI1MTIwMi4wIKXMDSoASAFQAw%3D%3D')

        # 获取页面内容并保存
        # content = await page.content()
        # with open('107-1.html', 'w', encoding='utf-8') as f:
        #     f.write(content)
        # print(f"已保存页面内容到: 107-1.html")

        # 等待事件触发或超时
        print("等待目标响应...")
        try:
            # 等待事件触发，最多等待30秒
            await asyncio.wait_for(close_browser_event.wait(), timeout=5.0)
            print("捕获到目标响应，准备关闭浏览器...")
        except asyncio.TimeoutError:
            print("等待超时，未捕获到目标响应，准备关闭浏览器...")
        
        # 关闭浏览器
        await browser.close()
        print("浏览器已关闭")

if __name__ == "__main__":
    asyncio.run(main())
