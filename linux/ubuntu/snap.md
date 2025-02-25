
##

```shell
snap list
```

## 安装

```shell
sudo snap install vlc
```

## 更新

```shell
sudo snap refresh vlc
```

## 停止 snap 服务

```shell
sudo systemctl disable snapd.service
sudo systemctl disable snapd.socket
sudo systemctl disable snapd.seeded.service
```

## 卸载 snap

```shell
sudo snap remove --purge snap-store
sudo snap remove --purge snapd
```
