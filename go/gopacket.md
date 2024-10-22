#

安装必需的库。你需要go，libpcap和gopacket包。因为gopacket是在libpcap之上构建的，所以我强烈建议您了解该libpcap库是如何工作的。你可以学习如何在C语言中使用libpcap来加深理解。这些示例应该在使用libpcap的Linux/Mac和使用winpcap的Windows上工作。
如果出现错误（如cc1.exe: sorry, unimplemented: 64-bit mode not compiled in），可能需要设置 `GOARCH=386`。

```shell
# Get the gopacket package from GitHub
go get github.com/google/gopacket
# Pcap dev headers might be necessary
sudo apt-get install libpcap-dev
```

## 查找设备
```go
package main
import (
    "fmt"
    "log"
    "github.com/google/gopacket/pcap"
)
func main() {
    // 得到所有的(网络)设备
    devices, err := pcap.FindAllDevs()
    if err != nil {
        log.Fatal(err)
    }
    // 打印设备信息
    fmt.Println("Devices found:")
    for _, device := range devices {
        fmt.Println("\nName: ", device.Name)
        fmt.Println("Description: ", device.Description)
        fmt.Println("Devices addresses: ", device.Description)
        for _, address := range device.Addresses {
            fmt.Println("- IP address: ", address.IP)
            fmt.Println("- Subnet mask: ", address.Netmask)
        }
    }
}
```

## 打开一个设备进行实时捕获
```go
package main
import (
    "fmt"
    "github.com/google/gopacket"
    "github.com/google/gopacket/pcap"
    "log"
    "time"
)
var (
    device       string = "eth0"
    snapshot_len int32  = 1024
    promiscuous  bool   = false
    err          error
    timeout      time.Duration = 30 * time.Second
    handle       *pcap.Handle
)
func main() {
    // Open device
    handle, err = pcap.OpenLive(device, snapshot_len, promiscuous, timeout)
    if err != nil {log.Fatal(err) }
    defer handle.Close()
    // Use the handle as a packet source to process all packets
    packetSource := gopacket.NewPacketSource(handle, handle.LinkType())
    for packet := range packetSource.Packets() {
        // Process packet here
        fmt.Println(packet)
    }
}
```

## 保存到 pcap 文件
```go
package main
import (
	"fmt"
	"os"
	"time"
	"github.com/google/gopacket"
	"github.com/google/gopacket/layers"
	"github.com/google/gopacket/pcap"
	"github.com/google/gopacket/pcapgo"
)
var (
	deviceName  string = "eth0"
	snapshotLen int32  = 1024
	promiscuous bool   = false
	err         error
	timeout     time.Duration = -1 * time.Second
	handle      *pcap.Handle
	packetCount int = 0
)
func main() {
	// Open output pcap file and write header 
	f, _ := os.Create("test.pcap")
	w := pcapgo.NewWriter(f)
	w.WriteFileHeader(snapshotLen, layers.LinkTypeEthernet)
	defer f.Close()
	// Open the device for capturing
	handle, err = pcap.OpenLive(deviceName, snapshotLen, promiscuous, timeout)
	if err != nil {
		fmt.Printf("Error opening device %s: %v", deviceName, err)
		os.Exit(1)
	}
	defer handle.Close()
	// Start processing packets
	packetSource := gopacket.NewPacketSource(handle, handle.LinkType())
	for packet := range packetSource.Packets() {
		// Process packet here
		fmt.Println(packet)
		w.WritePacket(packet.Metadata().CaptureInfo, packet.Data())
		packetCount++
		
		// Only capture 100 and then stop
		if packetCount > 100 {
			break
		}
	}
}
```

## 分析 pcap 文件
```shell
# Capture packets to test.pcap file
sudo tcpdump -w test.pcap
```

```go
package main
// Use tcpdump to create a test file
// tcpdump -w test.pcap
// or use the example above for writing pcap files
import (
    "fmt"
    "github.com/google/gopacket"
    "github.com/google/gopacket/pcap"
    "log"
)
var (
    pcapFile string = "test.pcap"
    handle   *pcap.Handle
    err      error
)
func main() {
    // Open file instead of device
    handle, err = pcap.OpenOffline(pcapFile)
    if err != nil { log.Fatal(err) }
    defer handle.Close()
    // Loop through packets in file
    packetSource := gopacket.NewPacketSource(handle, handle.LinkType())
    for packet := range packetSource.Packets() {
        fmt.Println(packet)
    }
}
```

