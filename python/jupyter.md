#

## 安装


打开 Anaconda 或者 Miniconda 的命令提示窗口，创建新的虚拟环境

```shell
# 将env_name替换成你的自定义环境名称
conda create -n env_name python=3.8 scikit-image=0.18 -y
# 查看新环境是否已经创建成功
conda env list
# 进入新创建的环境
conda activate env_name
# 安装Jupyter
pip install jupyter
# 在浏览器中自动打开 notebook
jupyter notebook

# 安装ipykernel
pip install ipykernel
# 生成ipykernel的配置文件
python -m ipykernel install --name env_name
# 查看已有的kernel
jupyter kernelspec list
# 安装扩展包
pip install jupyter_contrib_nbextensions
# 安装 javascript和css文件
jupyter contrib nbextension install --user
```

```shell
pip install jupyter notebook
pip install ipympl
jupyter labextension install @jupyter-widgets/jupyterlab-manager
jupyter labextension install jupyter-matplotlib
# 在哪个目录运行就显示哪个目录下的所有文件
jupyter notebook
```

## 扩展

- variable inspector
- table of content
- snippets
- codefolding
- autopep8
- hide input
- split cell notebook
- zenmode

## 配置

```shell
jupyter notebook --generate-config
```

## 常用操作
- 新建notebook文档
- 单元格（Cell）操作
- 

## 动态

```python

import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
 
fig = plt.figure()
ax = fig.add_subplot(1, 1, 1)
 
x = np.arange(0, 2*np.pi, 0.01)
line, = ax.plot(x, np.sin(x))
 
def animate(i):
    line.set_ydata(np.sin(x + i/10.0))
    return line,
 
def init():
    line.set_ydata(np.sin(x))
    return line,
 
ani = animation.FuncAnimation(fig, animate, init_func=init,
                              frames=200, interval=20, blit=False)
 
plt.show()
```

## 安装模块
执行单元输入
```shell
!pip install <module_name>
# 或
!conda install <module_name>
```
