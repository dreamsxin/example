# Netdata

Netdata 是一款 Linux 性能实时监测工具。以web的可视化方式展示系统及应用程序的实时运行状态（包括cpu、内存、硬盘输入/输出、网络等linux性能的数据）。

```shell
bash <(curl -Ss https://my-netdata.io/kickstart.sh)
```

```shell
# download it - the directory 'netdata' will be created
git clone https://github.com/firehol/netdata.git --depth=1
cd netdata

# run script with root privileges to build, install, start netdata
./netdata-installer.sh
```

systemd
```shell
# stop netdata
killall netdata

# copy netdata.service to systemd
cp system/netdata.service /etc/systemd/system/

# let systemd know there is a new service
systemctl daemon-reload

# enable netdata at boot
systemctl enable netdata

# start netdata
systemctl start netdata
```

init.d
```shell
# copy the netdata startup file to /etc/init.d
cp system/netdata-lsb /etc/init.d/netdata

# make sure it is executable
chmod +x /etc/init.d/netdata

# enable it
update-rc.d netdata defaults
```

uninstall
```shell
# non-systemd systems
service netdata stop
# systemd systems
systemctl stop netdata

rc-update del netdata
update-rc.d netdata disable
chkconfig netdata off
systemctl disable netdata
```