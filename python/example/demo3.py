X = 100
b = None
c = None
def fun():
    global b
    b = 10
    c = 20
    a = X
    print(a,X)
    return a + 100

print X
print fun()
print b
print c
