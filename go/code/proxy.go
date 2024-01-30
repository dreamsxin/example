func Proxy() {
	dialer, err := proxy.SOCKS5("tcp", "localhost:8080", &proxy.Auth{
		User:     "",
		Password: "",
	}, proxy.Direct)

	if err != nil {
		panic(err)
	}
	client := http.Client{
		Transport: &http.Transport{
			Dial: dialer.Dial,
		},
	}
	request, _ := http.NewRequest("GET", "https://myip.top", nil)
	response, err := client.Do(request)
	if err != nil {
		panic(err)
	}
	defer response.Body.Close()
	body, err := io.ReadAll(response.Body)
	if err != nil {
		panic(err)
	}
	log.Println("response body", string(body))
  
	// conn, err := dialer.Dial("tcp", net.JoinHostPort("myip.top", "443"))
	// if err != nil {
	// 	log.Println(err)
	// 	return
	// }

	// defer conn.Close()

	// conn.Write([]byte("myip.top" + "\r\n"))

	// b, err := io.ReadAll(conn)

	// if err != nil {
	// 	log.Println(err)
	// 	return
	// }
	// log.Println(string(b))
}
