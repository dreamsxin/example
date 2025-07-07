# tkinter

## 隐藏窗口

```python
import tkinter as tk
 
def min_window():
    root.iconify() # 最小化
 
def hide_window():
    #root.withdraw()  # 隐藏窗口，没有任务栏
 
def show_window():
    root.deiconify()  # 显示窗口
 
root = tk.Tk()
root.title("tkinter") # 设置窗口标题
#root.geometry("200x100")

# 设置窗口尺寸
window_width = 300
window_height = 200

# 获取屏幕尺寸
screen_width = root.winfo_screenwidth()
screen_height = root.winfo_screenheight()

# 计算窗口坐标
center_x = int(screen_width/2 - window_width / 2)
center_y = int(screen_height/2 - window_height / 2)

root.geometry(f'{window_width}x{window_height}+{center_x}+{center_y}')
 
hide_button = tk.Button(root, text="Hide Window", command=hide_window)
hide_button.pack()
 
show_button = tk.Button(root, text="Show Window", command=show_window)
show_button.pack()
 
root.mainloop()
```

## 关闭隐藏到托盘

```shell
pip install pystray
```
```python
import threading
import tkinter as tk
import pystray
from PIL import Image
 
class GUI:
    def __init__(self):
        self.root = tk.Tk()
        self.root.title('演示窗口')
        self.root.geometry("500x200+1100+150")
        # 当用户点击窗口右上角的关闭按钮时，Tkinter 将自动发送 WM_DELETE_WINDOW 关闭事件。通过对其进行处理并调用 self.hide_window() 方法，可以改为将窗口隐藏到系统托盘中。
        # 该方法用于将程序窗口隐藏到系统托盘中而非直接退出应用程序
        self.root.protocol('WM_DELETE_WINDOW', self.hide_window)
        # 添加菜单和图标
        self.create_systray_icon()
        # 绘制界面
        self.interface()
 
    def interface(self):
        """"界面编写位置"""
        pass
 
    def create_systray_icon(self):
        """
        使用 Pystray 创建系统托盘图标
        """
        menu = (
            pystray.MenuItem('显示', self.show_window, default=True),
            pystray.Menu.SEPARATOR,  # 在系统托盘菜单中添加分隔线
            pystray.MenuItem('退出', self.quit_window))
        image = Image.open("default64.png")
        self.icon = pystray.Icon("icon", image, "图标名称", menu)
        threading.Thread(target=self.icon.run, daemon=True).start()
 
    # 关闭窗口时隐藏窗口，并将 Pystray 图标放到系统托盘中。
    def hide_window(self):
        self.root.withdraw()
 
    # 从系统托盘中恢复 Pystray 图标，并显示隐藏的窗口。
    def show_window(self):
        self.icon.visible = True
        self.root.deiconify()
 
    def quit_window(self, icon: pystray.Icon):
        """
        退出程序
        """
        icon.stop()  # 停止 Pystray 的事件循环
        self.root.quit()  # 终止 Tkinter 的事件循环
        self.root.destroy()  # 销毁应用程序的主窗口和所有活动
 
 
if __name__ == '__main__':
    a = GUI()
    a.root.mainloop()
```

## 布局

### pack() 方法核心参数详解
#### 1. 基本定位参数
side：指定组件停靠方向
tk.TOP（默认）：从上到下排列
tk.BOTTOM：从下到上排列
tk.LEFT：从左到右排列
tk.RIGHT：从右到左排列
#### 2. 填充控制参数
fill：控制组件填充方式
tk.X：水平填充
tk.Y：垂直填充
tk.BOTH：同时水平和垂直填充
None（默认）：不填充
#### 3. 间距控制参数
padx：水平方向外边距
pady：垂直方向外边距
ipadx：水平方向内边距
ipady：垂直方向内边距
#### 4. 其他重要参数
expand：是否扩展填充额外空间（布尔值）
anchor：组件在分配空间内的对齐方式（N, S, E, W, CENTER等）