## 设置过滤器
```go
package main
import (
    "fmt"
    "github.com/google/gopacket"
    "github.com/google/gopacket/pcap"
    "log"
    "time"
)
var (
    device       string = "eth0"
    snapshot_len int32  = 1024
    promiscuous  bool   = false
    err          error
    timeout      time.Duration = 30 * time.Second
    handle       *pcap.Handle
)
func main() {
    // Open device
    handle, err = pcap.OpenLive(device, snapshot_len, promiscuous, timeout)
    if err != nil {
        log.Fatal(err)
    }
    defer handle.Close()
    // Set filter
    var filter string = "tcp and port 80"
    err = handle.SetBPFFilter(filter)
    if err != nil {
        log.Fatal(err)
    }
    fmt.Println("Only capturing TCP port 80 packets.")
    packetSource := gopacket.NewPacketSource(handle, handle.LinkType())
    for packet := range packetSource.Packets() {
        // Do something with a packet here.
        fmt.Println(packet)
    }
}
```

## 解码 packet 的各层
```go
package main
import (
    "fmt"
    "github.com/google/gopacket"
    "github.com/google/gopacket/layers"
    "github.com/google/gopacket/pcap"
    "log"
    "strings"
    "time"
)
var (
    device      string = "eth0"
    snapshotLen int32  = 1024
    promiscuous bool   = false
    err         error
    timeout     time.Duration = 30 * time.Second
    handle      *pcap.Handle
)
func main() {
    // Open device
    handle, err = pcap.OpenLive(device, snapshotLen, promiscuous, timeout)
    if err != nil {log.Fatal(err) }
    defer handle.Close()
    packetSource := gopacket.NewPacketSource(handle, handle.LinkType())
    for packet := range packetSource.Packets() {
        printPacketInfo(packet)
    }
}
func printPacketInfo(packet gopacket.Packet) {
    // Let's see if the packet is an ethernet packet
    ethernetLayer := packet.Layer(layers.LayerTypeEthernet)
    if ethernetLayer != nil {
        fmt.Println("Ethernet layer detected.")
        ethernetPacket, _ := ethernetLayer.(*layers.Ethernet)
        fmt.Println("Source MAC: ", ethernetPacket.SrcMAC)
        fmt.Println("Destination MAC: ", ethernetPacket.DstMAC)
        // Ethernet type is typically IPv4 but could be ARP or other
        fmt.Println("Ethernet type: ", ethernetPacket.EthernetType)
        fmt.Println()
    }
    // Let's see if the packet is IP (even though the ether type told us)
    ipLayer := packet.Layer(layers.LayerTypeIPv4)
    if ipLayer != nil {
        fmt.Println("IPv4 layer detected.")
        ip, _ := ipLayer.(*layers.IPv4)
        // IP layer variables:
        // Version (Either 4 or 6)
        // IHL (IP Header Length in 32-bit words)
        // TOS, Length, Id, Flags, FragOffset, TTL, Protocol (TCP?),
        // Checksum, SrcIP, DstIP
        fmt.Printf("From %s to %s\n", ip.SrcIP, ip.DstIP)
        fmt.Println("Protocol: ", ip.Protocol)
        fmt.Println()
    }
    // Let's see if the packet is TCP
    tcpLayer := packet.Layer(layers.LayerTypeTCP)
    if tcpLayer != nil {
        fmt.Println("TCP layer detected.")
        tcp, _ := tcpLayer.(*layers.TCP)
        // TCP layer variables:
        // SrcPort, DstPort, Seq, Ack, DataOffset, Window, Checksum, Urgent
        // Bool flags: FIN, SYN, RST, PSH, ACK, URG, ECE, CWR, NS
        fmt.Printf("From port %d to %d\n", tcp.SrcPort, tcp.DstPort)
        fmt.Println("Sequence number: ", tcp.Seq)
        fmt.Println()
    }
    // Iterate over all layers, printing out each layer type
    fmt.Println("All packet layers:")
    for _, layer := range packet.Layers() {
        fmt.Println("- ", layer.LayerType())
    }
    // When iterating through packet.Layers() above,
    // if it lists Payload layer then that is the same as
    // this applicationLayer. applicationLayer contains the payload
    applicationLayer := packet.ApplicationLayer()
    if applicationLayer != nil {
        fmt.Println("Application layer/Payload found.")
        fmt.Printf("%s\n", applicationLayer.Payload())
        // Search for a string inside the payload
        if strings.Contains(string(applicationLayer.Payload()), "HTTP") {
            fmt.Println("HTTP found!")
        }
    }
    // Check for errors
    if err := packet.ErrorLayer(); err != nil {
        fmt.Println("Error decoding some part of the packet:", err)
    }
}
```

