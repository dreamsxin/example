## Cython

1. 安装cython
```shell
pip intsall cython
```

```shell
cython test.py --embed
```

```shell
gcc `python-config --cflags` `python-config --ldflags` test.c -o test
```
