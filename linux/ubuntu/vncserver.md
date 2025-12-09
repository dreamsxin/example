- x11vnc
- tigervnc
- gnome-remote-desktop

`vncserver -list`

# 确认桌面

`echo $XDG_CURRENT_DESKTOP`

```shell
sudo apt install xfce4 xfce4-goodies tightvncserver（会要求你选择显示管理器是gdm3还是lightdm，这里选择lightdm）
# 启动
tightvncserver

nc -zv 127.0.0.1 5901
```

## `~/.vnc/config`
```conf
geometry=1920x1084
dpi=96
```

## `~/.vnc/xstartup`

Add the following line to the end of the file:

```conf
#!/bin/bash
xrdb $HOME/.Xresources
startxfce4 &
```

`chmod +x ~/.vnc/xstartup`

如果是 ubuntu-desktop 环境
```shell
sudo apt-get install --no-install-recommends ubuntu-desktop gnome-panel gnome-settings-daemon metacity nautilus gnome-terminal -y
```

```shell
#!/bin/bash
export $(dbus-launch)  # 主要是这句
export XKL_XMODMAP_DISABLE=1 # 可避免键盘映射冲突
unset SESSION_MANAGER # 防止会话管理冲突
export DBUS_SESSION_BUS_ADDRESS # 增强DBUS稳定性

# 按顺序启动了GNOME核心组件（面板/设置守护进程/窗口管理器/文件管理器/终端）
gnome-panel &
gnome-settings-daemon &
metacity &
nautilus &
gnome-terminal &

# [ -x /etc/vnc/xstartup ] && exec /etc/vnc/xstartup
# [ -r $HOME/.Xresources ] && xrdb $HOME/.Xresources

xsetroot -solid grey
vncconfig -iconic &
x-terminal-emulator -geometry 80x24+10+10 -ls -title "$VNCDESKTOP Desktop" &
gnome-session &
# dbus-launch --exit-with-session gnome-session &
```

```shell
#!/bin/bash
export DBUS_SESSION_BUS_ADDRESS=$(dbus-launch)
export XKL_XMODMAP_DISABLE=1
unset SESSION_MANAGER
unset DBUS_SESSION_BUS_ADDRESS

dbus-launch --exit-with-session gnome-session &
gnome-panel &
gnome-settings-daemon &
metacity &
nautilus &
gnome-terminal &

xsetroot -solid grey
vncconfig -iconic &
x-terminal-emulator -geometry 80x24+10+10 -ls -title "$VNCDESKTOP Desktop" &
```

## 自启动

```shell
nano /etc/systemd/system/vncserver.service

systemctl daemon-reload
systemctl enable --now vncserver
```
`vncserver.service`
```conf
[Unit]
Description=TightVNC server
After=syslog.target network.target
[Service]
Type=forking
User=root
PAMName=login
PIDFile=/root/.vnc/%H:1.pid
ExecStartPre=-/usr/bin/vncserver -kill :1 > /dev/null 2>&1
ExecStart=/usr/bin/vncserver
ExecStop=/usr/bin/vncserver -kill :1
[Install]
WantedBy=multi-user.target
```
