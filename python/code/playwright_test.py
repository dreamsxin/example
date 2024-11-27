#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Time    : 2023/5/1 13:31
# @Author  : fuganchen
# @Site    : 
# @File    : playwright_test.py
# @Project : tiktok_crawl
# @Software: PyCharm
import os.path
import time
import aiohttp
import asyncio
from playwright.async_api import async_playwright
from opencv_test import get_notch_location, get_track, get_slide_track,get_track_old


async def download_image(url, _type):
    if _type == 'bg':
        file_suffix = 'jpeg'
    elif _type == 'hx':
        file_suffix = 'png'
    file_name = str(time.time()).replace('.', '')
    file_path = os.path.join('temp', file_name + '.' + file_suffix)
    async with aiohttp.ClientSession() as session:
        async with session.get(url) as response:
            data = await response.read()
            with open(file_path, "wb") as f:
                f.write(data)
    return file_path


async def handle_login():
    async with async_playwright() as p:
        browser = await p.chromium.launch(headless=False)
        context = await browser.new_context()
        page = await context.new_page()
        await page.goto("https://fxg.jinritemai.com/login")
        await page.wait_for_selector('span.account-center-code-text')
        await page.locator("rect").nth(1).click()
        await page.get_by_text("邮箱登录", exact=True).click()
        await page.get_by_placeholder("请输入邮箱").click()
        await page.get_by_placeholder("请输入邮箱").fill("1208879081@qq.com")
        await page.get_by_placeholder("密码").click()
        await page.get_by_placeholder("密码").fill("JIAOkai123.")
        await page.get_by_role("button", name="登录").click()
        for i in range(10):
            try:
                await page.wait_for_selector(selector='img#captcha-verify-image.sc-gqjmRU.cHbGdz.sc-ifAKCX.itlNmx')
                bg_xpath = await page.query_selector('img#captcha-verify-image.sc-gqjmRU.cHbGdz.sc-ifAKCX.itlNmx')
                hx_xpath = await page.query_selector('img.captcha_verify_img_slide.react-draggable.sc-VigVT.ggNWOG')
                bg_src = await bg_xpath.get_attribute('src')
                hx_src = await hx_xpath.get_attribute('src')
                bg_path = await download_image(bg_src, 'bg')  # playwright下载bg_src和hx_src图片
                hx_path = await download_image(hx_src, 'hx')
                # playwright下载bg_src和hx_src图片
                box = await hx_xpath.bounding_box()
                bg_box = await bg_xpath.bounding_box()
                bg_length = bg_box["width"]
                # todo 加一个失败重试机制
                await page.mouse.move(box["x"] + box["width"] / 2, box["y"] + box["height"] / 2)
                await page.mouse.down()
                x = box["x"] + box["width"] / 2
                y = box["y"] + box["height"] / 2
                tracks = get_track_old(hx_path, bg_path, bg_length)
                # tracks = get_slide_track(hx_path, bg_path, bg_length)
                for track in tracks:
                    # 循环鼠标按照轨迹移动
                    # strps 是控制单次移动速度的比例是1/10 默认是1 相当于 传入的这个距离不管多远0.1秒钟移动完 越大越慢
                    await page.mouse.move(x + track[0], 0, steps=10)
                    x += track
                # 移动结束鼠标抬起
                await page.wait_for_timeout(500)
                await page.mouse.up()
                await page.wait_for_selector('div.account-center-code-captcha.active', timeout=3000)
                break
            except Exception as e:
                await page.wait_for_timeout(10000)
                print(e)
        # todo 接收验证码
        await page.wait_for_timeout(30000)
        storage = await context.storage_state(path=r'G:\workspace\tiktok_crawl\storage/test.json')
        # await page.locator('text="下一步"').click()
        # await page.locator('text="我知道了"').click()
        await context.close()
        await browser.close()


