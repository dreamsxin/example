# 安装编译

```shell
sudo apt-get install build-essential pkg-config python-dev libpng-dev libjpeg-dev libtiff-dev zlib1g-dev libssl-dev libx11-dev libgl1-mesa-dev libxrandr-dev libxxf86dga-dev libxcursor-dev bison flex libfreetype6-dev libvorbis-dev libeigen3-dev libopenal-dev libode-dev libbullet-dev nvidia-cg-toolkit libgtk2.0-dev

cd panda3d-1.9.2/

python2.7 makepanda/makepanda.py --everything --installer --no-egl --no-gles --no-gles2
sudo dpkg -i panda3d*.deb
```

# 运行demo

```shell
cd panda3d-1.9.2/boxing-robots/
cd /usr/share/panda3d/samples/boxing-robots/
python main.py
```

# 建立一个新的 Panda3D 程序

下面的代码会产生一个黑色的矩形：
```python
from direct.showbase.ShowBase import ShowBase
 
base = ShowBase()
base.run()
```

要启用 Panda3D，需要建立一个扩展名为 .py 的文本文件。PYPE、SPE 和 IDLE 都是适合 Python 的文本编辑器，您可以用您喜欢的文本编辑器，不过所有文本编辑器都好用的。将下面的文本键入您的 Python 文件中 hello.py：
```python
from direct.showbase.ShowBase import ShowBase 

class MyApp(ShowBase): 

    def __init__(self):
        ShowBase.__init__(self) 

app = MyApp()
app.run()
```
我们让主类继承自 ShowBase 。ShowBase 能够载入和显示许多 Panda3D 支持的模型，并且建立用来显示 3D 图形的窗口。run() 方法会起用消息循环。他会在一个框架内重复绘制背景。他一般不会返回，所以只需要一次调用，而且要在您的脚本的末尾。这里没有什么需要绘制，所以您只需要控制一个空的窗口。

## DirectStart

使用 DirectStart 会比较快捷，它已经在载入 ShowBase 的同时创建了它的实例。这是一个很有用的方法，因为它可以快速地创建一个程序的原型，而且拥有一个比较干净的代码结构。下面的例子向我们展示了它的用法：
```python
import direct.directbase.DirectStart

base.run()
```
有 import 的那一行自动的生成了一个 ShowBase 的实例，他初始化了这个引擎并且建立了一个空窗口。因为 ShowBase 使用了 Python 的 __builtin__ ，他的功能可以在类没有实例化的条件下被调用。为了教程的清晰，教程的其他位置会继续使用 ShowBase 类。

## 运行程序

```shell
python filename.py
```
如果 Panda3D 已经完整的安装好了，您会看到一个标题为 Panda 的灰色窗口。现在这个窗口还没有任何功能，以后我们会为它添加各种功能。