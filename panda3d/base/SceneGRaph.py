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
