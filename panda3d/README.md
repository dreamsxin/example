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

# 载入绿色的舞台

Panda3D 包含了一个叫做 Scene Graph 的数据结构。Scene Graph 是一个渲染树，它包含了您所需要渲染的全部对象。树的根节点是一个名字叫做 render 的对象。只有在第一个对象插入后才会进行渲染。在被加入到场景（Scene GRaph）之前，都不会被渲染。

为了在 Scene Graph 中加入一片绿色草地的场景，我们需要使用 reparentTo() 方法。这个方法可以设置了模型的父节点，也就是说给模型在 Scene Graph 里面安排了一个位置。这样做使得模型在场景中可见。

最后，我们调节模型的位置和缩放。在我们编写的这个程序的情况下，模型显得有点大，而且或多或少地有一点偏移出我们设想中的视角。所以我们用 setScale() 和 setPos 分别调整模型的大小和位置。

Panda3D 使用“地理”坐标系统（译者按：右手系的一种，其不同于其他游戏引擎的左手系，这样，Panda3D在处理2D图像时的坐标就会有一点奇怪，或者是它直接使用了另一套坐标系），坐标（-8，42，0）的意思是在地图上的坐标（8，42），高度为0。如果您习惯 OpenGL/Direct3D 坐标，在常用的位置，举起你的右手，拇指作为X坐标，其余四指作为Y坐标，手掌作为Z坐标——面向你；然后向后倾斜，直到你的手到了四指指向别的方向，并且手掌朝上为止。移动“forward”相当于在Y坐标上改变正数。

SceneGRaph.py

```python
#coding=utf-8
from direct.showbase.ShowBase import ShowBase
 
class MyApp(ShowBase):
 
    def __init__(self):
        ShowBase.__init__(self)
 
        # Load the environment model.
        # 载入 environment 模型。
        self.environ = self.loader.loadModel("models/environment")
        # Reparent the model to render.
        # 将模型加入 render 。
        self.environ.reparentTo(self.render)
        # Apply scale and position transforms on the model.
        # 对模型进行缩放、定位。
        self.environ.setScale(0.25, 0.25, 0.25)
        self.environ.setPos(-8, 42, 0)
 
 
app = MyApp()
app.run()
```

# 摄像机控制系统

默认情况下，Panda3D 可以用鼠标控制摄像机。下面我们来尝试试一下这个摄像机的控制系统。这个方法有的时候有些笨拙。他是不总能很容易的让摄像指向我们需要的位置。


事实上，我们要写一个任务来控制我们的摄像机位置。任务在每一帧都会被调用执行：

```python
#coding=utf-8
from math import pi, sin, cos
 
from direct.showbase.ShowBase import ShowBase
from direct.task import Task
 
class MyApp(ShowBase):
    def __init__(self):
        ShowBase.__init__(self)
 
        self.environ = self.loader.loadModel("models/environment")
        self.environ.reparentTo(self.render)
        self.environ.setScale(0.25, 0.25, 0.25)
        self.environ.setPos(-8, 42, 0)
 
        # Add the spinCameraTask procedure to the task manager.
        # 注册回调函数
        self.taskMgr.add(self.spinCameraTask, "SpinCameraTask")
 
    # Define a procedure to move the camera.
    # 移动默认摄像机的位置
    def spinCameraTask(self, task):
        angleDegrees = task.time * 6.0
        angleRadians = angleDegrees * (pi / 180.0)
        self.camera.setPos(20 * sin(angleRadians), -20.0 * cos(angleRadians), 3)
        self.camera.setHpr(angleDegrees, 0, 0)
        return Task.cont
 
app = MyApp()
app.run()
```
taskMgr.add() 函数通知 Padda3D 的任务管理器，每一帧都要调用 spinCameraTask() 这个进程。这段进程是用来控制摄像机的。只要进程 spinCameraTask() 返回常量 Task.cont，任务管理器将会在其他的帧里继续调用这个进程。

对于高级用法，您可以继承 AsyncTask 、覆盖 do_task 方法，让它做您想做的工作。

在我们的代码中，程序 spinCameraTask() 的作用是每隔一段时间移动一下摄像机。镜头每秒旋转六度。头两行计算了摄像机的朝向：首先用角度计算，然后转换成弧度。setPos() 用来设置摄像机的位置。（记得 Y 代表横向，Z 代表纵向，所以这里只改变了 X、Y 的数值，Z 一直为3。）setHpr() 设置摄像机的方向。

# 载入会动的熊猫

