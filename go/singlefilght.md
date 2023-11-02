# singlefilght

在 go 标准库中（`golang.org/x/sync/singleflight`）提供了可重复的函数调用抑制机制。
通过给每次函数调用分配一个key，相同key的函数并发调用时，只会被执行一次，返回相同的结果。其本质是对函数调用的结果进行复用。

## 源码解读

```go
// Group 对外的核心结构体
type Group struct {
	mu sync.Mutex       // 保护 m
	m  map[string]*call // lazily initialized
}

// Do 执行函数, 对同一个 key 多次调用的时候，在第一次调用没有执行完的时候
// 只会执行一次 fn，其他的调用会**阻塞**住等待这次调用返回
// v, err 是传入的 fn 的返回值
// shared 表示fn的结果是否被共享
func (g *Group) Do(key string, fn func() (interface{}, error)) (v interface{}, err error, shared bool)

// DoChan 和 Do 类似，只是 DoChan 返回一个 channel，也就是同步与异步的区别
func (g *Group) DoChan(key string, fn func() (interface{}, error)) <-chan Result

// Forget 用于通知 Group 删除某个 key 这样后面继续这个 key 的调用的时候就不会在阻塞等待了
func (g *Group) Forget(key string){
	g.mu.Lock()
	if c, ok := g.m[key]; ok {
		c.forgotten = true
	}
	delete(g.m, key)
	g.mu.Unlock()
}
```

## Do
```go
// 普通调用方法
func callFunc(i int) (int,error) {
	time.Sleep(500 * time.Millisecond)
	return i, nil
}

// 使用singleflight

// 1. 定义全局变量
var sf singleflight.Group

func callFuncBySF(key string, i int) (int, error) {
	// 2. 调用sf.Do方法
	value, err, shared := sf.Do(key, func() (interface{}, error) {
		return callFunc(i)
	})
	res, _ := value.(int)
	return res, err
}
```

## DoChan

```go
	ch := sf.DoChan(key, func() (interface{}, error) {
	    return callFunc(i)
	})

	select {
	case <-time.After(500 * time.Millisecond): 
		    return
	case <-ctx.Done()
				return
	case ret := <-ch: 
	    go handle(ret)
	}
```

## gin

see https://github.com/janartist/api-cache
