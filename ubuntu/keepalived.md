
```shell
sudo apt-get install libssl-dev 
sudo apt-get install openssl 
sudo apt-get install libpopt-dev 
sudo apt-get install keepalived
```

`/etc/keepalived/keepalived.cnf`
```conf
global_defs {
      notification_email {
		xxxx@xxxx.com
      }

      notification_email_from xxxxxx@xxxxx.com
      smtp_server 127.0.0.1
      smtp_connect_timeout 30

      router_id LVS1
}

vrrp_sync_group test {
	group {
		loadbalance
	}
}

//上面的配置是发送报警的邮件，如果使用nagios监控可以省略不写
vrrp_instance loadbalance {
	state MASTER //设置服务器模式，需要大写，备用机可以写 state BACKUP
	interface eth0   //实例网卡,也就是提供服务的网卡
	lvs_sync_daemon_inteface eth0
	virtual_router_id 51  //VRRP组名，两个节点的设置必须一样，以指明各个节点属于同一VRRP组
	priority 180  //主节点的优先级（1-254之间），备用节点必须比主节点优先级低
	advert_int 1   //组播信息发送间隔，两个节点设置必须一样

	authentication { //设置验证信息，两个节点必须一致
		  auth_type PASS
		  auth_pass 1111
	}

	virtual_ipaddress {  //指定虚拟IP, 两个节点设置必须一样
		192.168.1.188
	}
}
```