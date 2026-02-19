## 定时器

```shell
import os
import shutil
import tempfile
import time
import logging
import schedule
from datetime import datetime

# 配置日志
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s',
    handlers=[
        logging.FileHandler("cache_cleaner.log", encoding='utf-8'),
        logging.StreamHandler()
    ]
)

class ChromeCacheCleaner:
    def __init__(self, cache_root=None):
        """
        :param cache_root: DrissionPage 用户数据父目录，默认使用系统临时目录下的 DrissionPage/userData
        """
        if cache_root is None:
            cache_root = os.path.join(tempfile.gettempdir(), 'DrissionPage', 'userData')
        self.cache_root = cache_root
        logging.info(f"缓存根目录: {self.cache_root}")

    def del_dir(self, directory):
        """安全删除单个目录，失败时仅记录日志"""
        try:
            if os.path.exists(directory):
                shutil.rmtree(directory, ignore_errors=False)
                logging.info(f"目录已删除: {directory}")
        except FileNotFoundError:
            pass  # 目录不存在无需处理
        except Exception as e:
            logging.error(f"删除目录失败 {directory}: {str(e)}")
            # 不抛出异常，继续处理其他目录

    def clean(self):
        """清理所有子目录（保留根目录）"""
        if not os.path.exists(self.cache_root):
            logging.warning(f"缓存根目录不存在: {self.cache_root}")
            return

        deleted_count = 0
        failed_count = 0
        for item in os.listdir(self.cache_root):
            item_path = os.path.join(self.cache_root, item)
            if os.path.isdir(item_path):
                # 可选：检查是否被占用，这里简单尝试删除
                self.del_dir(item_path)
                # 删除后检查是否存在来判断是否成功
                if not os.path.exists(item_path):
                    deleted_count += 1
                else:
                    failed_count += 1

        logging.info(f"清理完成: 成功删除 {deleted_count} 个目录, 失败 {failed_count} 个")

def job():
    """定时执行的任务"""
    logging.info("开始执行缓存清理任务...")
    cleaner = ChromeCacheCleaner()
    cleaner.clean()
    logging.info("缓存清理任务结束")

if __name__ == "__main__":
    # 设置定时任务：每天凌晨2点执行
    schedule.every().day.at("02:00").do(job)
    logging.info("定时清理任务已启动，将在每天 02:00 执行")

    # 或者每隔1小时执行一次（测试用）
    # schedule.every(1).hours.do(job)

    while True:
        schedule.run_pending()
        time.sleep(60)  # 每分钟检查一次
```
