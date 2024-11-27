
# net/url
```go
package main
import(
    "fmt"
    "net/url"
)
func main()  {
    var urlStr string = "运维之路"
    escapeUrl := url.QueryEscape(urlStr)
    fmt.Println("编码:",escapeUrl)
    enEscapeUrl, _ := url.QueryUnescape(escapeUrl)
    fmt.Println("解码:",enEscapeUrl)
}
```

多个参数

```go
package main
import (
	"fmt"
	"net/url"
)
func main() {
	params := url.Values{}
	params.Add("name", "@Rajeev")
	params.Add("phone", "+919999999999")
	fmt.Println(params.Encode())
}
```

```go
package main
import (
	"fmt"
	"net/url"
)
func main() {
	// Let's start with a base url
	baseUrl, err := url.Parse("http://www.mywebsite.com")
	if err != nil {
		fmt.Println("Malformed URL: ", err.Error())
		return
	}
	// Add a Path Segment (Path segment is automatically escaped)
	baseUrl.Path += "path with?reserved characters"
	// Prepare Query Parameters
	params := url.Values{}
	params.Add("q", "Hello World")
	params.Add("u", "@rajeev")
	// Add Query Parameters to the URL
	baseUrl.RawQuery = params.Encode() // Escape Query Parameters
	fmt.Printf("Encoded URL is %q
", baseUrl.String())
}
#输出结果
Encoded URL is "http://www.mywebsite.com/path%20with%3Freserved%20characters?q=Hello+World&u=%40rajeev"
```

```go
package main
import (
	"fmt"
	"log"
	"net/url"
)
func main() {
	encodedValue := "Hell%C3%B6+W%C3%B6rld%40Golang"
	decodedValue, err := url.QueryUnescape(encodedValue)
	if err != nil {
		log.Fatal(err)
		return
	}
	fmt.Println(decodedValue)
}
```

```go
package main
import (
	"fmt"
	"log"
	"net/url"
)
func main() {
	queryStr := "name=Rajeev%20Singh&phone=%2B9199999999&phone=%2B628888888888"
	params, err := url.ParseQuery(queryStr)
	if err != nil {
		log.Fatal(err)
		return
	}
	fmt.Println("Query Params: ")
	for key, value := range params {
		fmt.Printf("  %v = %v
", key, value)
	}
}
#其输出为：
Query Params:
  name = [Rajeev Singh]
  phone = [+9199999999 +628888888888]
```
