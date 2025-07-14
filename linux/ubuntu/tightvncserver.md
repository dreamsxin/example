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
`gnome-session &`

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
