- https://wakemeonlan.findmysoft.com/
- https://github.com/nicolasarmenti/WakeMeOnLan/blob/main/src/WakeMeOnLan/WOL.cs

```go
package main

import (
	"bytes"
	"context"
	"encoding/hex"
	"fmt"
	"log"
	"net"
	"os"
	"regexp"
	"time"
)

func WakeOnLan(macAddress string) error {
	magicPacket := BuildMagicPacket(macAddress)
	ifaces, err := net.Interfaces()
	if err != nil {
		return err
	}

	log.Printf("Found %d interfaces\n", len(ifaces))
	for _, iface := range ifaces {
		if iface.Flags&net.FlagLoopback != 0 || iface.Flags&net.FlagUp == 0 {
			log.Printf("Skipping interface %s\n", iface.Name)
			continue
		}

		addrs, err := iface.Addrs()
		if err != nil {
			return err
		}

		for _, addr := range addrs {
			ip, network, err := net.ParseCIDR(addr.String())
			if err != nil {
				return err
			}

			if ip.To16() != nil {
				ifaceIP := ip.To16()
				if ifaceIP != nil && !ifaceIP.IsLinkLocalUnicast() {
					multicastIP := network.IP.To16()
					if err := SendWakeOnLan(context.Background(), ifaceIP, multicastIP, magicPacket); err != nil {
						return err
					}
					break
				}
			} else if ip.To4() != nil {
				ifaceIP := ip.To4()
				if ifaceIP != nil {
					// 计算 IPv4 广播地址
					multicastIP := network.IP.To4()
					if err := SendWakeOnLan(context.Background(), ifaceIP, multicastIP, magicPacket); err != nil {
						return err
					}
					break
				}
			} else {
				log.Printf("Skipping interface %s with address %s\n", iface.Name, addr.String())
			}
		}
	}

	return nil
}

func BuildMagicPacket(macAddress string) []byte {
	// 去除 macAddress 中的分隔符
	re := regexp.MustCompile("[: -]")
	macAddress = re.ReplaceAllString(macAddress, "")

	// 将十六进制字符串转换为字节数组
	macBytes, err := hex.DecodeString(macAddress)
	if err != nil {
		panic(err)
	}

	// 确保 macBytes 长度为 6
	if len(macBytes) != 6 {
		panic("Invalid MAC address length")
	}

	// 创建一个缓冲区来构建 magic packet
	var buffer bytes.Buffer

	// 写入 6 个 0xFF 字节
	for i := 0; i < 6; i++ {
		buffer.WriteByte(0xFF)
	}

	// 写入 16 次 MAC 地址
	for i := 0; i < 16; i++ {
		buffer.Write(macBytes)
	}

	// 返回构建好的 magic packet
	return buffer.Bytes()
}

func SendWakeOnLan(ctx context.Context, ifaceIP, multicastIP net.IP, magicPacket []byte) error {
	// 创建一个 UDP 连接
	conn, err := net.ListenUDP("udp", &net.UDPAddr{IP: ifaceIP})
	if err != nil {
		return err
	}
	defer conn.Close()

	// 设置连接超时时间为 1 秒
	conn.SetDeadline(time.Now().Add(time.Second))

	// 发送 magic packet 到指定的多播地址
	_, err = conn.WriteToUDP(magicPacket, &net.UDPAddr{IP: multicastIP, Port: 9})
	if err != nil {
		return err
	}
	log.Printf("Sent magic packet to %s via %s\n", multicastIP, ifaceIP)
	return nil
}

func main() {
	log.SetFlags(log.LstdFlags | log.Lshortfile)
	// 示例用法
	macAddress := "00:1A:2B:3C:4D:5E"
	if err := WakeOnLan(macAddress); err != nil {
		fmt.Printf("Error: %v", err)
		os.Exit(1)
	}
}
```
