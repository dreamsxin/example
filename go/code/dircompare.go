/*


## 新增参数说明

| 参数                 | 简写 | 作用                                                         |
| -------------------- | ---- | ------------------------------------------------------------ |
| `-ignore-missing`    | `-i` | 在目录对比模式下，忽略“仅存在于一个目录”的文件，只比较两边都存在的文件。 |

## 使用示例

### 忽略缺失文件，只关心内容不同
```bash
./dircompare -i backup1/ backup2/
```
输出示例：
```
对比目录:
  backup1/
  backup2/
(已启用 -ignore-missing，将忽略仅存在于一个目录的文件)

内容不同: config/app.conf
  MD5(backup1/) = 5d41402abc4b2a76b9719d911017c592
  MD5(backup2/) = 7d793037a0760186574b0282f2f435e7

总结: 存在内容不同的文件
```

### 不忽略缺失文件（默认行为）
```bash
./dircompare backup1/ backup2/
```
会同时输出缺失文件和内容不同的文件。

## 注意事项
- `-ignore-missing` 仅对目录对比模式有效，文件对比模式下自动忽略。
- 该参数不会影响 MD5 计算过程，只在最终结果过滤时生效，因此效率不受影响。

*/

package main

import (
	"crypto/md5"
	"flag"
	"fmt"
	"io"
	"os"
	"path/filepath"
	"runtime"
	"sync"
)

// FileInfo 存储文件的MD5哈希值
type FileInfo struct {
	RelPath string
	Hash    string
	Err     error
}

// CompareResult 对比结果
type CompareResult struct {
	RelPath    string
	Hash1      string
	Hash2      string
	OnlyInDir1 bool
	OnlyInDir2 bool
	Different  bool
}

func main() {
	// 解析命令行参数
	var workers int
	var ignoreMissing bool
	flag.IntVar(&workers, "j", runtime.NumCPU(), "并发计算MD5的工作协程数（仅目录模式有效）")
	flag.BoolVar(&ignoreMissing, "ignore-missing", false, "忽略仅存在于一个目录的文件（仅目录模式）")
	flag.BoolVar(&ignoreMissing, "i", false, "忽略仅存在于一个目录的文件（简写）")
	flag.Usage = func() {
		fmt.Fprintf(os.Stderr, "用法: %s [选项] <路径1> <路径2>\n", os.Args[0])
		fmt.Fprintf(os.Stderr, "选项:\n")
		flag.PrintDefaults()
		fmt.Fprintf(os.Stderr, `
对比模式:
  1. 两个都是目录: 对比两个目录中相同相对路径的文件的MD5值
  2. 两个都是文件: 对比这两个文件的MD5值
  3. 一个是目录一个是文件: 报错退出

示例:
  %s dir1 dir2                    # 对比两个目录
  %s -i dir1 dir2                 # 对比两个目录，忽略缺失文件
  %s file1.txt file2.txt          # 对比两个文件
`, os.Args[0], os.Args[0], os.Args[0])
	}
	flag.Parse()

	if flag.NArg() != 2 {
		flag.Usage()
		os.Exit(1)
	}

	path1 := flag.Arg(0)
	path2 := flag.Arg(1)

	// 获取文件状态信息，决定模式
	info1, err1 := os.Stat(path1)
	info2, err2 := os.Stat(path2)

	if err1 != nil || err2 != nil {
		fmt.Fprintf(os.Stderr, "错误: 无法访问路径\n")
		if err1 != nil {
			fmt.Fprintf(os.Stderr, "  %s: %v\n", path1, err1)
		}
		if err2 != nil {
			fmt.Fprintf(os.Stderr, "  %s: %v\n", path2, err2)
		}
		os.Exit(1)
	}

	isDir1 := info1.IsDir()
	isDir2 := info2.IsDir()

	if isDir1 && isDir2 {
		// 目录对比模式
		if err := compareDirectories(path1, path2, workers, ignoreMissing); err != nil {
			fmt.Fprintf(os.Stderr, "错误: %v\n", err)
			os.Exit(1)
		}
	} else if !isDir1 && !isDir2 {
		// 文件对比模式
		if err := compareFiles(path1, path2); err != nil {
			fmt.Fprintf(os.Stderr, "错误: %v\n", err)
			os.Exit(1)
		}
	} else {
		fmt.Fprintf(os.Stderr, "错误: 两个路径类型不一致，一个为目录另一个为文件\n")
		os.Exit(1)
	}
}

