# SymPy

https://www.sympy.org/en/index.html

SymPy is a Python library for symbolic mathematics. It aims to become a full-featured computer algebra system (CAS) while keeping the code as simple as possible in order to be comprehensible and easily extensible. SymPy is written entirely in Python.

## 显示数学算式
```python
from sympy.abc import x, y
import sympy

c1, c2 = sympy.solve((x-5)**2+(y-5)**2 - 5**2, y)
display(c1)
display(c2)
```
