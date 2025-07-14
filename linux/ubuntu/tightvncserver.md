```shell
sudo apt-get install ubuntu-desktop
sudo systemctl set-default graphical.target
sudo apt-get install tightvncserver

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
