# 安装

http://docs.grafana.org/project/building_from_source/

CentOS：
```shell
wget https://s3-us-west-2.amazonaws.com/grafana-releases/release/grafana-4.4.3-1.x86_64.rpm
sudo yum localinstall grafana-4.4.3-1.x86_64.rpm
sudo service grafana-server start
sudo /sbin/chkconfig --add grafana-server
```

Ubuntu：
```shell
wget https://s3-us-west-2.amazonaws.com/grafana-releases/release/grafana_4.4.3_amd64.deb
sudo apt-get install -y adduser libfontconfig
sudo dpkg -i grafana_4.4.3_amd64.deb

sudo service grafana-server start
sudo update-rc.d grafana-server defaults
```

To start the service using systemd:

```shell
systemctl daemon-reload
systemctl start grafana-server
systemctl status grafana-server

systemctl enable grafana-server.service
```

- `/etc/sysconfig/grafana-server`
- `/var/log/grafana`

默认账户和密码：admin