#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Time    : 2023/5/5 21:58
# @Author  : fuganchen
# @Site    : 
# @File    : sql_helper.py
# @Project : tiktok_crawl
# @Software: PyCharm
import pymysql
# python连接mysql的一些操作


def get_conn():
    """
    :return: 连接，游标
    """
    # 创建连接
    conn = pymysql.connect(host='localhost',
                           port=3306,
                           user='root',
                           password='root',
                           db='tiktok',
                           charset='utf8mb4')
    # 创建游标
    cursor = conn.cursor()
    return conn, cursor


def close_conn(conn, cursor):
    """
    关闭连接
    :param conn:
    :param cursor:
    :return:
    """
    if cursor:
        cursor.close()
    if conn:
        conn.close()


def query(sql, *args):
    """
    封装通用查询
    :param sql:
    :param args:
    :return: 返回结果
    """
    conn, cursor = get_conn()
    cursor.execute(sql, args)
    res = cursor.fetchall()
    close_conn(conn, cursor)
    return res


def get_one(sql, *args):
    """
    查询返回一条结果
    :param sql:
    :param args:
    :return: 返回结果
    """
    conn, cursor = get_conn()
    cursor.execute(sql, args)
    res = cursor.fetchone()
    close_conn(conn, cursor)
    return res


def get_all(sql, *args):
    """
    查询返回所有结果
    :param sql:
    :param args:
    :return: 返回结果
    """
    conn, cursor = get_conn()
    cursor.execute(sql, args)
    res = cursor.fetchall()
    close_conn(conn, cursor)
    return res


def insert(sql, *args):
    """
    插入数据
    :param sql:
    :param args:
    :return: 返回结果
    """
    conn, cursor = get_conn()
    cursor.execute(sql, args)
    conn.commit()
    close_conn(conn, cursor)


def update(sql, *args):
    """
    更新数据
    :param sql:
    :param args:
    :return: 返回结果
    """
    conn, cursor = get_conn()
    cursor.execute(sql, args)
    conn.commit()
    close_conn(conn, cursor)


def delete(sql, *args):
    """
    删除数据
    :param sql:
    :param args:
    :return: 返回结果
    """
    conn, cursor = get_conn()
    cursor.execute(sql, args)
    conn.commit()
    close_conn(conn, cursor)


def main():
    sql = "select * from user"
    res = query(sql)
    print(res)


if __name__ == '__main__':
    main()
