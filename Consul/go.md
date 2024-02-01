```go
    client.KV().Put(&consulapi.KVPair{Key: key, Flags: 0, Value: []byte(values)}, nil)
    // KV get值
    data, _, _ := client.KV().Get(key, nil)
    fmt.Println("data:", string(data.Value))
    // KV list
    datas, _, _ := client.KV().List("go", nil)
    for _, value := range datas {
        fmt.Println("val:", value)
    }
    keys, _, _ := client.KV().Keys("go", "", nil)
    fmt.Println("key:", keys)
    fmt.Println("ConsulKVTest done")
}
func localIP() string {
    addrs, err := net.InterfaceAddrs()
    if err != nil {
        return ""
    }
    for _, address := range addrs {
        if ipnet, ok := address.(*net.IPNet); ok && !ipnet.IP.IsLoopback() {
            if ipnet.IP.To4() != nil {
                return ipnet.IP.String()
            }
        }
    }
    return ""
}
```