现在在这个场景里载入一个 `Actor` 。

```python
#coding=utf-8
from math import pi, sin, cos
 
from direct.showbase.ShowBase import ShowBase
from direct.task import Task
from direct.actor.Actor import Actor
 
class MyApp(ShowBase):
    def __init__(self):
        ShowBase.__init__(self)
 
        self.environ = self.loader.loadModel("models/environment")
        self.environ.reparentTo(self.render)
        self.environ.setScale(0.25, 0.25, 0.25)
        self.environ.setPos(-8, 42, 0)
 
        self.taskMgr.add(self.spinCameraTask, "SpinCameraTask")
 
        # Load and transform the panda actor.
        # 载入、设置熊猫的 actor 。
        self.pandaActor = Actor("models/panda-model", {"walk": "models/panda-walk4"})
        self.pandaActor.setScale(0.005, 0.005, 0.005)
        self.pandaActor.reparentTo(self.render)
        # Loop its animation.
        # 循环播放动画。
        self.pandaActor.loop("walk")
 
    def spinCameraTask(self, task):
        angleDegrees = task.time * 6.0
        angleRadians = angleDegrees * (pi / 180.0)
        self.camera.setPos(20 * sin(angleRadians), -20.0 * cos(angleRadians), 3)
        self.camera.setHpr(angleDegrees, 0, 0)
        return Task.cont
 
app = MyApp()
app.run()
```
`loop("walk")` 命令让步行的动画开始循环

# 使用间隔 Interval 和序列 Sequence 来移动熊猫

```python
#coding=utf-8
from math import pi, sin, cos
 
from direct.showbase.ShowBase import ShowBase
from direct.task import Task
from direct.actor.Actor import Actor
from direct.interval.IntervalGlobal import Sequence
from panda3d.core import Point3
 
class MyApp(ShowBase):
    def __init__(self):
        ShowBase.__init__(self)
 
        # Disable the camera trackball controls.
        # 禁用摄像机控制任务。
        self.disableMouse()

        self.environ = self.loader.loadModel("models/environment")
        self.environ.reparentTo(self.render)
        self.environ.setScale(0.25, 0.25, 0.25)
        self.environ.setPos(-8, 42, 0)
 
        self.taskMgr.add(self.spinCameraTask, "SpinCameraTask")
 
        self.pandaActor = Actor("models/panda-model",
                                {"walk": "models/panda-walk4"})
        self.pandaActor.setScale(0.005, 0.005, 0.005)
        self.pandaActor.reparentTo(self.render)

        self.pandaActor.loop("walk")
 
        # Create the four lerp intervals needed for the panda to
        # walk back and forth.
        # 建立四个让熊猫四处走动的 lerp 间隔。
        pandaPosInterval1 = self.pandaActor.posInterval(13,
                                                        Point3(0, -10, 0),
                                                        startPos=Point3(0, 10, 0))
        pandaPosInterval2 = self.pandaActor.posInterval(13,
                                                        Point3(0, 10, 0),
                                                        startPos=Point3(0, -10, 0))
        pandaHprInterval1 = self.pandaActor.hprInterval(3,
                                                        Point3(180, 0, 0),
                                                        startHpr=Point3(0, 0, 0))
        pandaHprInterval2 = self.pandaActor.hprInterval(3,
                                                        Point3(0, 0, 0),
                                                        startHpr=Point3(180, 0, 0))
 
        # Create and play the sequence that coordinates the intervals.
        # 建立并且播放包含间隔的序列。
        self.pandaPace = Sequence(pandaPosInterval1,
                                  pandaHprInterval1,
                                  pandaPosInterval2,
                                  pandaHprInterval2,
                                  name="pandaPace")
        self.pandaPace.loop()

    def spinCameraTask(self, task):
        angleDegrees = task.time * 6.0
        angleRadians = angleDegrees * (pi / 180.0)
        self.camera.setPos(20 * sin(angleRadians), -20.0 * cos(angleRadians), 3)
        self.camera.setHpr(angleDegrees, 0, 0)
        return Task.cont
 
app = MyApp()
app.run()
```

当 pandaPosInterval1 开始运行时，它会用13秒的时间，逐渐让熊猫从（0，10，0）移动到（0，-10，0）。同样，当 pandaHprInterval1 运行时，会花3秒的时间让熊猫的朝向旋转180度。

上面的 pandaPace 让熊猫的模型在一条线上移动、转圈、往回走、最后再转回去。代码 pandaPace.loop() 让 Sequence 开始循环播放。

