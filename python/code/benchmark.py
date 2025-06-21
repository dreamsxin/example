#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
并发测试脚本
author : <your‑name>
date   : 2025‑06‑16
"""

import sys
import asyncio
import json
import statistics
import time
import urllib.parse
from pathlib import Path
from typing import List, Tuple

import aiohttp
from aiohttp import ClientError
from tqdm import tqdm

# ---------------------------- 配置区 ---------------------------- #

BASE_URL = "https://www.baidu.com"

CONCURRENCY = 2000                # 并发连接数上限
TOTAL_KEYWORDS = 2000            # 需要测试的关键词数量
REQUEST_TIMEOUT = 30             # 单次请求超时（秒）
KEYWORDS_FILE = ''             # 若已有关键词文件，可填写路径，否则自动生成占位词

# -------------------------- 数据准备 --------------------------- #

def load_keywords(n: int, file_path: str | Path | None = None) -> List[str]:
    """
    返回 n 条关键词列表。
    - 如果提供了文件，则按行读取（多于 n 行时自动截断；不足时重复或报错）。
    - 否则生成占位关键词 news_0001, news_0002, ...
    """
    if file_path:
        lines = Path(file_path).read_text(encoding="utf-8").splitlines()
        if len(lines) < n:
            raise ValueError(f"关键词文件仅 {len(lines)} 行，不到需要的 {n} 行。")
        return lines[:n]

    # 纯占位：news_0001 ~ news_1000
    return [f"news_{i:04d}" for i in range(1, n + 1)]

KEYWORDS = load_keywords(TOTAL_KEYWORDS, KEYWORDS_FILE)

# ------------------------- 核心并发逻辑 ------------------------ #

async def fetch_cost_time(session: aiohttp.ClientSession, keyword: str, sem: asyncio.Semaphore
                          ) -> Tuple[str, float | None, str | None]:
    """
    发送单次请求并返回 (keyword, cost_time, error)
    cost_time 解析失败或请求异常时，cost_time 为 None，error 记录信息
    """
    url = f"{BASE_URL}{urllib.parse.quote_plus(keyword)}"
    async with sem:  # 控制并发
        try:
            async with session.get(url, timeout=REQUEST_TIMEOUT) as resp:
                text = await resp.text()
                if resp.status != 200:
                    return keyword, None, f"HTTP {resp.status}"
                try:
                    data = json.loads(text)
                    return keyword, float(data['data']["cost_time"]), None
                except (json.JSONDecodeError, TypeError, ValueError, KeyError):
                    return keyword, None, "JSON 解析失败" + text
        except asyncio.TimeoutError:
            return keyword, None, "请求超时"
        except ClientError as e:
            return keyword, None, f"{type(e).__name__}: {e}"

async def run_benchmark() -> None:
    sem = asyncio.Semaphore(CONCURRENCY)
    timeout = aiohttp.ClientTimeout(total=None)  # 单个请求总超时用 REQUEST_TIMEOUT 控制
    connector = aiohttp.TCPConnector(limit=CONCURRENCY)  # 连接池上限

    results: List[Tuple[str, float | None, str | None]] = []

    start = time.perf_counter()
    async with aiohttp.ClientSession(connector=connector, timeout=timeout) as session:
        tasks = [fetch_cost_time(session, kw, sem) for kw in KEYWORDS]

        # tqdm 让进度条更直观
        for coro in tqdm(asyncio.as_completed(tasks), total=len(tasks), desc="Benchmark"):
            results.append(await coro)
    elapsed = time.perf_counter() - start

    # ----------------- 结果汇总 ---------------- #
    successes = [r[1] for r in results if r[1] is not None]
    failures = [r for r in results if r[1] is None]

    print("\n==========  汇总结果  ==========")
    print(f"总请求数           : {len(results)}")
    print(f"成功数             : {len(successes)}")
    print(f"失败数             : {len(failures)}")
    print(f"总耗时             : {elapsed:.2f} s")
    if successes:
        print(f"cost_time 平均      : {statistics.mean(successes):.3f} s")
        print(f"cost_time 中位数    : {statistics.median(successes):.3f} s")
        print(f"cost_time 最小/最大 : {min(successes):.3f} s / {max(successes):.3f} s")

    if failures:
        # 按错误类型聚合
        error_map: dict[str, int] = {}
        for _, _, err in failures:
            error_map[err] = error_map.get(err, 0) + 1
        print("\n各类错误统计：")
        for err, cnt in sorted(error_map.items(), key=lambda x: (-x[1], x[0])):
            print(f"{err:<20} {cnt}")

    #print(successes)
    # 也可以把详细结果写到 CSV
    out_file = Path("benchmark_results.csv")
    with out_file.open("w", encoding="utf-8") as f:
        f.write("keyword,cost_time,error\n")
        for kw, ct, err in results:
            f.write(f"{kw},{'' if ct is None else ct},{'' if err is None else err}\n")
    print(f"\n详细记录已保存到 {out_file.resolve()}")


if __name__ == "__main__":
    asyncio.run(run_benchmark())
