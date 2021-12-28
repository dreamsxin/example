## Graceful Shutdown
从 1.8 开始，Go 标准库中的net/http支持了 `Graceful Shutdown`，使得进程可以把现有请求都处理完之后再退出，从而最大限度地减少不一致性给服务端带来的负担。

## Graceful Restart
优雅重启要求更高，它的目标是在进程重启时整个过程要平滑，不要让用户感受到任何异样，不要有任何downtime，也就是停机时间，保证进程持续可用。
因此，Graceful Shutdown 只是实现 Graceful Restart 的一个必要部分。

* 方法一

通过 nginx 反向代理来完成。在重启时，从 nginx 的 upstream 中摘除掉，等一段时间比如1分钟，该进程差不多也处理完了所以请求，实际上已经处于空闲状态了。
这时就可以kill掉该进程并重启，等重启成功之后，再加回到 nginx 对应的 upstream 中去。

* 方法二

FD继承，file descriptor 文件描述符，是 Unix 系统上最常见的概念，everything is file。我们基于一个非常基础的知识点：

进程 fork 出子进程时，子进程会继承父进程打开的 fd。

```go
package main

import (
    "context"
    "flag"
    "fmt"
    "net"
    "net/http"
    "os"
    "os/exec"
    "os/signal"
    "syscall"
)

var (
    upgrade bool
    ln net.Listener
    server *http.Server
)

func init() {
    flag.BoolVar(&upgrade, "upgrade", false, "user can't use this")
}

func hello(w http.ResponseWriter, r *http.Request) {
    fmt.Fprintf(w, "hello world from pid:%d, ppid: %d\n", os.Getpid(), os.Getppid())
}

func main() {
    flag.Parse()
    http.HandleFunc("/", hello)
    server = &http.Server{Addr:":8999",}
    var err error
    if upgrade { //如果是升级重启的进程从参数中获取文件描述符
        fd := os.NewFile(3, "")
        ln,err = net.FileListener(fd)
        if err != nil {
            fmt.Printf("fileListener fail, error: %s\n", err)
            os.Exit(1)
        }
        fd.Close()
    } else {
        ln, err = net.Listen("tcp", server.Addr)
        if err != nil {
            fmt.Printf("listen %s fail, error: %s\n", server.Addr, err)
            os.Exit(1)
        }
    }
    go func() {
        err := server.Serve(ln)
        if err != nil && err != http.ErrServerClosed{
            fmt.Printf("serve error: %s\n", err)
        }
    }()
    setupSignal()
    fmt.Println("over")
}

func setupSignal() {
    ch := make(chan os.Signal, 1)
    signal.Notify(ch, syscall.SIGUSR2, syscall.SIGINT, syscall.SIGTERM)
    sig := <-ch
    switch sig {
    case syscall.SIGUSR2:
        err := forkProcess()
        if err != nil {
            fmt.Printf("fork process error: %s\n", err)
        }
        err = server.Shutdown(context.Background())
        if err != nil {
            fmt.Printf("shutdown after forking process error: %s\n", err)
        }
    case syscall.SIGINT,syscall.SIGTERM:
        signal.Stop(ch)
        close(ch)
        err := server.Shutdown(context.Background())
        if err != nil {
            fmt.Printf("shutdown error: %s\n", err)
        }
    }
}

// os/exec只默认继承stdin stdout stderr。fd 需要在fork之前手动放到 ExtraFiles 中。fd 的序号从3开始。
func forkProcess() error {
    flags := []string{"-upgrade"}
    cmd := exec.Command(os.Args[0], flags...)
    cmd.Stderr = os.Stderr
    cmd.Stdout = os.Stdout
    l,_ := ln.(*net.TCPListener)
    lfd,err := l.File()
    if err != nil {
        return err
    }
    cmd.ExtraFiles = []*os.File{lfd,}
    return cmd.Start()
}
```

