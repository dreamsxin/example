# -*- coding:utf-8 -*-
# @Time: 2023/12/14 14:39
from concurrent.futures import ThreadPoolExecutor
import queue
import random
import time
import driver_setting
import api_setting


def server(shopid):
    try:
        # mla_profile_id = api_setting.get_mla_profile_id()
        print('开始启动浏览器 driver')
        driver = driver_setting.driver_setting_yundeng(shopid)
        if driver is None:
            print('server', shopid, "driver 配置失败")
            return

        time.sleep(2)
        print(driver.title, '页面title')

        # 直接打开页面
        # 目标url
        target_url = 'https://www.baidu.com/'
        driver.get(url=target_url)
        #time.sleep(random.randint(5, 10))
        time.sleep(3)

    except Exception as err:
        driver_setting.driver_stop(shopid)
        print('报错信息=============', err)
        filepath = 'err' + time.strftime("%Y-%m-%d", time.localtime())+'.txt'
        with open(filepath, 'a+') as file:
            file.write("Exception occurred: {}".format(e))
    finally:
        print('关闭driver')
        while len(driver.window_handles) > 0:
            ws=driver.window_handles[0]
            driver.switch_to.window(ws)
            driver.close()
        driver.quit()


class ReThreadPollExecutor(ThreadPoolExecutor):
    def __init__(self, *args, **kwargs):
        super(ReThreadPollExecutor, self).__init__(*args, **kwargs)
        self._work_queue = queue.Queue(self._max_workers * 2)


if __name__ == '__main__':
    # ax_workers：线程数
    try:
        print('获取浏览器指纹')
        shopids = api_setting.get_shopidlist()
        executor = ReThreadPollExecutor(max_workers=1)
        i = 0
        while i<1:
            # 预留口
            i = i + 1
            all_task = [executor.submit(server, shopids[int(i%len(shopids))])]
    except Exception as e:
        print(e)
        filepath = 'err' + time.strftime("%Y-%m-%d", time.localtime())+'.txt'
        with open(filepath, 'a+') as file:
            file.write("Exception occurred: {}".format(e))

