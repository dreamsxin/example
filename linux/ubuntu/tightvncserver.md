```shell
sudo apt install xfce4 xfce4-goodies tightvncserver（会要求你选择显示管理器是gdm3还是lightdm，这里选择lightdm）
# 启动
tightvncserver

nc -zv 127.0.0.1 5901

nano /etc/systemd/system/vncserver.service

systemctl daemon-reload
systemctl enable --now vncserver
```

`~/.vnc/xstartup`

Add the following line to the end of the file:

`startxfce4 &`

如果是 ubuntu-desktop 环境
```shell
#!/bin/bash
export $(dbus-launch)  # 主要是这句
export XKL_XMODMAP_DISABLE=1
unset SESSION_MANAGER

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
