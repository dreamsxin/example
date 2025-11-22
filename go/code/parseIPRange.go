package main

import (
	"encoding/csv"
	"fmt"
	"log"
	"net"
	"os"
	"strconv"
	"strings"
	"time"
)

// parseIPRange 解析IP范围格式，如 "192.168.1.10-20"
func parseIPRange(ipRange string) ([]string, error) {
	// 分割IP和范围部分
	parts := strings.Split(ipRange, "-")
	if len(parts) != 2 {
		return nil, fmt.Errorf("无效的IP范围格式")
	}

	// 解析基础IP
	baseIP := parts[0]
	ip := net.ParseIP(baseIP)
	if ip == nil {
		return nil, fmt.Errorf("无效的IP地址: %s", baseIP)
	}

	// 将IP转换为4字节表示
	ip = ip.To4()
	if ip == nil {
		return nil, fmt.Errorf("不是IPv4地址")
	}

	// 解析结束范围
	end, err := strconv.Atoi(parts[1])
	if err != nil {
		return nil, fmt.Errorf("无效的结束范围: %s", parts[1])
	}

	// 获取起始IP的最后一个字节
	start := int(ip[3])

	// 验证范围
	if start > end {
		return nil, fmt.Errorf("起始IP(%d)不能大于结束IP(%d)", start, end)
	}
	if end > 255 {
		return nil, fmt.Errorf("结束IP(%d)不能大于255", end)
	}

	// 生成所有IP地址
	var ips []string
	for i := start; i <= end; i++ {
		newIP := net.IPv4(ip[0], ip[1], ip[2], byte(i))
		ips = append(ips, newIP.String())
	}

	return ips, nil
}
func main() {

	// 读取 csv 文件
	// 打开文件
	file, err := os.Open("ip.csv")
	if err != nil {
		log.Fatalf("Failed to open file: %v", err)
	}
	defer file.Close()

	// 创建 CSV 读取器
	reader := csv.NewReader(file)

	// 读取所有行
	rows, err := reader.ReadAll()
	if err != nil {
		log.Fatalf("Failed to read CSV: %v", err)
	}
	allipFile, err := os.OpenFile(fmt.Sprintf("allip-%s", time.Now().Format("20060102")), os.O_CREATE|os.O_WRONLY|os.O_TRUNC, 0644)
	if err != nil {
		log.Fatalf("Failed to open aliveip.txt: %v", err)
	}
	defer allipFile.Close()
	// 打印所有行 http://7hQPiPOpR0:ekBSKA3Ez3@192.161.48.50:5206
	for i, row := range rows {
		if i == 0 {
			continue
		}
		//fmt.Println(row)
		// 打印 ip 地址
		fmt.Println(row[5])
		fmt.Println(row[6])
		user := row[6]
		allips := strings.Split(row[5], "\n")
		for _, ips := range allips {
			fmt.Println(ips)
			ips, err := parseIPRange(ips)
			if err != nil {
				log.Fatalf("Failed to parse IP range: %v", err)
			}
			fmt.Println(ips)
			for _, ip := range ips {
				proxyStr := fmt.Sprintf("http://%s@%s:5206", user, ip)
				fmt.Println(proxyStr)
				allipFile.WriteString(proxyStr + "\n")

			}
		}
	}
}
