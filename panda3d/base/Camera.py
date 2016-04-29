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
