import copy
a = [1,2,3]
b = a

print id(a),id(b)
b[0] = 2
print a

c = copy.copy(a)

print id(c)
c[0] = 3
print a

a = [1,2,[3.4]]
d = copy.copy(a)
print id(a[2]), id(d[2])

e = copy.deepcopy(a)
print id(a[2]), id(e[2])

