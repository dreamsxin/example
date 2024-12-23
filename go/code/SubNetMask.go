package main

import (
	"bytes"
	"fmt"
	"net"
	"net/netip"
	"strconv"
	"strings"
	"encoding/binary"
)

func ip4To6() {
	ip := net.ParseIP("127.0.0.1")
	if ip == nil {
		fmt.Println("IP address is not valid")
		return
	}

	if ip.To4() != nil {
		fmt.Println("IP address is v4", ip, netip.AddrFrom16([16]byte(ip)).String())
	} else {
		fmt.Println("IP address is v6")
	}

	data := netip.MustParseAddr("::ffff:1.2.3.4").As16()
	ip4As6 := net.IP(data[:])

	fmt.Println("ip4As6 address is v4", ip4As6.String(), netip.AddrFrom16([16]byte(ip)).String())
}

// ip 地址转 二进制
func ipToBinary(ipStr string) (string, error) {
    ip := net.ParseIP(ipStr).To4()
    if ip == nil {
        return "", fmt.Errorf("invalid IP address: %s", ipStr)
    }
 
    // 转换为大端序uint32
    uint32IP := binary.BigEndian.Uint32(ip)
 
    // 转换为二进制字符串
    return strconv.FormatUint(uint64(uint32IP), 2), nil
}

func ipAddrToInt(ipAddr string) int64 {
	bits := strings.Split(ipAddr, ".")
	b0, _ := strconv.Atoi(bits[0])
	b1, _ := strconv.Atoi(bits[1])
	b2, _ := strconv.Atoi(bits[2])
	b3, _ := strconv.Atoi(bits[3])
	var sum int64
	sum += int64(b0) << 24
	sum += int64(b1) << 16
	sum += int64(b2) << 8
	sum += int64(b3)

	return sum
}

// 二进制 转 ip
func binaryToIP(binaryIP string) (string, error) {
    // 将二进制字符串转换为[]byte
    ipBytes, err := hex.DecodeString(binaryIP)
    if err != nil {
        return "", err
    }
 
    // 将[]byte转换为net.IP
    ip := net.IP(ipBytes)
 
    // 将net.IP转换为字符串
    return ip.String(), nil
}

func UInt32ToIP(intIP uint32) net.IP {
	var bytes [4]byte
	bytes[0] = byte(intIP & 0xFF)
	bytes[1] = byte((intIP >> 8) & 0xFF)
	bytes[2] = byte((intIP >> 16) & 0xFF)
	bytes[3] = byte((intIP >> 24) & 0xFF)

	return net.IPv4(bytes[3], bytes[2], bytes[1], bytes[0])
}


// 如 255.255.255.0 对应的网络位长度为 24
func SubNetMaskToLen(netmask string) (int, error) {
	ipSplitArr := strings.Split(netmask, ".")
	if len(ipSplitArr) != 4 {
		return 0, fmt.Errorf("netmask:%v is not valid, pattern should like: 255.255.255.0", netmask)
	}
	ipv4MaskArr := make([]byte, 4)
	for i, value := range ipSplitArr {
		intValue, err := strconv.Atoi(value)
		if err != nil {
			return 0, fmt.Errorf("ipMaskToInt call strconv.Atoi error:[%v] string value is: [%s]", err, value)
		}
		if intValue > 255 {
			return 0, fmt.Errorf("netmask cannot greater than 255, current value is: [%s]", value)
		}
		ipv4MaskArr[i] = byte(intValue)
	}

	ones, _ := net.IPv4Mask(ipv4MaskArr[0], ipv4MaskArr[1], ipv4MaskArr[2], ipv4MaskArr[3]).Size()
	return ones, nil
}

// 如 24 对应的子网掩码地址为 255.255.255.0
func LenToSubNetMask(subnet int) string {
	var buff bytes.Buffer
	for i := 0; i < subnet; i++ {
		buff.WriteString("1")
	}
	for i := subnet; i < 32; i++ {
		buff.WriteString("0")
	}
	masker := buff.String()
	a, _ := strconv.ParseUint(masker[:8], 2, 64)
	b, _ := strconv.ParseUint(masker[8:16], 2, 64)
	c, _ := strconv.ParseUint(masker[16:24], 2, 64)
	d, _ := strconv.ParseUint(masker[24:32], 2, 64)
	resultMask := fmt.Sprintf("%v.%v.%v.%v", a, b, c, d)
	return resultMask
}
