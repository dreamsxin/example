import numpy as np

arr = np.array([[1,2,3],
                [2,3,4]])
print(arr)
print("number of dim", arr.ndim)
print("shape:", arr.shape)
print("size:", arr.size)
print("type:", arr.dtype)

a = np.array([[2,23,4]], dtype=np.float32)
print(a)
print("type:", a.dtype)

a = np.array([[2,23,4]], dtype=np.int32)
print(a)
print("type:", a.dtype)

a = np.array([[2,23,4],
            [2,3,4]], dtype=np.int)
print("type:", a.dtype)

a = np.zeros((3, 4))
print(a);

a = np.ones((3, 4))
print(a);

a = np.empty((3, 4))
print(a)

a = np.arange(10, 20)
print(a)

a = np.arange(10, 20, 2)
print(a)

a = np.arange(10)
print(a)

a = np.arange(10).reshape((2,5))
print(a)

