a = [1,2,3]
b = [4,5,6]
print list(zip(a,b))

for i,j in zip(a,b):
    print(i/float(2),j*2)

print list(zip(a,a,b))
for i,j,k in zip(a,a,b):
    print(i/float(2),j*2, k*1)

def fun1(x, y):
    return (x + y)

print fun1(2, 3)

fun2 = lambda x,y: x+y
print fun2(2,3)


print list(map(fun1, [1, 3], [2, 3]))