## 创建和发送packet
```go
package main
import (
    "github.com/google/gopacket"
    "github.com/google/gopacket/layers"
    "github.com/google/gopacket/pcap"
    "log"
    "net"
    "time"
)
var (
    device       string = "eth0"
    snapshot_len int32  = 1024
    promiscuous  bool   = false
    err          error
    timeout      time.Duration = 30 * time.Second
    handle       *pcap.Handle
    buffer       gopacket.SerializeBuffer
    options      gopacket.SerializeOptions
)
func main() {
    // Open device
    handle, err = pcap.OpenLive(device, snapshot_len, promiscuous, timeout)
    if err != nil {log.Fatal(err) }
    defer handle.Close()
    // Send raw bytes over wire
    rawBytes := []byte{10, 20, 30}
    err = handle.WritePacketData(rawBytes)
    if err != nil {
        log.Fatal(err)
    }
    // Create a properly formed packet, just with
    // empty details. Should fill out MAC addresses,
    // IP addresses, etc.
    buffer = gopacket.NewSerializeBuffer()
    gopacket.SerializeLayers(buffer, options,
        &layers.Ethernet{},
        &layers.IPv4{},
        &layers.TCP{},
        gopacket.Payload(rawBytes),
    )
    outgoingPacket := buffer.Bytes()
    // Send our packet
    err = handle.WritePacketData(outgoingPacket)
    if err != nil {
        log.Fatal(err)
    }
    // This time lets fill out some information
    ipLayer := &layers.IPv4{
        SrcIP: net.IP{127, 0, 0, 1},
        DstIP: net.IP{8, 8, 8, 8},
    }
    ethernetLayer := &layers.Ethernet{
        SrcMAC: net.HardwareAddr{0xFF, 0xAA, 0xFA, 0xAA, 0xFF, 0xAA},
        DstMAC: net.HardwareAddr{0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD},
    }
    tcpLayer := &layers.TCP{
        SrcPort: layers.TCPPort(4321),
        DstPort: layers.TCPPort(80),
    }
    // And create the packet with the layers
    buffer = gopacket.NewSerializeBuffer()
    gopacket.SerializeLayers(buffer, options,
        ethernetLayer,
        ipLayer,
        tcpLayer,
        gopacket.Payload(rawBytes),
    )
    outgoingPacket = buffer.Bytes()
}
```

## 更多创建和解码packet的例子
```go
package main
import (
    "fmt"
    "github.com/google/gopacket"
    "github.com/google/gopacket/layers"
)
func main() {
    // If we don't have a handle to a device or a file, but we have a bunch
    // of raw bytes, we can try to decode them in to packet information
    // NewPacket() takes the raw bytes that make up the packet as the first parameter
    // The second parameter is the lowest level layer you want to decode. It will
    // decode that layer and all layers on top of it. The third layer
    // is the type of decoding: default(all at once), lazy(on demand), and NoCopy
    // which will not create a copy of the buffer
    // Create an packet with ethernet, IP, TCP, and payload layers
    // We are creating one we know will be decoded properly but
    // your byte source could be anything. If any of the packets
    // come back as nil, that means it could not decode it in to
    // the proper layer (malformed or incorrect packet type)
    payload := []byte{2, 4, 6}
    options := gopacket.SerializeOptions{}
    buffer := gopacket.NewSerializeBuffer()
    gopacket.SerializeLayers(buffer, options,
        &layers.Ethernet{},
        &layers.IPv4{},
        &layers.TCP{},
        gopacket.Payload(payload),
    )
    rawBytes := buffer.Bytes()
    // Decode an ethernet packet
    ethPacket :=
        gopacket.NewPacket(
            rawBytes,
            layers.LayerTypeEthernet,
            gopacket.Default,
        )
    // with Lazy decoding it will only decode what it needs when it needs it
    // This is not concurrency safe. If using concurrency, use default
    ipPacket :=
        gopacket.NewPacket(
            rawBytes,
            layers.LayerTypeIPv4,
            gopacket.Lazy,
        )
    // With the NoCopy option, the underlying slices are referenced
    // directly and not copied. If the underlying bytes change so will
    // the packet
    tcpPacket :=
        gopacket.NewPacket(
            rawBytes,
            layers.LayerTypeTCP,
            gopacket.NoCopy,
        )
    fmt.Println(ethPacket)
    fmt.Println(ipPacket)
    fmt.Println(tcpPacket)
}
```

