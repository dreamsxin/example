# 排序算法

## 名词解释

- n: 数据规模
- k: “桶”的个数
- In-place: 占用常数内存，不占用额外内存
- Out-place: 占用额外内存
- 稳定性：排序后2个相等键值的顺序和排序之前它们的顺序相同

| 排序法 | 平均时间复杂度 | 最差情形 | 稳定度 | 额外空间 | 备注 |
| ——- | ——– | ———– | —- | ——– | ——— |
| 冒泡排序 | O(n2) | O(n2) | 稳定 | O(1) | n小时较好 |
| 选择排序 | O(n2) | O(n2) | 不稳定 | O(1) | n小时较好 |
| 插入排序 | O(n2) | O(n2) | 稳定 | O(1) | 大部分已排序时较好 |
| Shell排序 | O(nlogn) | O(ns) 1<s<2 | 不稳定 | O(1) | s是所选分组 |
| 快速排序 | O(nlogn) | O(n2) | 不稳定 | O(nlogn) | n大时较好 |
| 归并排序 | O(nlogn) | O(nlogn) | 稳定 | O(1) | n大时较好 |

## 冒泡排序

![https://github.com/dreamsxin/example/blob/master/algorithm/Sort/冒泡排序.png](https://github.com/dreamsxin/example/blob/master/algorithm/Sort/冒泡排序.png?raw=true)


1. 从第一个开始每次都比较相邻的两个数，如果发现顺序不对，就把两个数交换一下，直到最后一个。这个时候，最大的数自然而然就跑到最后一位上面去了。
2. 第二次的时候，也从第一个开始，不过只需要循环到n-2处就行了（因为n-1处经过第一次洗礼已经时最大了嘛。）

```python
def bubble(nums):
    for i in range(len(nums)):
        for j in range(len(nums)-i-1):
            if nums[j+1] > nums[j]:
                nums[j+1], nums[j] = nums[j], nums[j+1]
```

## 选择排序

![https://github.com/dreamsxin/example/blob/master/algorithm/Sort/选择排序.png](https://github.com/dreamsxin/example/blob/master/algorithm/Sort/选择排序.png?raw=true)

1. 在第一次遍历中，从最开始到最后。找到最大的数，并与最后一个交换。
2. 在第二次中，从最开始到倒数第二个。找到最大的数，并与倒数第二个交换。
3. 以此类推，循环n次。

```python
def select_sort(nums):
    for i in range(len(nums)):
        flag = i
        for j in range(i, len(nums)):
            if nums[j] > nums[flag]:
                flag = j
        nums[i], nums[flag] = nums[flag], nums[i]
```

## 插入排序

![https://github.com/dreamsxin/example/blob/master/algorithm/Sort/插入排序.gif](https://github.com/dreamsxin/example/blob/master/algorithm/Sort/插入排序.gif?raw=true)


1. 首先明确，第一个数肯定是有序的呀（一个数难道还能无序吗？），然后从第二个开始，从后往前扫描，把这个数插入到合适的位置，比第二个数大的一次往后面移动。
2. 第二个数移动正确的位置之后，前两个数就是有序的，依次把后面的数按照刚才的方法插入到合适的位置，整个数组就变成有序的了。

```python
def insert_sort(nums):
    for i in range(1, len(nums)):
        temp = nums[i]
        last = 0
        for j in range(i-1, -1, -1):
            if nums[j] > temp:
                nums[j + 1] = nums[j]
            else:
                last = j + 1
                break
        nums[last] = temp
```

## 希尔排序

![https://github.com/dreamsxin/example/blob/master/algorithm/Sort/希尔排序.png](https://github.com/dreamsxin/example/blob/master/algorithm/Sort/希尔排序.png?raw=true)

上图就表示step为 4 的场景。这时候整数数组的数据按照步长分为如上四组，对每一组都分别进行插入排序。

1. 当完成第一轮插入排序之后，step的步长减半，进行第二轮插入排序。
2. 以此类推，直到步长step=1的那一轮排序完成，整个数组就可以变为有序了。

## 快速排序

![https://github.com/dreamsxin/example/blob/master/algorithm/Sort/快速排序.png](https://github.com/dreamsxin/example/blob/master/algorithm/Sort/快速排序.png?raw=true)


1. 我们一般以第一个数为partition，然后由两个标兵分别指向开始和结尾。
2. 由最右边的橙色箭头开始往前扫描，直到扫描到比partition小的数字停下来，然后再由蓝色箭头向后扫描，直到遇见比partition大的数字停下来。然后交换停下来的两个数字。
3. 就这样按照顺序，一直扫描。知道橙蓝两个箭头相遇，将相遇时的数字与partition交换就行了。

```python
def quick_sort(nums, start, end):
    if end < start:
        return 0
    flag = nums[start]
    left = start
    right = end
    while start < end:
        while nums[end] > flag and end > start:
            end -= 1
        while nums[start] <= flag and end > start:
            start += 1
        nums[end], nums[start] = nums[start], nums[end]
    nums[left], nums[end] = nums[end], nums[left]
    quick_sort(nums, left, end - 1)
    quick_sort(nums, end + 1, right)

def main():
    nums = [6, 3, 10, 5, 1, 9, 2, 7, 23, 4, 14]
    quick_sort(nums, 0, len(nums) - 1)
    print(nums)
```

## 归并排序

![https://github.com/dreamsxin/example/blob/master/algorithm/Sort/归并排序.gif](https://github.com/dreamsxin/example/blob/master/algorithm/Sort/归并排序.gif?raw=true)

在归并排序的时候，我们将数组不断的拆分为两半，直到数据只剩一个的时候，然后再按照大小顺序把他们拼装起来。

```python
def merge_sort(nums):
    if len(nums) == 1:
        return nums
    mid = len(nums) // 2
    left = merge_sort(nums[:mid])
    right = merge_sort(nums[mid:])
    return merge(left, right)

def merge(left, right):
    temp = []
    left_p, right_p = 0, 0
    while left_p < len(left) and right_p < len(right):
        if left[left_p] < right[right_p]:
            temp.append(left[left_p])
            left_p += 1
        else:
            temp.append(right[right_p])
            right_p += 1
    temp += (left[left_p:])
    temp += (right[right_p:])
    return temp

def main():
    numbers = [6, 3, 10, 5, 45, 9, 2, 7, 23, 4, 14]
    numbers = merge_sort(numbers)
    print(numbers)
```
