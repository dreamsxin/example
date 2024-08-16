package main

import (
	"encoding/binary"
	"log"
	"math"
	"net"
	"strconv"
)

func GenSubnetsInNetwork() {
	subnetMaskSize := 26
	ip, ipNet, err := net.ParseCIDR("192.168.0.0/24")
	if err != nil {
		log.Panicln(err)
	}
	if !ip.Equal(ipNet.IP) {
		log.Panicln("netCIDR is not a valid network address")
	}
	netMaskSize, _ := ipNet.Mask.Size()
	log.Println("netMaskSize", netMaskSize)

	totalSubnetsInNetwork := math.Pow(2, float64(subnetMaskSize)-float64(netMaskSize))
	log.Println("totalSubnetsInNetwork", totalSubnetsInNetwork)
	totalHostsInSubnet := math.Pow(2, 32-float64(subnetMaskSize))
	log.Println("totalHostsInSubnet", totalHostsInSubnet)
	subnetIntAddresses := make([]uint32, int(totalSubnetsInNetwork))
	// first subnet address is same as the network address
	subnetIntAddresses[0] = ip2int(ip.To4())
	for i := 1; i < int(totalSubnetsInNetwork); i++ {
		subnetIntAddresses[i] = subnetIntAddresses[i-1] + uint32(totalHostsInSubnet)
	}

	subnetCIDRs := make([]string, 0)
	for _, sia := range subnetIntAddresses {
		subnetCIDRs = append(
			subnetCIDRs,
			int2ip(sia).String()+"/"+strconv.Itoa(int(subnetMaskSize)),
		)
	}
	log.Println(subnetCIDRs)
}

func ip2int(ip net.IP) uint32 {
	if len(ip) == 16 {
		panic("cannot convert IPv6 into uint32")
	}
	return binary.BigEndian.Uint32(ip)
}

func int2ip(nn uint32) net.IP {
	ip := make(net.IP, 4)
	binary.BigEndian.PutUint32(ip, nn)
	return ip
}
