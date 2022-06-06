# NOTIFY

NOTIFY 命令向当前数据库中所有执行过 LISTEN name ，正在监听特定通知条件的前端应用发送一个通知事件。

```shell
LISTEN virtual;
NOTIFY virtual;
Asynchronous notification "virtual" received from server process with PID 8448.
NOTIFY virtual, 'This is the payload';
Asynchronous notification "virtual" with payload "This is the payload" received from server process with PID 8448.

LISTEN foo;
SELECT pg_notify('fo' || 'o', 'pay' || 'load');
Asynchronous notification "foo" with payload "payload" received from server process with PID 14728.
```

## Go 示例

```go
package main

import (
	"database/sql"
	"fmt"
	"log"
	"time"

	"github.com/lib/pq"
)

func main() {
	uri := "user=postgres password={YOUR_PASSWORD} host={YOUR_DB_HOST} port={YOUR_DB_PORT} dbname={YOUR_DB_NAME} sslmode=disable"
	db, err := sql.Open("postgres", uri)
	if err != nil {
		log.Fatal(err)
	}

	if err := db.Ping(); err != nil {
		log.Fatal(err)
	}

	report := func(et pq.ListenerEventType, err error) {
		if err != nil {
			fmt.Println(err)
		}
	}

	listener := pq.NewListener(uri, 10*time.Second, time.Minute, report)
	err = listener.Listen("hello")
	if err != nil {
		log.Fatal(err)
	}

	fmt.Println("-------start listen------------")
	for {
		n := <-listener.Notify
		switch n.Channel {
		case "hello":
			fmt.Println("get notify : ", n.Extra)
		}
	}
}
```
