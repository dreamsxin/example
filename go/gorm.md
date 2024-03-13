
## 缓存

```go
package start

import (
	"context"
	"log"
	"sync"

	"github.com/go-gorm/caches/v4"
)

type memoryCacher struct {
	store *sync.Map
}

func (c *memoryCacher) init() {
	if c.store == nil {
		log.Println("---------init------------")
		c.store = &sync.Map{}
	}
}

func (c *memoryCacher) Get(ctx context.Context, key string, q *caches.Query[any]) (*caches.Query[any], error) {
	c.init()

	val, ok := c.store.Load(key)
	if !ok {
		log.Println("---------memoryCacher.Get------------", key)
		return nil, nil
	}

	log.Println("---------memoryCacher.Get------------", key, string(val.([]byte)))
	if err := q.Unmarshal(val.([]byte)); err != nil {
		return nil, err
	}

	return q, nil
}

func (c *memoryCacher) Store(ctx context.Context, key string, val *caches.Query[any]) error {
	c.init()
	res, err := val.Marshal()
	if err != nil {
		log.Println("---------memoryCacher.Store------------", key, err)
		return err
	}

	log.Println("---------memoryCacher.Store------------", key, string(res))
	c.store.Store(key, res)
	val2, ok := c.store.Load(key)

	log.Println("---------memoryCacher.Load------------", key, ok, string(val2.([]byte)))
	return nil
}

// 清理
func (c *memoryCacher) Invalidate(ctx context.Context) error {
	c.store = &sync.Map{}
	return nil
}

// gorm cache
cachesPlugin := &caches.Caches{Conf: &caches.Config{
	Easer:  true,
	Cacher: &memoryCacher{},
}}

err = db.Use(cachesPlugin)
```