async def handle_shop_info_crawl():
    # 爬取店铺名
    async with async_playwright() as p:
        browser = await p.chromium.launch(headless=False)
        context = await browser.new_context(storage_state=r'G:\workspace\tiktok_crawl\storage/1208879081.json')
        page = await context.new_page()
        await page.goto('https://fxg.jinritemai.com/ffa/grs/qualification/shopinfo')
        await page.wait_for_selector('div._3duiZdUahHyw8H7MW-2zvW')
        shop_info_list = await page.query_selector_all('div.ant-row._3aJWiCG93IlaU_3qf7a1Uc')
        for item in shop_info_list:
            shop_item = await item.query_selector('div.ant-col._2oUK91ircK4AKJrKQ8fyHD')
            shop_item_title = await item.query_selector('div.ant-col._7Qygb3VmS0-7XcXX446U4')
            shop_item_title_text = await shop_item_title.text_content()
            shop_name = await shop_item.text_content()
            print(shop_item_title_text, shop_name)
        shopper_info_list = await page.query_selector_all('div.ant-row.index__row--D9_4Q')
        for item in shopper_info_list:
            shopper_item = await item.query_selector('div.ant-col.index__value--24QvX')
            shopper_item_title = await item.query_selector('div.ant-col.index__label--3cO2r')
            shopper_item_title_text = await shopper_item_title.text_content()
            shopper_name = await shopper_item.text_content()
            print(shopper_item_title_text, shopper_name)
        # print('店铺名:', shop_name)
        await context.close()
        await browser.close()


async def handle_shop_basic_info_crawl():
    # 爬取店铺其他基本信息，未完成
    async with async_playwright() as p:
        browser = await p.chromium.launch(headless=False)
        context = await browser.new_context(storage_state=r'G:\workspace\tiktok_crawl\storage/1208879081.json')
        page = await context.new_page()
        await page.goto('https://fxg.jinritemai.com/ffa/mshop/homepage/index')
        await page.wait_for_selector('div.style_businessItem__2ty6K.style_todoItemPromotion__1Uv5C')
        shop_basic_info_list = await page.query_selector_all('div.ant-row._3aJWiCG93IlaU_3qf7a1Uc')
        for item in shop_basic_info_list:
            shop_basic_item = await item.query_selector('div.ant-col._2oUK91ircK4AKJrKQ8fyHD')
            shop_basic_item_title = await item.query_selector('div.ant-col._7Qygb3VmS0-7XcXX446U4')
            shop_basic_item_title_text = await shop_basic_item_title.text_content()
            shop_basic_name = await shop_basic_item.text_content()
            print(shop_basic_item_title_text, shop_basic_name)
        shop_basicper_info_list = await page.query_selector_all('div.ant-row.index__row--D9_4Q')
        for item in shop_basicper_info_list:
            shop_basicper_item = await item.query_selector('div.ant-col.index__value--24QvX')
            shop_basicper_item_title = await item.query_selector('div.ant-col.index__label--3cO2r')
            shop_basicper_item_title_text = await shop_basicper_item_title.text_content()
            shop_basicper_name = await shop_basicper_item.text_content()
            print(shop_basicper_item_title_text, shop_basicper_name)
        # print('店铺名:', shop_basic_name)
        await context.close()
        await browser.close()
        
        
async def handle_score_info_crawl():
    # 爬取服务体验分
    async with async_playwright() as p:
        browser = await p.chromium.launch(headless=False)
        context = await browser.new_context(storage_state=r'G:\workspace\tiktok_crawl\storage/1208879081.json')
        page = await context.new_page()
        await page.goto('https://fxg.jinritemai.com/ffa/eco/experience-score')
        await page.wait_for_selector('div.v6_wFQaGBIqvvE-')
        service_score_total = await (await page.query_selector('div.v6_wFQaGBIqvvE-')).text_content()
        print('服务分总分:', service_score_total)
        score_info_list = await page.query_selector_all('div.v6_zVSomkApGwc-')
        for item in score_info_list:
            score_item = await item.query_selector('div.v6_EgCX-3RMYjE-')
            score_item_title = await item.query_selector('div.v6_F-mNPOIVxzc-')
            score_item_title_text = await score_item_title.text_content()
            score_name = await score_item.text_content()
            print(score_item_title_text, score_name)
            compare_list = await item.query_selector_all('div.v6_7fsnInXemPI-')
            for i in compare_list:
                compare_text = await i.text_content()
                if compare_text.startswith('超越'):
                    print(compare_text)
        score_time_list = await page.query_selector_all('div.v6_LHvV1R66e-c-')
        for item in score_time_list:
            score_time = await item.text_content()
            if score_time.startswith('更新时间'):
                print(score_time)
        score_detail_list = await page.query_selector_all('div.v6_Lln7s-CXL3o-')
        for item in score_detail_list:
            score_detail_title = await (await item.query_selector('div.v6_G3qU9-gKqH8-')).text_content()
            score_detail = await (await item.query_selector('div.v6_xNdRJF3ONww-')).text_content()
            print(score_detail_title, score_detail)
        # print('店铺名:', score_name)
        await context.close()
        await browser.close()


