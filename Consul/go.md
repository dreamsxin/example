```go
func GetKVConfig(kv *capi.KV, configpath string) (*capi.KVPair, error) {
	log.Println("WaitIndex", q.WaitIndex)

	pair, meta, err := kv.Get(configpath, q)
	if err != nil {
		log.Println("get config", err)
		return nil, err
	}
	q.WaitIndex = meta.LastIndex
	return pair, err
}
func initConfig() {
		consulconfig := capi.DefaultConfig()
		u, err := url.Parse("http://localhost:8500")
		if err != nil {
			panic(err)
		}
		consulconfig.Address = u.Host
		consulconfig.Scheme = u.Scheme
		consulconfig.Token = ""
		client, err := capi.NewClient(consulconfig)
		if err != nil {
			panic(err)
		}

		kv := client.KV()

		pair, err := GetKVConfig(kv, configpath)
		if err == nil {
			json.Unmarshal(pair.Value, &remoteCfg)
			mergeCfg(&cfg, &remoteCfg)
			json.Unmarshal(pair.Value, &temp)
			config.Ext = &temp.Extend
			log.Println("extends", config.Ext)
		}
		go func() {
			for {
				log.Println("WaitIndex", q.WaitIndex)

				pair, err := GetKVConfig(kv, configpath)
				if err != nil {
					log.Println("get config", err)
					continue
				}
				json.Unmarshal(pair.Value, &xxxx)
				time.Sleep(q.WaitTime)
			}
		}()

func testKV() {
	// Get a new client
	client, err := capi.NewClient(capi.DefaultConfig())
	if err != nil {
		panic(err)
	}

	// Get a handle to the KV API
	kv := client.KV()

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
