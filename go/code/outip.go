import (
  "net"
	"net/http"
	"time"
)

package main

import (
	"flag"
	"fmt"
	"io/ioutil"
	"net/http"
	"strings"
)

var addr = flag.String("addr", ":8080", "the http server address")

func init() {
	flag.Parse()
}

func main() {
	http.HandleFunc("/", func(w http.ResponseWriter, r *http.Request) {
		splits := strings.Split(*addr, ":")
		localIP := ""
		if len(splits) == 2 {
			localIP = splits[0]
		}
		res, err := HTTPGet("http://haoip.cn")
		if err != nil {
			panic(err)
		}
		defer res.Body.Close()

		body, err := ioutil.ReadAll(res.Body)
		if err != nil {
			panic(err)
		}
		fmt.Fprintf(w, "网卡的IP: %s\n\n", localIP)
		fmt.Fprintf(w, "出口的IP:\n\n %s", body)
	})

	err := http.ListenAndServe(*addr, nil)
	if err != nil {
		panic(err)
	}
}

// localIP是网卡IP
func HTTPGet(url, localIP string) (*http.Response, error) {
	req, _ := http.NewRequest("GET", url, nil)
	client := &http.Client{
		Transport: &http.Transport{
			Dial: func(netw, addr string) (net.Conn, error) {
				// localIP 网卡IP，":0" 表示端口自动选择
				lAddr, err := net.ResolveTCPAddr(netw, localIP+":0")
				if err != nil {
					return nil, err
				}

				rAddr, err := net.ResolveTCPAddr(netw, addr)
				if err != nil {
					return nil, err
				}
				conn, err := net.DialTCP(netw, lAddr, rAddr)
				if err != nil {
					return nil, err
				}
				return conn, nil
			},
		},
	}
	return client.Do(req)
}