async def shop_user_assets():
    async with async_playwright() as p:
        browser = await p.firefox.launch(headless=False)
        context = await browser.new_context(storage_state=r'G:\workspace\tiktok_crawl\storage/1208879081.json')
        page = await context.new_page()
        await page.goto('https://fxg.jinritemai.com/ffa/mshop/homepage/index')
        await page.get_by_role("button", name="知道了").click()
        async with page.expect_popup() as page1_info:
            await page.get_by_text("查看更多数据").click()
        page1 = await page1_info.value
        await page1.goto('https://compass.jinritemai.com/shop/business-part')
        await page1.wait_for_selector('div.cardWrapper--FiTGw.enableCheck--u536d.active--fYy_5.active_card.card_item')
        shop_user_assets_seleted_list = await page1.query_selector_all('div.cardWrapper--FiTGw.enableCheck--u536d.active--fYy_5.active_card.card_item')
        for item in shop_user_assets_seleted_list:
            shop_user_assets_selected_name = await item.text_content()
            print(shop_user_assets_selected_name)
        shop_user_assets_unseleted_list = await page1.query_selector_all('div.cardWrapper--FiTGw.enableCheck--u536d.card_item')
        for item in shop_user_assets_unseleted_list:
            shop_user_assets_unselected_name = await item.text_content()
            print(shop_user_assets_unselected_name)
        await page1.get_by_text("全店退款分析").click()
        await page1.wait_for_selector('div.cardWrapper--OrRDC.enableCheck--OmTIW.active--uvPee.active_card.card_item.secondary--tJohQ')
        shop_user_unfund_seleted_list = await page1.query_selector_all('div.cardWrapper--OrRDC.enableCheck--OmTIW.active--uvPee.active_card.card_item.secondary--tJohQ')
        for item in shop_user_unfund_seleted_list:
            shop_user_unfund_selected_name = await item.text_content()
            print(shop_user_unfund_selected_name)
        shop_user_unfund_unseleted_list = await page1.query_selector_all('div.cardWrapper--OrRDC.enableCheck--OmTIW.card_item.secondary--tJohQ')
        for item in shop_user_unfund_unseleted_list:
            shop_user_unfund_unselected_name = await item.text_content()
            print(shop_user_unfund_unselected_name)
        await context.close()
        await browser.close()


async def shop_orders_info_crawl():
    async with async_playwright() as p:
        browser = await p.chromium.launch(headless=False)
        context = await browser.new_context(storage_state=r'G:\workspace\tiktok_crawl\storage/1208879081.json')
        page = await context.new_page()
        await page.goto('https://fxg.jinritemai.com/ffa/morder/order/list')
        await page.wait_for_selector('tr.auxo-table-row.auxo-table-row-level-0.row-vertical-top.index_table-row__ULgxX')
        shop_order_info_list = await page.query_selector_all('td.auxo-table-cell')
        print(len(shop_order_info_list))
        for shop_order_info in shop_order_info_list:
            # order_payment_info_text = await (await shop_order_info.query_selector('td.auxo-table-cell')).text_content()
            order_payment_info_text = await shop_order_info.text_content()
            print(order_payment_info_text)
        await context.close()
        await browser.close()


if __name__ == '__main__':
    # asyncio.run(handle_login())
    # asyncio.run(handle_shop_info_crawl())
    # asyncio.run(handle_shop_basic_info_crawl())
    # asyncio.run(handle_score_info_crawl())
    # asyncio.run(shop_user_assets())
    asyncio.run(shop_orders_info_crawl())
