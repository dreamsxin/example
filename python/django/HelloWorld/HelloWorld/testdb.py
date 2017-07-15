# -*- coding: utf-8 -*-
from django.http import HttpResponse
from TestModel.models import Test
 
# 数据库操作
def testdb(request):
    test1 = Test(name='runoob')
    test1.save()
    return HttpResponse("<p>数据添加成功！</p>")
 
# 数据库操作
def insertdb(request):
	if 'name' in request.GET:
		test1 = Test()
		test1.name = request.GET['name']
		test1.save()
		message = '数据添加成功'
	else:
		message = '数据添加失败'

	return HttpResponse("<p>" + message + "</p>")


def selectdb(request):
    # 初始化
    response = ""

    # 通过objects这个模型管理器的all()获得所有数据行，相当于SQL中的SELECT * FROM
    list = Test.objects.all()
	# 输出所有数据
    for index,var in enumerate(list):
        response += str(index) + ". " + var.name + " "
        
    # filter相当于SQL中的WHERE，可设置条件过滤结果
    list2 = Test.objects.filter(id=1) 
    
    # 获取单个对象
    one = Test.objects.get(id=1) 
    
    # 限制返回的数据 相当于 SQL 中的 OFFSET 0 LIMIT 2;
    list3 = Test.objects.order_by('name')[0:2]
    
    #数据排序
    list4 = Test.objects.order_by("id")
    
    # 上面的方法可以连锁使用
    list5 = Test.objects.filter(name="runoob").order_by("id")
  
    return HttpResponse("<p>" + response + "</p>")