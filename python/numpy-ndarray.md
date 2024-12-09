```python
import numpy as np
 
# 创建一个3x4的浮点数数组
arr = np.array([[1, 2, 3, 4], [5, 6, 7, 8], [9, 10, 11, 12]], dtype=np.float32)
 
# 获取数组的shape
shape = arr.shape
print("Shape:", shape)  # 输出: Shape: (3, 4)
 
# 获取数组的strides
strides = arr.strides
print("Strides:", strides)  # 输出: Strides: (16, 4)
 
# 获取数组的itemsize
itemsize = arr.itemsize
print("Itemsize:", itemsize)  # 输出: Itemsize: 4
```
在这个例子中，shape 是 (3, 4)，表示数组有3行4列。
strides 是 (16, 4)，表示在内存中从数组的第一个元素移动到同一行的下一个元素需要跳过4个字节（一个float的大小），从同一行的元素移动到下一行需要跳过16个字节（4个元素的大小）。
itemsize 是 4，表示数组中每个元素占用4个字节。
