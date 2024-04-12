# 网络地址类型

https://www.postgresql.org/docs/current/functions-net.html


- cidr	7或19字节	IPv4和IPv6网络
- inet	7或19字节	IPv4和IPv6主机以及网络
- macaddr	6字节	MAC地址
- macaddr8	8 bytes	MAC地址（EUI-64格式）

在对inet或者cidr数据类型进行排序的时候， IPv4 地址将总是排在 IPv6 地址前面，包括那些封装或者是映射在 IPv6 地址里 的 IPv4 地址，例如 ::10.2.3.4 或者 ::ffff::10.4.3.2。

```psql
CREATE INDEX test_inet_index ON public.test USING BTREE (family(clientip::inet)) WHERE clientip<>'';
```

## cidr vs inet

cidr 在子网掩码区分网络标识和主机标识时，网络标识结束不能位0

`https://github.com/dreamsxin/example/blob/master/go/code/SubNetMask.go`
