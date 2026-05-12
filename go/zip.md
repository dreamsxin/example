# 验证 ZIP 压缩包内的文件是否损坏

它通过重新计算每个文件的 CRC32（ZIP 格式内置的校验码）并与压缩包内存储的值对比，从而判断文件数据是否完整。

## 原理说明
ZIP 文件中每个被压缩的文件都附带一个 **CRC32 校验码**。解压时，解压工具通常会重新计算数据的 CRC32 并与存储值比对，若不匹配则报错。本程序直接读取 ZIP 包内每个文件的数据流，实时计算 CRC32，并与 ZIP 内记录的 CRC32 进行比较，实现**不依赖外部原始文件**的自校验。

## 完整 Go 程序

```go
package main

import (
	"archive/zip"
	"flag"
	"fmt"
	"hash/crc32"
	"io"
	"os"
	"path/filepath"
	"runtime"
	"sync"
)

func main() {
	var workers int
	var verbose bool
	flag.IntVar(&workers, "j", runtime.NumCPU(), "并发校验的工作协程数")
	flag.BoolVar(&verbose, "v", false, "详细输出每个文件的校验结果")
	flag.Usage = func() {
		fmt.Fprintf(os.Stderr, "用法: %s [选项] <zip文件>\n", os.Args[0])
		fmt.Fprintf(os.Stderr, "选项:\n")
		flag.PrintDefaults()
		fmt.Fprintf(os.Stderr, `
说明: 检查ZIP压缩包内的每个文件是否损坏。
程序会读取每个文件的内容，重新计算CRC32，与ZIP内记录的CRC32比对。
如果一致，则文件未损坏；否则说明文件数据已损坏。
`)
	}
	flag.Parse()

	if flag.NArg() != 1 {
		flag.Usage()
		os.Exit(1)
	}
	zipPath := flag.Arg(0)

	// 打开ZIP文件
	r, err := zip.OpenReader(zipPath)
	if err != nil {
		fmt.Fprintf(os.Stderr, "打开ZIP文件失败: %v\n", err)
		os.Exit(1)
	}
	defer r.Close()

	// 收集所有需要校验的文件（排除目录）
	type fileTask struct {
		name    string
		crc32   uint32
		offset  int64 // 用于进度显示（可选）
	}
	var tasks []fileTask
	for _, f := range r.File {
		if f.FileInfo().IsDir() {
			continue
		}
		tasks = append(tasks, fileTask{
			name:  f.Name,
			crc32: f.CRC32,
		})
	}

	if len(tasks) == 0 {
		fmt.Println("ZIP文件中没有需要校验的文件（可能只有空目录）")
		return
	}

	fmt.Printf("开始校验 ZIP 文件: %s\n", zipPath)
	fmt.Printf("共 %d 个文件，使用 %d 个并发协程\n\n", len(tasks), workers)

	// 并发校验
	taskChan := make(chan fileTask, workers)
	resultChan := make(chan struct {
		name    string
		ok      bool
		err     error
	}, len(tasks))

	var wg sync.WaitGroup
	for i := 0; i < workers; i++ {
		wg.Add(1)
		go func() {
			defer wg.Done()
			for task := range taskChan {
				ok, err := verifyFileInZip(r, task.name, task.crc32)
				resultChan <- struct {
					name    string
					ok      bool
					err     error
				}{task.name, ok, err}
			}
		}()
	}

	// 发送任务
	go func() {
		for _, task := range tasks {
			taskChan <- task
		}
		close(taskChan)
	}()

	// 等待所有任务完成并关闭结果通道
	go func() {
		wg.Wait()
		close(resultChan)
	}()

	// 收集结果
	var passed, failed int
	for res := range resultChan {
		if res.err != nil {
			fmt.Printf("[错误] %s : %v\n", res.name, res.err)
			failed++
		} else if res.ok {
			if verbose {
				fmt.Printf("[通过] %s\n", res.name)
			}
			passed++
		} else {
			fmt.Printf("[损坏] %s (CRC32 不匹配)\n", res.name)
			failed++
		}
	}

	fmt.Printf("\n校验完成: 通过 %d, 失败 %d\n", passed, failed)
	if failed > 0 {
		os.Exit(1)
	}
}

// verifyFileInZip 校验ZIP内的一个具体文件
// 参数：zip reader, 文件名, 期望的CRC32
// 返回：true=CRC32匹配，false=不匹配；以及可能发生的IO错误
func verifyFileInZip(r *zip.ReadCloser, name string, expectedCRC uint32) (bool, error) {
	// 在ZIP中查找该文件
	var f *zip.File
	for _, file := range r.File {
		if file.Name == name {
			f = file
			break
		}
	}
	if f == nil {
		return false, fmt.Errorf("在ZIP中找不到文件 %s", name)
	}

	// 打开文件内容流
	rc, err := f.Open()
	if err != nil {
		return false, fmt.Errorf("打开文件流失败: %w", err)
	}
	defer rc.Close()

	// 重新计算CRC32
	hasher := crc32.NewIEEE()
	if _, err := io.Copy(hasher, rc); err != nil {
		return false, fmt.Errorf("读取文件内容失败: %w", err)
	}
	actualCRC := hasher.Sum32()

	return actualCRC == expectedCRC, nil
}
```

## 使用方法

### 1. 编译
```bash
go build -o zipcheck zipcheck.go
```

### 2. 运行
```bash
./zipcheck yourfile.zip
```

可选的参数：
- `-j 8`：使用 8 个协程并发校验（默认 CPU 核心数）
- `-v`：输出每个成功文件的校验结果（默认静默，只显示损坏或错误的文件）

### 3. 示例输出
```bash
$ ./zipcheck -v archive.zip
开始校验 ZIP 文件: archive.zip
共 4 个文件，使用 8 个并发协程

[通过] docs/readme.txt
[损坏] images/logo.png (CRC32 不匹配)
[通过] config/settings.json
[错误] data/log.txt : 打开文件流失败: zip: not a valid zip file

校验完成: 通过 2, 失败 2
```

## 如何保证 ZIP 压缩目录中的文件都没有损坏？

1. **使用标准工具快速检查**  
   大多数系统自带的 `unzip` 提供了测试功能：  
   ```bash
   unzip -t archive.zip
   ```  
   该命令会模拟解压并校验每个文件的 CRC32，输出测试结果。

2. **使用本程序进行更精细的控制**  
   - 支持并发校验，速度快。  
   - 可集成到自动化脚本中，根据退出码（0=全部正确，非0=有损坏）判断。  
   - 支持大 ZIP 文件流式读取，内存占用低。

3. **对比原始目录与 ZIP 内容**（若需要更严格的完整性验证）  
   如果要确认 ZIP 包里的文件与某个原始目录的内容**完全一致**（不仅仅是压缩没损坏），可以先解压 ZIP，再用之前提供的 **`dircompare` 程序**对比原始目录与解压后的目录。  
   示例：
   ```bash
   unzip archive.zip -d temp_extract
   ./dircompare original_dir/ temp_extract/
   ```

## 总结
- **仅检查 ZIP 文件自身是否损坏**：使用 `unzip -t` 或本程序。  
- **检查 ZIP 文件与外部目录是否一致**：解压后使用目录对比工具。  

本程序提供了纯 Go 实现的 ZIP 完整性自检方案，无需外部命令，跨平台可用。