// compareFiles 对比两个具体文件的MD5
func compareFiles(file1, file2 string) error {
	fmt.Printf("对比文件:\n  %s\n  %s\n\n", file1, file2)

	hash1, err1 := md5File(file1)
	hash2, err2 := md5File(file2)

	if err1 != nil {
		return fmt.Errorf("读取文件1失败: %w", err1)
	}
	if err2 != nil {
		return fmt.Errorf("读取文件2失败: %w", err2)
	}

	fmt.Printf("MD5(%s) = %s\n", file1, hash1)
	fmt.Printf("MD5(%s) = %s\n", file2, hash2)

	if hash1 == hash2 {
		fmt.Println("\n结论: 两个文件内容相同 ✓")
	} else {
		fmt.Println("\n结论: 两个文件内容不同 ✗")
	}
	return nil
}

// compareDirectories 对比两个目录
func compareDirectories(dir1, dir2 string, workers int, ignoreMissing bool) error {
	// 检查目录是否存在
	if err := checkDir(dir1); err != nil {
		return err
	}
	if err := checkDir(dir2); err != nil {
		return err
	}

	fmt.Printf("对比目录:\n  %s\n  %s\n", dir1, dir2)
	if ignoreMissing {
		fmt.Println("(已启用 -ignore-missing，将忽略仅存在于一个目录的文件)")
	}
	fmt.Println()

	// 并行计算两个目录的文件哈希
	hashMap1, err := computeHashes(dir1, workers)
	if err != nil {
		return fmt.Errorf("计算目录 %s 哈希时出错: %w", dir1, err)
	}
	hashMap2, err := computeHashes(dir2, workers)
	if err != nil {
		return fmt.Errorf("计算目录 %s 哈希时出错: %w", dir2, err)
	}

	// 对比并输出结果
	results := compareMaps(hashMap1, hashMap2, ignoreMissing)

	// 打印结果
	printResults(results, dir1, dir2, ignoreMissing)
	return nil
}

// checkDir 检查目录是否存在且可读
func checkDir(path string) error {
	info, err := os.Stat(path)
	if err != nil {
		return fmt.Errorf("无法访问目录 %s: %w", path, err)
	}
	if !info.IsDir() {
		return fmt.Errorf("%s 不是一个目录", path)
	}
	return nil
}

// computeHashes 遍历目录，并发计算所有常规文件的MD5哈希值
// 返回 map[相对路径]哈希值
func computeHashes(root string, workers int) (map[string]string, error) {
	// 收集所有需要处理的文件路径
	fileChan := make(chan string, 1000)
	resultChan := make(chan FileInfo, 1000)
	var wg sync.WaitGroup

	// 启动工作协程
	for i := 0; i < workers; i++ {
		wg.Add(1)
		go func() {
			defer wg.Done()
			for filePath := range fileChan {
				relPath, err := filepath.Rel(root, filePath)
				if err != nil {
					resultChan <- FileInfo{RelPath: filePath, Err: fmt.Errorf("获取相对路径失败: %w", err)}
					continue
				}
				// 转换为统一格式（使用斜杠），便于跨平台比较
				relPath = filepath.ToSlash(relPath)
				hash, err := md5File(filePath)
				resultChan <- FileInfo{RelPath: relPath, Hash: hash, Err: err}
			}
		}()
	}

	// 遍历目录并发送文件路径
	go func() {
		err := filepath.WalkDir(root, func(path string, d os.DirEntry, err error) error {
			if err != nil {
				// 忽略访问错误（如权限），但记录到 stderr
				fmt.Fprintf(os.Stderr, "警告: 跳过 %s: %v\n", path, err)
				return nil
			}
			if d.IsDir() {
				return nil
			}
			// 只处理常规文件（忽略符号链接、设备等）
			info, err := d.Info()
			if err != nil {
				fmt.Fprintf(os.Stderr, "警告: 无法获取 %s 文件信息: %v\n", path, err)
				return nil
			}
			if !info.Mode().IsRegular() {
				return nil
			}
			fileChan <- path
			return nil
		})
		if err != nil {
			fmt.Fprintf(os.Stderr, "遍历目录 %s 失败: %v\n", root, err)
		}
		close(fileChan)
	}()

	// 等待所有工作协程完成，并关闭结果通道
	go func() {
		wg.Wait()
		close(resultChan)
	}()

	// 收集结果
	hashMap := make(map[string]string)
	for res := range resultChan {
		if res.Err != nil {
			fmt.Fprintf(os.Stderr, "警告: 处理文件 %s 时出错: %v\n", res.RelPath, res.Err)
			continue
		}
		hashMap[res.RelPath] = res.Hash
	}
	return hashMap, nil
}

