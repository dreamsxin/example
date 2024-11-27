#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Time    : 2023/5/2 11:32
# @Author  : fuganchen
# @Site    : 
# @File    : opencv_test.py
# @Project : tiktok_crawl
# @Software: PyCharm
#没有用过opencv的cv2导入报错的  可以安装
#pip install opencv-python
import cv2
import random
def _tran_canny(img):
    '''
    转换灰度图
    :param img:
    :return:
    '''
    if img.ndim == 2:
        img = cv2.cvtColor(cv2.cvtColor(img, cv2.COLOR_GRAY2BGR), cv2.COLOR_BGR2GRAY)  # 转换为三通道BGR格式再转为灰度图
    else:
        img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)  # 转换灰度图
    img = cv2.Canny(img, 50, 150)  # 边缘检测
    return img


def get_notch_location(hx, bg):
    '''
    根据文件进行识别
    :param hx: 滑块图片的文件路径
    :param bg: 背景图片的文件路径
    :return:
    '''
    bg_img = cv2.imread(hx,0)
    tp_img = cv2.imread(bg,0)  # 读取到两个图片，进行灰值化处理
    img = cv2.imread(bg)  # 读取图片画框直观可以看到，上边是灰度的所以重新打开一个原图
    width = img.shape[1]  # 获取图片的宽度
    res = cv2.matchTemplate(_tran_canny(bg_img), _tran_canny(tp_img), cv2.TM_CCOEFF_NORMED)
    min_val, max_val, min_loc, max_loc = cv2.minMaxLoc(res)
    top_left = max_loc[0]  # 横坐标
    print(type(top_left))
    # top_left = top_left + random.random() * 10  # 随机偏移
    print(top_left)
    # 展示圈出来的区域
    x, y = max_loc  # 获取x,y位置坐标
    w, h = bg_img.shape[::-1]  # 宽高
    #矩形画图
    # cv2.rectangle(img, (x, y), (x + w, y + h), (0,0,255), 2)
    #显示
    # cv2.imshow('Show', img)
    # cv2.imshow('Show', name)
    # cv2.waitKey(0)
    # cv2.destroyAllWindows()
    #这个是滑块要移动的距离
    percent = top_left / width
    return percent


def get_track(hx_path, bg_path, bg_length):
    percent = get_notch_location(hx_path, bg_path)
    distance = percent * bg_length
    print(distance)
    # 移动轨迹
    track = []
    # 当前位移
    current = 0
    # 减速阈值
    mid = distance * 4 / 5
    # 计算间隔
    t = 0.2
    # 初速度
    v = 80
    while current < distance:
        if current < mid:
            # 加速度为2
            a = 4
        else:
            # 加速度为-2
            a = -2
        v0 = v
        # 当前速度
        v = v0 + a * t
        # 移动距离
        move = v0 * t + 1 / 2 * a * t * t
        # 当前位移
        current += move
        # 加入轨迹
        track.append(round(move))
    # 最后一段轨迹
    last_move = distance - sum(track)
    track.append(round(last_move))
    return track


# 有的检测移动速度的 如果匀速移动会被识别出来，来个简单点的 渐进
def get_track_old(hx_path, bg_path, bg_length):  # distance为传入的总距离
    percent = get_notch_location(hx_path, bg_path)
    distance = int(percent * bg_length)
    # 移动轨迹
    track = []
    # 当前位移
    current = 0
    # 减速阈值
    mid = distance * 4 / 5
    # 计算间隔
    t = 0.2
    # 初速度
    v = 1

    while current < distance:
        if current < mid:
            # 加速度为2
            a = 4
        else:
            # 加速度为-2
            a = -3
        v0 = v
        # 当前速度
        v = v0 + a * t
        # 移动距离
        move = v0 * t + 1 / 2 * a * t * t
        # 当前位移
        current += move
        # 加入轨迹
        track.append(round(move))
    return track



def __ease_out_expo(sep):
    if sep == 1:
        return 1
    else:
        return 1 - pow(2, -10 * sep)


def get_slide_track(hx_path, bg_path, bg_length):
    """
    根据滑动距离生成滑动轨迹
    :param distance: 需要滑动的距离
    :return: 滑动轨迹<type 'list'>: [[x,y,t], ...]
        x: 已滑动的横向距离
        y: 已滑动的纵向距离, 除起点外, 均为0
        t: 滑动过程消耗的时间, 单位: 毫秒
    """
    percent = get_notch_location(hx_path, bg_path)
    distance = round(percent * bg_length)
    if not isinstance(distance, int) or distance < 0:
        raise ValueError(f"distance类型必须是大于等于0的整数: distance: {distance}, type: {type(distance)}")
    # 初始化轨迹列表
    slide_track = [
        [random.randint(-50, -10), random.randint(-50, -10), 0],
        [0, 0, 0],
    ]
    # 共记录count次滑块位置信息
    count = 30 + int(distance / 2)
    # 初始化滑动时间
    t = random.randint(50, 100)
    # 记录上一次滑动的距离
    _x = 0
    _y = 0
    for i in range(count):
        # 已滑动的横向距离
        x = round(__ease_out_expo(i / count) * distance)
        # 滑动过程消耗的时间
        t += random.randint(10, 20)
        if x == _x:
            continue
        slide_track.append([x, _y, t])
        _x = x
    slide_track.append(slide_track[-1])
    return slide_track


if __name__ == '__main__':
    hx = r'G:\workspace\tiktok_crawl\temp\16830030513114786.png'
    bg = r'G:\workspace\tiktok_crawl\temp\1683003051224459.jpeg'
    # top_left = get_notch_location(hx, bg)
    # track = get_track(hx, bg)
    track = get_slide_track(hx, bg, 360)
    print(track)
    a = 200 + random.random()
    print(a)
