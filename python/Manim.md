#

https://github.com/3b1b/manim

Manim is an engine for precise programmatic animations, designed for creating explanatory math videos.

##

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

