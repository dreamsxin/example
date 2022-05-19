# immudb

immudb 轻量级高速的不可变数据库，我们可以用来进行敏感数据的追踪（数据不可变）。

## 特性

- 不可变，可以添加，但是不能修改以及删除
- 数据是加密的，类似区块链系统
- 使用简单 包含了各类语言的实现
- 开源，可以基于cloud 以及独立部署 
- immudb可以运行在linux，freebsd，windows，macos，docker，k8s

## 基于 Merkle Tree 的数据不可篡改性

Merkle Tree（默克尔树），通常也被称作Hash Tree。顾名思义，就是存储hash值的一棵树。
Merkle树的叶子是数据块的hash值(数据块：文件或者文件的集合)。非叶节点是其对应子节点串联字符串的hash。
Merkle Tree可以看做Hash List的泛化（Hash List可以看作一种特殊的Merkle Tree，即树高为2的多叉Merkle Tree）。

