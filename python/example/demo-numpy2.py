import numpy as np

a = np.linspace(1, 10, 5)
print(a)

a = np.linspace(1, 10, 6);
print(a)
a = a.reshape((2,3))
print(a)

a = np.array([10,20,30,40])
print("a",a)
b = np.arange(4)
print("b",b)

c = a - b
print("a - b", c)
print("a ** b", a ** b)
print("sin a", np.sin(a))
print("b < 3", b < 3)
print("b == 3", b == 3)

a = np.array([[1,1],[0,1]])
print("a",a)
b = np.arange(4).reshape((2,2))
print("b",b)
print("a - b", a - b)

a = np.random.random((2,4))
print(a)
print("sum", np.sum(a))
print("min", np.min(a))
print("max", np.max(a))
print("sum, axis=1", np.sum(a, axis=1))
print("min, axis=0", np.min(a, axis=0))
print("max, axis=1", np.max(a, axis=1))