```go
package main

import (
	"context"
	"encoding/json"
	"flag"
	"fmt"
	"net"
	"net/http"
	"os"
	"os/signal"
	"path/filepath"
	"syscall"
	"time"
)

type listener struct {
	Addr     string `json:"addr"`
	FD       int    `json:"fd"`
	Filename string `json:"filename"`
}

func importListener(addr string) (net.Listener, error) {
	// Extract the encoded listener metadata from the environment.
	listenerEnv := os.Getenv("LISTENER")
	if listenerEnv == "" {
		return nil, fmt.Errorf("unable to find LISTENER environment variable")
	}

	// Unmarshal the listener metadata.
	var l listener
	err := json.Unmarshal([]byte(listenerEnv), &l)
	if err != nil {
		return nil, err
	}
	if l.Addr != addr {
		return nil, fmt.Errorf("unable to find listener for %v", addr)
	}

	// The file has already been passed to this process, extract the file
	// descriptor and name from the metadata to rebuild/find the *os.File for
	// the listener.
	listenerFile := os.NewFile(uintptr(l.FD), l.Filename)
	if listenerFile == nil {
		return nil, fmt.Errorf("unable to create listener file: %v", err)
	}
	defer listenerFile.Close()

	// Create a net.Listener from the *os.File.
	ln, err := net.FileListener(listenerFile)
	if err != nil {
		return nil, err
	}

	return ln, nil
}

func createListener(addr string) (net.Listener, error) {
	ln, err := net.Listen("tcp", addr)
	if err != nil {
		return nil, err
	}

	return ln, nil
}

func createOrImportListener(addr string) (net.Listener, error) {
	// Try and import a listener for addr. If it's found, use it.
	ln, err := importListener(addr)
	if err == nil {
		fmt.Printf("Imported listener file descriptor for %v.\n", addr)
		return ln, nil
	}

	// No listener was imported, that means this process has to create one.
	ln, err = createListener(addr)
	if err != nil {
		return nil, err
	}

	fmt.Printf("Created listener file descriptor for %v.\n", addr)
	return ln, nil
}

func getListenerFile(ln net.Listener) (*os.File, error) {
	switch t := ln.(type) {
	case *net.TCPListener:
		return t.File()
	case *net.UnixListener:
		return t.File()
	}
	return nil, fmt.Errorf("unsupported listener: %T", ln)
}

func forkChild(addr string, ln net.Listener) (*os.Process, error) {
	// Get the file descriptor for the listener and marshal the metadata to pass
	// to the child in the environment.
	lnFile, err := getListenerFile(ln)
	if err != nil {
		return nil, err
	}
	defer lnFile.Close()
	l := listener{
		Addr:     addr,
		FD:       3,
		Filename: lnFile.Name(),
	}
	listenerEnv, err := json.Marshal(l)
	if err != nil {
		return nil, err
	}

	// Pass stdin, stdout, and stderr along with the listener to the child.
	files := []*os.File{
		os.Stdin,
		os.Stdout,
		os.Stderr,
		lnFile,
	}

	// Get current environment and add in the listener to it.
	environment := append(os.Environ(), "LISTENER="+string(listenerEnv))

	// Get current process name and directory.
	execName, err := os.Executable()
	if err != nil {
		return nil, err
	}
	execDir := filepath.Dir(execName)

	// Spawn child process.
	p, err := os.StartProcess(execName, []string{execName}, &os.ProcAttr{
		Dir:   execDir,
		Env:   environment,
		Files: files,
		Sys:   &syscall.SysProcAttr{},
	})
	if err != nil {
		return nil, err
	}

	return p, nil
}

func waitForSignals(addr string, ln net.Listener, server *http.Server) error {
	signalCh := make(chan os.Signal, 1024)
	signal.Notify(signalCh, syscall.SIGHUP, syscall.SIGUSR2, syscall.SIGINT, syscall.SIGQUIT)
	for {
		select {
		case s := <-signalCh:
			fmt.Printf("%v signal received.\n", s)
			switch s {
			case syscall.SIGHUP:
				// Fork a child process.
				p, err := forkChild(addr, ln)
				if err != nil {
					fmt.Printf("Unable to fork child: %v.\n", err)
					continue
				}
				fmt.Printf("Forked child %v.\n", p.Pid)

				// Create a context that will expire in 5 seconds and use this as a
				// timeout to Shutdown.
				ctx, cancel := context.WithTimeout(context.Background(), 5*time.Second)
				defer cancel()

				// Return any errors during shutdown.
				return server.Shutdown(ctx)
			case syscall.SIGUSR2:
				// Fork a child process.
				p, err := forkChild(addr, ln)
				if err != nil {
					fmt.Printf("Unable to fork child: %v.\n", err)
					continue
				}

				// Print the PID of the forked process and keep waiting for more signals.
				fmt.Printf("Forked child %v.\n", p.Pid)
			case syscall.SIGINT, syscall.SIGQUIT:
				// Create a context that will expire in 5 seconds and use this as a
				// timeout to Shutdown.
				ctx, cancel := context.WithTimeout(context.Background(), 5*time.Second)
				defer cancel()

				// Return any errors during shutdown.
				return server.Shutdown(ctx)
			}
		}
	}
}

func handler(w http.ResponseWriter, r *http.Request) {
	fmt.Fprintf(w, "Hello from %v!\n", os.Getpid())
}

func startServer(addr string, ln net.Listener) *http.Server {
	http.HandleFunc("/hello", handler)

	httpServer := &http.Server{
		Addr: addr,
	}
	go httpServer.Serve(ln)

	return httpServer
}

func main() {
	// Parse command line flags for the address to listen on.
	var addr string
	flag.StringVar(&addr, "addr", ":8080", "Address to listen on.")

	// Create (or import) a net.Listener and start a goroutine that runs
	// a HTTP server on that net.Listener.
	ln, err := createOrImportListener(addr)
	if err != nil {
		fmt.Printf("Unable to create or import a listener: %v.\n", err)
		os.Exit(1)
	}
	server := startServer(addr, ln)

	// Wait for signals to either fork or quit.
	err = waitForSignals(addr, ln, server)
	if err != nil {
		fmt.Printf("Exiting: %v\n", err)
		return
	}
	fmt.Printf("Exiting.\n")
}
```

* 方法三

`RE_USEPORT`

还有第三种可以做到不停机重启的办法，那便是使用Linux内核的新特性reuseport。以前，如果多个进程或者线程同时监听一个端口，只有一个可以成功，其它都会返回端口被占用的错误。
新内核支持通过setsockopt对socket进行设置，使得多个进程或者线程可以同时监听一个端口，内核来进行负载均衡。
