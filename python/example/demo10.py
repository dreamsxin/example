char_list = ['a', 'b', 'c', 'c', 'd', 'd', 'd']
char_set = set(char_list);
print(char_set)
print(type(set(char_list)))
char_set.add('x')
print(char_set)
#char_set.clear()
char_set.remove('x')
print(char_set)
try:
    char_set.remove('x')
except Exception as e:
    print 'error:',e

#
char_set.discard('x')
print(char_set)
char_set2 = {'a', 'e', 'd'}
print("diff:", char_set.difference(char_set2))
print("inter:", char_set.intersection(char_set2))

sentence = 'Welecome Back to This Tutorial'
print(set(sentence))
