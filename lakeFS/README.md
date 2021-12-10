## lakeFS

https://github.com/treeverse/lakeFS

基于golang编写的兼容git 的对象存储.

```go
type ChanLocker struct {
   sync.Map
}

func(w *ChanLocker) Lock(key interface{}, acquireFn func()) bool {
   waitCh := make(chan struct{})
   actual, locked := w.LoadOrStore(key, waitCh)
   if !locked {
       acquireFn()
       w.Delete(key)
       close(waitCh)
       return true
   }
   <-actual.(chan struct{})
   return false
}

var (
    lruCache      lru.Cache
    acquireLocker ChanLocker
 
    ErrCacheItemNotFound = errors.New("cache item not found")
)
 
func GetUser(id string) (interface{}, error) {
    v, ok := lruCache.Get(id)
    if ok {
        return v, nil
    }
    var err error
    acquired := acquireLocker.Lock(id, func() {
        v, err = expensiveDBLookup(id)
        if err != nil {
            return
        }
        lruCache.Add(id, v)
    })
    if acquired {
        return v, err
    }
    v, ok = lruCache.Get(id)
    if !ok {
        return nil, ErrCacheItemNotFound
    }
    return v, nil
}

const (
   expiryDuration = 20 * time.Second
   jitterDuration =  3 * time.Second
)
 
expire := expireDuration + time.Duration(rand.Intn(int(jitterDuration)))
lru.AddEx(key, v, expire)
```
