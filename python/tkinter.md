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
