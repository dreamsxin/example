## 使用

```go
package main

import (
	"fmt"
	"os"
	"runtime"
	"runtime/pprof"
	"time"

	"github.com/shirou/gopsutil/v3/cpu"
	"github.com/shirou/gopsutil/v3/mem"
	"github.com/shirou/gopsutil/v3/process"
)

func main() {
	v, _ := mem.VirtualMemory()

	// almost every return value is a struct
	fmt.Printf("Total: %v, Free:%v, UsedPercent:%f%%\n", v.Total, v.Free, v.UsedPercent)

	// convert to JSON. String() is also implemented
	fmt.Println(v)

	c, _ := cpu.Times(false)
	for i, cpu := range c {
		fmt.Printf("CPU[%d]: %v\n", i, cpu)
	}

	physicalCnt, _ := cpu.Counts(false)
	logicalCnt, _ := cpu.Counts(true)
	fmt.Printf("physical count:%d logical count:%d\n", physicalCnt, logicalCnt)

	// totalPercent, _ := cpu.Percent(3*time.Second, false)
	// perPercents, _ := cpu.Percent(3*time.Second, true)
	// fmt.Printf("total percent:%v per percents:%v", totalPercent, perPercents)

	var rootProcess *process.Process
	processes, _ := process.Processes()
	for _, p := range processes {
		if p.Pid == 0 {
			rootProcess = p
			fmt.Println("rootProcess", rootProcess.Pid, "mypid", os.Getpid())
			fmt.Println("children list:")
			children, _ := rootProcess.Children()
			for _, p := range children {
				fmt.Println("children", p.Pid)
			}
		} else {
			fmt.Println("pid", p.Pid)
			// 返回指定时间内进程占用CPU时间的比例
			cpuPercent, err := p.Percent(time.Second)
			if err != nil {
				panic(err)
			}
			fmt.Println("cpuPercent", cpuPercent)

			// 获取进程占用内存的比例
			mp, _ := p.MemoryPercent()
			fmt.Println("memoryPercent", mp)

		}
	}

	proc, err := process.NewProcess(int32(os.Getpid()))
	if err != nil {
		fmt.Println(err)
	}
	go func() {
		for {
			time.Sleep(time.Millisecond * 100)
		}
	}()

	for {
		// 返回指定时间内进程占用CPU时间的比例
		cpuPercent, err := proc.Percent(time.Second)
		if err != nil {
			panic(err)
		}
		fmt.Println("cpuPercent", cpuPercent)

		// 获取进程占用内存的比例
		mp, _ := proc.MemoryPercent()
		fmt.Println("memoryPercent", mp)

		// 上面返回的是占所有CPU核心时间的比例，如果想更直观的看占比，可以算一下占单个核心的比例
		cp := cpuPercent / float64(runtime.NumCPU())
		fmt.Println("cp", cp)

		// 创建的线程数
		threadCount := pprof.Lookup("threadcreate").Count()
		fmt.Println("threadCount", threadCount)

		// Goroutine数
		gNum := runtime.NumGoroutine()
		fmt.Println("gNum", gNum)

		time.Sleep(time.Second * 2)
	}
}
```
