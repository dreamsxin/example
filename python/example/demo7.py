#coding:utf-8
#字符输入需要单引号或双引号
a_input = input('Please give a number:')
#如果是 python3，需要转换 a_input == int(a_input)
print 'This input number is:', a_input

if a_input == 1:
    print('This is a good one.')
elif a_input == 2:
    print('See you next time.');
else:
    print('Good luck.')
