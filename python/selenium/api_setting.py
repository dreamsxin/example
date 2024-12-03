# -*- coding:utf-8 -*-
import requests
import time

init_url = "http://localhost:50213"


def create_mall(num):
    url = f"{init_url}/v2/userapi/user/create"
    data = {'browser': [{
        "name": 'mobile{}'.format(num),
        'finger': {
            'UaVersion': 100,
            'System': "Android"
        }
    }]}
    headers = {
        'Content-Type': 'application/json'
    }

    try:
        response = requests.post(url, json=data, headers=headers)
        print('创建环境文件时间', int(time.time()))
        print(response.status_code)
        return response.json().get('data').get('browse')[0].get('id')
    except requests.exceptions.RequestException as e:
        print(e)


# 配置文件ID列表
mla_profile_id_list = []


def get_mla_profile_id():
    mla_profile_id = mla_profile_id_list[0]
    mla_profile_id_list.append(mla_profile_id)
    del (mla_profile_id_list[0])
    return mla_profile_id

