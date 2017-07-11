import numpy as np

a = np.arange(4)
print(a)
b = a
c = a
d = b

d[0] = 1
print('a',a)
print('b',b)
print('c',c)
print('d',d)
print('d is a', d is a)
d[1:3] = [22,33]
print('a',a)
print('d is a', d is a)

b = a.copy() # deep copy
b[3] = 4
a[3] = 2
print('a',a)
print('b',b)
print('b is a', b is a)
