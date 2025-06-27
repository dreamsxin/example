# Cache组件

##

https://github.com/w1ck3dg0ph3r/dbadger

##

https://github.com/olric-data/olric

## SugarDB

https://github.com/EchoVault/SugarDB

## bytestorage

https://github.com/kiriklo/bytestorage

## ccache

https://github.com/karlseguin/ccache

##

https://github.com/jellydator/ttlcache

## ccache

https://github.com/karlseguin/ccache

## BigCache

https://github.com/allegro/bigcache

## freecache:

https://github.com/coocood/freecache
内存中的长寿命对象会引入昂贵的 GC 开销，使用 FreeCache，您可以在内存中缓存无限数量的对象，而不会增加延迟和降低吞吐量。

* Features

存储数以亿计的条目
零 GC 开销
高并发线程安全访问
纯 Go 实现
支持淘汰
近似 LRU 算法
严格限制内存使用
附带一个toy server，支持一些带有pipeline的基本 Redis 命令
支持Iterator

* 注意

内存是预先分配的。
如果分配大量内存，则可能需要将 debug.SetGCPercent() 设置为低得多的百分比以获得正常的 GC 频率。

* example
```go
// In bytes, where 1024 * 1024 represents a single Megabyte, and 100 * 1024*1024 represents 100 Megabytes.
cacheSize := 100 * 1024 * 1024
cache := freecache.NewCache(cacheSize)
debug.SetGCPercent(20)
key := []byte("abc")
val := []byte("def")
expire := 60 // expire in 60 seconds
cache.Set(key, val, expire)
got, err := cache.Get(key)
if err != nil {
    fmt.Println(err)
} else {
    fmt.Printf("%s\n", got)
}
affected := cache.Del(key)
fmt.Println("deleted key ", affected)
fmt.Println("entry count ", cache.EntryCount())
```

## groupcache:

https://github.com/golang/groupcache

groupcache is a distributed caching and cache-filling library, intended as a replacement for a pool of memcached nodes in many cases.

内部支持lrucache：

https://github.com/golang/groupcache/tree/master/lru

注意lru cache的实现并非线程安全：Cache is an LRU cache. It is not safe for concurrent access.

主要方法：
```go
// New creates a new Cache.
// If maxEntries is zero, the cache has no limit and it's assumed
// that eviction is done by the caller.
func New(maxEntries int) *Cache 
 
// Add adds a value to the cache.
func (c *Cache) Add(key Key, value interface{}) 
 
// Get looks up a key's value from the cache.
func (c *Cache) Get(key Key) (value interface{}, ok bool)
 
// Remove removes the provided key from the cache.
func (c *Cache) Remove(key Key)
 
// RemoveOldest removes the oldest item from the cache.
func (c *Cache) RemoveOldest()
 
func (c *Cache) removeElement(e *list.Element) 
 
// Len returns the number of items in the cache.
func (c *Cache) Len() int 
 
// Clear purges all stored items from the cache.
func (c *Cache) Clear() 
```

## golang-lru:

https://github.com/hashicorp/golang-lru

This provides the lru package which implements a fixed-size thread safe LRU cache. It is based on the cache in Groupcache.

特点：线程安全的LRU CACHE