## 定制层
创建您自己的层。这有助于实现当前不包含在gopacket layers包中的协议。如果您想创建自己的l33t协议，甚至不使用TCP/IP或ethernet，那么它也很有用。
```go
package main
import (
    "fmt"
    "github.com/google/gopacket"
)
// Create custom layer structure
type CustomLayer struct {
    // This layer just has two bytes at the front
    SomeByte    byte
    AnotherByte byte
    restOfData  []byte
}
// Register the layer type so we can use it
// The first argument is an ID. Use negative
// or 2000+ for custom layers. It must be unique
var CustomLayerType = gopacket.RegisterLayerType(
    2001,
    gopacket.LayerTypeMetadata{
        "CustomLayerType",
        gopacket.DecodeFunc(decodeCustomLayer),
    },
)
// When we inquire about the type, what type of layer should
// we say it is? We want it to return our custom layer type
func (l CustomLayer) LayerType() gopacket.LayerType {
    return CustomLayerType
}
// LayerContents returns the information that our layer
// provides. In this case it is a header layer so
// we return the header information
func (l CustomLayer) LayerContents() []byte {
    return []byte{l.SomeByte, l.AnotherByte}
}
// LayerPayload returns the subsequent layer built
// on top of our layer or raw payload
func (l CustomLayer) LayerPayload() []byte {
    return l.restOfData
}
// Custom decode function. We can name it whatever we want
// but it should have the same arguments and return value
// When the layer is registered we tell it to use this decode function
func decodeCustomLayer(data []byte, p gopacket.PacketBuilder) error {
    // AddLayer appends to the list of layers that the packet has
    p.AddLayer(&CustomLayer{data[0], data[1], data[2:]})
    // The return value tells the packet what layer to expect
    // with the rest of the data. It could be another header layer,
    // nothing, or a payload layer.
    // nil means this is the last layer. No more decoding
    // return nil
    // Returning another layer type tells it to decode
    // the next layer with that layer's decoder function
    // return p.NextDecoder(layers.LayerTypeEthernet)
    // Returning payload type means the rest of the data
    // is raw payload. It will set the application layer
    // contents with the payload
    return p.NextDecoder(gopacket.LayerTypePayload)
}
func main() {
    // If you create your own encoding and decoding you can essentially
    // create your own protocol or implement a protocol that is not
    // already defined in the layers package. In our example we are just
    // wrapping a normal ethernet packet with our own layer.
    // Creating your own protocol is good if you want to create
    // some obfuscated binary data type that was difficult for others
    // to decode
    // Finally, decode your packets:
    rawBytes := []byte{0xF0, 0x0F, 65, 65, 66, 67, 68}
    packet := gopacket.NewPacket(
        rawBytes,
        CustomLayerType,
        gopacket.Default,
    )
    fmt.Println("Created packet out of raw bytes.")
    fmt.Println(packet)
    // Decode the packet as our custom layer
    customLayer := packet.Layer(CustomLayerType)
    if customLayer != nil {
        fmt.Println("Packet was successfully decoded with custom layer decoder.")
        customLayerContent, _ := customLayer.(*CustomLayer)
        // Now we can access the elements of the custom struct
        fmt.Println("Payload: ", customLayerContent.LayerPayload())
        fmt.Println("SomeByte element:", customLayerContent.SomeByte)
        fmt.Println("AnotherByte element:", customLayerContent.AnotherByte)
    }
}
```

## 更快的解码packet
使用DecodingLayerParser可以更快一点。这就像marshalling/unmarshalling数据一样。
```go
package main
import (
    "fmt"
    "github.com/google/gopacket"
    "github.com/google/gopacket/layers"
    "github.com/google/gopacket/pcap"
    "log"
    "time"
)
var (
    device       string = "eth0"
    snapshot_len int32  = 1024
    promiscuous  bool   = false
    err          error
    timeout      time.Duration = 30 * time.Second
    handle       *pcap.Handle
    // Will reuse these for each packet
    ethLayer layers.Ethernet
    ipLayer  layers.IPv4
    tcpLayer layers.TCP
)
func main() {
    // Open device
    handle, err = pcap.OpenLive(device, snapshot_len, promiscuous, timeout)
    if err != nil {
        log.Fatal(err)
    }
    defer handle.Close()
    packetSource := gopacket.NewPacketSource(handle, handle.LinkType())
    for packet := range packetSource.Packets() {
        parser := gopacket.NewDecodingLayerParser(
            layers.LayerTypeEthernet,
            ðLayer,
            &ipLayer,
            &tcpLayer,
        )
        foundLayerTypes := []gopacket.LayerType{}
        err := parser.DecodeLayers(packet.Data(), &foundLayerTypes)
        if err != nil {
            fmt.Println("Trouble decoding layers: ", err)
        }
        for _, layerType := range foundLayerTypes {
            if layerType == layers.LayerTypeIPv4 {
                fmt.Println("IPv4: ", ipLayer.SrcIP, "->", ipLayer.DstIP)
            }
            if layerType == layers.LayerTypeTCP {
                fmt.Println("TCP Port: ", tcpLayer.SrcPort, "->", tcpLayer.DstPort)
                fmt.Println("TCP SYN:", tcpLayer.SYN, " | ACK:", tcpLayer.ACK)
            }
        }
    }
}
```

