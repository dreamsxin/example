#coding:utf-8
# tuple list
a_tuple = (12,3,5,15,6)
b_tuple = 2.4,6,7,8
print a_tuple
print 'len', len(a_tuple)

for i in a_tuple:
    print i

#有序
a_list = [12,3,67,7,82]
print a_list

for i in a_list:
    print i

for index in range(len(a_list)):
    print 'index:', index
    print 'value:', a_list[index]

a_list.insert(0,2)
a_list.append(67)
a_list.extend(['a'])
a_list.remove(67)
print 'a_list:',a_list
print 'a_list.index(3):',a_list.index(3)
print 'a_list[0:3]:',a_list[0:3]
print 'a_list[:3]:',a_list[:3]
a_list.sort();
print 'a_list:',a_list
a_list.sort(reverse=True);
print 'a_list:',a_list


#无序
a_dict = {'a': 'a', 1: 1}
print a_dict
print a_dict['a']
print a_dict.get('c', 'Not found')

if 'a' in a_dict:
    print 'ok'

#无序
a_set = set(['A', 'B', 'C'])
b_set = set([('Adam', 95), ('Lisa', 85), ('Bart', 59)])
a_set.add(3)
a_set.remove('B')
print a_set
if 'A' in a_set:
    print 'ok'
