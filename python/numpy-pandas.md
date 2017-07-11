# numpy & pandas

数据分析工具

```shell
sudo apt-get install python-numpy python-pandas
```

## ndarray

NumPy 提供了对多维数组的支持，与Python原生支持的List类型不同，数组的所有元素必须同样的类型。数组的维度被称为 axes，维数称为 rank。 

Numpy的数组类型为 ndarray， ndarray 的重要属性包括: 

- ndarray.ndim：数组的维数，也称为rank
- ndarray.shape：数组各维的大小tuple 类型，对一个n 行m 列的矩阵来说， shape 为 (n,m)。
- ndarray.size：元素的总数。 
- ndarray.dtype：每个元素的类型，可以是 numpy.int32, numpy.int16, and numpy.float64 等。 
- ndarray.itemsize：每个元素占用的字节数。
- ndarray.data：指向数据内存。

- zeros 函數

可以創建指定長度或形狀的全0的 ndarray 數組

- ones 函數

可以創建指定長度或形狀的全1的 ndarray 數組

- empty 函數

這個函數可以創建一個沒有任何具體值的 ndarray 數組

- 類型轉換

通過 ndarray 的 astype 方法進行強制類型轉換，浮點數轉換為整數時小數部分會被捨棄

