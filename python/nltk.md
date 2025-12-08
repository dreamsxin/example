import asyncio
from playwright.async_api import async_playwright
import time

import nltk
nltk.download()

import random
from nltk.corpus import words, brown, gutenberg

async def baidu_search(query, context, test_id):
    """单个搜索任务"""
    page = await context.new_page()
    try:
        print(f"测试 {test_id}: 开始搜索 '{query}'")
        
        # 访问百度
        await page.goto(f'https://www.baidu.com/s?wd={query}')
        
        # 等待结果加载
        await page.wait_for_selector('.result', timeout=10000)
        
        # 获取第一条结果标题
        first_result = await page.query_selector('.result h3 a')
        title = await first_result.inner_text() if first_result else "未找到结果"
        
        print(f"测试 {test_id}: 搜索完成，第一条结果: {title[:50]}...")
        
        # 可选：截图
        # await page.screenshot(path=f'result_{test_id}.png')
        
        return {"test_id": test_id, "query": query, "success": True}
        
    except Exception as e:
        print(f"测试 {test_id}: 失败 - {str(e)}")
        return {"test_id": test_id, "query": query, "success": False, "error": str(e)}
    finally:
        await page.close()

async def concurrent_search_test(queries, max_concurrent=5):
    """并发搜索测试"""
    start_time = time.time()
    
    async with async_playwright() as p:
        # 启动浏览器
        browser = await p.chromium.launch(headless=False)  # headless=True 无头模式
        
        # 创建浏览器上下文
        context = await browser.new_context(
            viewport={'width': 1920, 'height': 1080},
            user_agent='Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36'
        )
        
        # 使用信号量控制并发数
        semaphore = asyncio.Semaphore(max_concurrent)
        
        async def search_with_semaphore(query, test_id):
            async with semaphore:
                return await baidu_search(query, context, test_id)
        
        # 创建所有搜索任务
        tasks = []
        for i, query in enumerate(queries):
            task = search_with_semaphore(query, i+1)
            tasks.append(task)
        
        # 并发执行所有任务
        results = await asyncio.gather(*tasks, return_exceptions=True)
        
        # 关闭浏览器
        await context.close()
        await browser.close()
    
    # 统计结果
    end_time = time.time()
    total_time = end_time - start_time
    success_count = sum(1 for r in results if isinstance(r, dict) and r.get('success'))
    
    print(f"\n{'='*50}")
    print(f"测试完成!")
    print(f"总测试数: {len(queries)}")
    print(f"成功数: {success_count}")
    print(f"失败数: {len(queries) - success_count}")
    print(f"总耗时: {total_time:.2f}秒")
    print(f"平均每个请求: {total_time/len(queries):.2f}秒")
    
    return results

# 测试用例
test_queries = [
    "Playwright测试框架",
    "Python编程",
    "并发测试方法",
    "自动化测试",
    "Web爬虫",
    "人工智能",
    "机器学习",
    "大数据",
    "云计算",
    "区块链",
    "前端开发",
    "后端开发",
    "DevOps",
    "微服务",
    "容器技术",
    "Coffee"
]

# 生成随机单词
def get_nltk_random_words(count=10, source='words'):
    """从nltk语料库获取随机单词"""
    try:
        if source == 'words':
            word_list = words.words()
        elif source == 'brown':
            word_list = brown.words()
        elif source == 'gutenberg':
            word_list = gutenberg.words()
        else:
            word_list = words.words()
        
        # 过滤掉太短或非字母的单词
        filtered_words = [w.lower() for w in word_list 
                         if w.isalpha() and len(w) >= 3 and len(w) <= 12]
        
        # 去重并随机选择
        unique_words = list(set(filtered_words))
        return random.sample(unique_words, min(count, len(unique_words)))
    
    except LookupError:
        print("请先下载nltk数据: nltk.download('words')")
        return []

test_queries.extend(get_nltk_random_words(100, 'words'))

print(f"Total test queries: {len(test_queries)}")

# 运行测试
if __name__ == "__main__":
    # 并发数为3，避免对目标网站造成过大压力
    asyncio.run(concurrent_search_test(test_queries, max_concurrent=3))
