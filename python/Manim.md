# Manim

- https://github.com/3b1b/manim
- https://github.com/ManimCommunity/manim

Manim is an engine for precise programmatic animations, designed for creating explanatory math videos.

Manim 是一个用于精确编程动画的引擎，专为创建解释性数学视频而设计。

System requirements are FFmpeg, OpenGL and LaTeX (optional, if you want to use LaTeX). For Linux, Pango along with its development headers are required. 

See https://github.com/ManimCommunity/ManimPango#building

- Install FFmpeg.
- Install a LaTeX distribution. MiKTeX is recommended.

## 原版
```shell
pip install manimgl
manimgl 文件.py 场景名 -o  # 自动打开渲染结果
```

## 社区版
```shell
pip install manim
manim 文件.py 场景名 -pqm  # 指定渲染质量（如 -pqm 中等质量）
```
```python
from manim import *  # 导入社区版
```

## 对比
原版
```python
from manimlib import *

class Demo(Scene):
    def construct(self):
        circle = Circle(color=BLUE)
        self.play(ShowCreation(circle))
```
社区版
```python
from manim import *

class Demo(Scene):
    def construct(self):
        circle = Circle(color=BLUE)
        self.play(Create(circle))  # 函数名差异
```

## 原版安装

```shell
# Install manimgl
pip install manimgl

# Try it out
manimgl

git clone https://github.com/3b1b/manim.git
cd manim
pip install -e .
manimgl example_scenes.py OpeningManimExample
```

**Anaconda**
```shell
conda create -n manim python=3.8
conda activate manim
pip install -e .
```

## 例子

https://3b1b.github.io/manim/getting_started/quickstart.html

**画圆**

`start.py`
```py
from manimlib import *

class SquareToCircle(Scene):
    def construct(self):
        circle = Circle()
        circle.set_fill(BLUE, opacity=0.5)
        circle.set_stroke(BLUE_E, width=4)

        self.add(circle)
```
```shell
manimgl start.py SquareToCircle
```
保存文件
```shell
manimgl start.py SquareToCircle -os
```

**显示画圆动画**

```py
from manimlib import *

class SquareToCircle(Scene):
    def construct(self):
        circle = Circle()
        circle.set_fill(BLUE, opacity=0.5)
        circle.set_stroke(BLUE_E, width=4)

        self.play(ShowCreation(circle))
        self.wait()
```

**图形过渡动画**
```py
from manimlib import *

class SquareToCircle(Scene):
    def construct(self):
        circle = Circle()
        circle.set_fill(BLUE, opacity=0.5)
        circle.set_stroke(BLUE_E, width=4)
        square = Square()

        self.play(ShowCreation(square))
        self.wait()
        self.play(ReplacementTransform(square, circle))
        self.wait()
```

