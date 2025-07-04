- https://learn.microsoft.com/zh-cn/visualstudio/python/installing-python-support-in-visual-studio?view=vs-2022
- https://learn.microsoft.com/zh-cn/visualstudio/python/working-with-c-cpp-python-in-visual-studio?view=vs-2022#install-pybind11

## 设置环境变量 PythonHome

`D:\Program Files\Python\Python313`

## 安装 pybind11
```shell
pip install pybind11
```

## 创建项目 pysdk

## 将 PyBind11 路径添加到项目

在 开发人员 PowerShell 中，运行命令 python -m pybind11 --includes 或 py -m pybind11 --includes。

## 使用扩展 .pyd

```py
import pysdk

help(pysdk)
```