// md5File 计算文件的MD5哈希值
func md5File(filePath string) (string, error) {
	f, err := os.Open(filePath)
	if err != nil {
		return "", fmt.Errorf("打开文件失败: %w", err)
	}
	defer f.Close()

	hash := md5.New()
	if _, err := io.Copy(hash, f); err != nil {
		return "", fmt.Errorf("读取文件内容失败: %w", err)
	}
	return fmt.Sprintf("%x", hash.Sum(nil)), nil
}

// compareMaps 对比两个哈希映射，找出内容不同的文件
// ignoreMissing 为 true 时，忽略只在一侧存在的文件
func compareMaps(map1, map2 map[string]string, ignoreMissing bool) []CompareResult {
	var results []CompareResult

	// 扫描 map1 和 map2 的并集
	allKeys := make(map[string]bool)
	for k := range map1 {
		allKeys[k] = true
	}
	for k := range map2 {
		allKeys[k] = true
	}

	for relPath := range allKeys {
		hash1, ok1 := map1[relPath]
		hash2, ok2 := map2[relPath]

		res := CompareResult{
			RelPath: relPath,
		}

		if !ok1 {
			res.OnlyInDir2 = true
		} else if !ok2 {
			res.OnlyInDir1 = true
		} else if hash1 != hash2 {
			res.Different = true
			res.Hash1 = hash1
			res.Hash2 = hash2
		}
		// 哈希相同的情况不记录

		// 根据 ignoreMissing 过滤
		if ignoreMissing && (res.OnlyInDir1 || res.OnlyInDir2) {
			continue
		}

		if res.OnlyInDir1 || res.OnlyInDir2 || res.Different {
			results = append(results, res)
		}
	}
	return results
}

// printResults 打印对比结果（目录模式）
func printResults(results []CompareResult, dir1, dir2 string, ignoreMissing bool) {
	if len(results) == 0 {
		fmt.Println("两个目录的内容完全一致（相同相对路径的文件MD5均相同）")
		return
	}

	hasDiff := false
	hasOnly := false
	for _, r := range results {
		switch {
		case r.OnlyInDir1:
			fmt.Printf("仅存在于第一个目录: %s\n", r.RelPath)
			hasOnly = true
		case r.OnlyInDir2:
			fmt.Printf("仅存在于第二个目录: %s\n", r.RelPath)
			hasOnly = true
		case r.Different:
			fmt.Printf("内容不同: %s\n  MD5(%s) = %s\n  MD5(%s) = %s\n",
				r.RelPath, dir1, r.Hash1, dir2, r.Hash2)
			hasDiff = true
		}
	}

	if hasDiff {
		fmt.Println("\n总结: 存在内容不同的文件")
	} else if hasOnly && !ignoreMissing {
		fmt.Println("\n总结: 存在仅在一个目录中的文件，但所有共同文件内容相同")
	} else if hasOnly && ignoreMissing {
		// 理论上 ignoreMissing 时不会出现 hasOnly，但为防止留个输出
		fmt.Println("\n总结: 存在仅在一个目录中的文件（已忽略）")
	} else {
		fmt.Println("\n总结: 所有共同文件内容相同")
	}
}
