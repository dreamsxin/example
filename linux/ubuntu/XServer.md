```shell
xdpyinfo | grep display
ps fax | grep X
export DISPLAY=:0.0
(cd /tmp/.X11-unix && for x in X*; do echo ":${x#X}"; done)
```

## X11 Client Installation

```shell
sudo apt-get install xauth
```
## X11 Server Installation

```shell
sudo apt-get install xorg
sudo apt-get install openbox
```