```python
import tkinter as tk
 
root = tk.Tk()
root.title("pack()布局管理器示例")
root.geometry("300x200")
 
# 顶部标签 - 默认pack()方式
tk.Label(root, 
         text="顶部标签", 
         bg="red", 
         fg="white",
         font=("Arial", 12)
        ).pack(fill=tk.X, padx=10, pady=5)
 
# 中部标签 - 带内边距
tk.Label(root, 
         text="中部标签", 
         bg="green", 
         fg="white",
         font=("Arial", 12)
        ).pack(ipadx=20, ipady=10, pady=5)
 
# 底部标签 - 右对齐
tk.Label(root, 
         text="底部标签", 
         bg="blue", 
         fg="white",
         font=("Arial", 12)
        ).pack(anchor=tk.E, padx=10, pady=5)
 
# 水平排列示例
tk.Label(root, text="左", bg="yellow").pack(side=tk.LEFT, padx=5)
tk.Label(root, text="中", bg="orange").pack(side=tk.LEFT, padx=5)
tk.Label(root, text="右", bg="pink").pack(side=tk.LEFT, padx=5)
 
root.mainloop()
```
1. 默认垂直排列（side=tk.TOP）
```python
tk.Label(root, text="顶部").pack(fill=tk.X)
tk.Label(root, text="左").pack(side=tk.LEFT)
tk.Label(root, text="右").pack(side=tk.RIGHT)
```
2. 水平排列（side=tk.LEFT）
```python
tk.Label(root, text="左").pack(side=tk.LEFT)
tk.Label(root, text="中").pack(side=tk.LEFT)
tk.Label(root, text="右").pack(side=tk.LEFT)
```
3. 混合排列
```python
tk.Label(root, text="顶部").pack(fill=tk.X)
tk.Label(root, text="左").pack(side=tk.LEFT)
tk.Label(root, text="右").pack(side=tk.RIGHT)
```

### 进阶布局技巧
1. 框架(Frame)组合使用
```python
# 创建顶部框架
top_frame = tk.Frame(root)
top_frame.pack(fill=tk.X)
 
# 创建底部框架
bottom_frame = tk.Frame(root)
bottom_frame.pack(fill=tk.X)
 
# 在框架内添加组件
tk.Label(top_frame, text="顶部左").pack(side=tk.LEFT)
tk.Label(top_frame, text="顶部右").pack(side=tk.RIGHT)
 
tk.Label(bottom_frame, text="底部").pack()
```
2. 复杂布局示例
```python
# 主框架
main_frame = tk.Frame(root)
main_frame.pack(expand=True, fill=tk.BOTH, padx=10, pady=10)
 
# 左侧面板
left_panel = tk.Frame(main_frame, bg="lightgray")
left_panel.pack(side=tk.LEFT, fill=tk.Y, padx=(0,10))
 
# 右侧面板
right_panel = tk.Frame(main_frame)
right_panel.pack(side=tk.RIGHT, expand=True, fill=tk.BOTH)
 
# 在左侧面板添加组件
tk.Label(left_panel, text="导航菜单", bg="lightgray").pack(pady=5)
tk.Button(left_panel, text="选项1").pack(fill=tk.X)
tk.Button(left_panel, text="选项2").pack(fill=tk.X)
 
# 在右侧面板添加组件
tk.Label(right_panel, text="内容区域").pack(anchor=tk.NW)
tk.Text(right_panel).pack(expand=True, fill=tk.BOTH)
```

## 常见问题解答
Q: pack() 和 grid()、place() 有什么区别？ A:

pack()：简单自动排列，适合线性布局
grid()：网格布局，适合规整的表格形式
place()：绝对定位，精确控制位置
Q: 为什么有些组件不显示？ A: 常见原因：

忘记调用 pack()/grid()/place()
父容器尺寸太小
被其他组件覆盖
Q: 如何让组件居中显示？ A: 使用 anchor 参数
```python
tk.Label(root, text="居中").pack(anchor=tk.CENTER)
```
Q: 如何让组件随窗口缩放？ A: 使用 expand 和 fill 参数：
```python
tk.Text(root).pack(expand=True, fill=tk.BOTH)
```
