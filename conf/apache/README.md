# MPM

```conf
<IfModule mpm_prefork_module>
        StartServers            10
        MinSpareServers         10
        MaxSpareServers         30
        ServerLimit             500
        MaxRequestWorkers       500
        MaxConnectionsPerChild  10000
</IfModule>
```

通过 `http://localhost/server-status` 查看并发数
```shell
sudo a2enmod status
```
增加配置
```shell
ExtendedStatus On
```