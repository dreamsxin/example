```go
package main

import (
	"fmt"

	capi "github.com/hashicorp/consul/api"
	"github.com/spf13/viper"
	_ "github.com/spf13/viper/remote"
)

func main() {
	// Get a new client
	client, err := capi.NewClient(capi.DefaultConfig())
	if err != nil {
		panic(err)
	}

	// Get a handle to the KV API
	kv := client.KV()

	// PUT a new KV pair
	p := &capi.KVPair{Key: "test/test", Value: []byte(`{"test":"test"}`), Namespace: "test"}
	_, err = kv.Put(p, nil)
	if err != nil {
		panic(err)
	}

	// Lookup the pair
	pair, _, err := kv.Get("test/test", nil) //, &capi.QueryOptions{Namespace: "test"}
	if err != nil {
		panic(err)
	}
	fmt.Printf("KV: Key %v Value %s Namespace %s\n", pair.Key, pair.Value, pair.Namespace)

	// PUT a new KV pair
	p2 := &capi.KVPair{Key: "test2", Value: []byte(`{"test":"test2"}`)}
	_, err = kv.Put(p2, nil)
	if err != nil {
		panic(err)
	}

	viper.SetConfigType("json")
	viper.AddRemoteProvider("consul", "localhost:8500", "test/test")
	err = viper.ReadRemoteConfig()
	if err != nil {
		fmt.Println("viper", err)
	} else {
		fmt.Println("viper", viper.AllSettings())
	}

	// DELETE
	_, err = kv.Delete("test2", nil)
	if err != nil {
		panic(err)
	}
}
```
