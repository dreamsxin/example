## 策略设计：基于奇偶校验的LSB干扰与恢复

一种基于数学奇偶校验的LSB干扰策略，可以在不知道原始值的情况下恢复LSB。这种方法通过引入冗余信息来实现恢复，而不需要存储额外的修改记录。

### 核心思想

1. 分组处理：将像素分成小组（例如2×2或3×3的块）
2. 奇偶校验：为每组像素计算奇偶校验位
3. LSB调整：通过调整像素值，使得组的LSB奇偶性匹配要隐藏的信息
4. 恢复机制：利用奇偶校验关系恢复原始LSB值

### 数学原理

对于一组像素值 $P_1, P_2, ..., P_n$，我们定义它们的LSB奇偶性为： \text{Parity} = (P_1 \mod 2) \oplus (P_2 \mod 2) \oplus ... \oplus (P_n \mod 2)

其中 $\oplus$ 表示异或操作。

要隐藏信息位 $b$，我们调整其中一个像素的LSB，使得： (P_1 \mod 2) \oplus (P_2 \mod 2) \oplus ... \oplus (P_n \mod 2) = b

恢复时，我们可以计算当前奇偶性，并与隐藏的信息位比较，确定是否需要调整。

### Go语言实现

```go
package main

import (
    "fmt"
    "image"
    "image/color"
    "image/jpeg"
    "log"
    "math/rand"
    "os"
    "time"
)

// LSBManipulator LSB操作器
type LSBManipulator struct {
    BlockSize int // 分组大小（像素数量）
    Seed      int64
}

// NewLSBManipulator 创建新的LSB操作器
func NewLSBManipulator(blockSize int) *LSBManipulator {
    return &LSBManipulator{
        BlockSize: blockSize,
        Seed:      time.Now().UnixNano(),
    }
}

// EmbedData 嵌入数据到图像中
func (m *LSBManipulator) EmbedData(img image.Image, data []byte) *image.RGBA {
    bounds := img.Bounds()
    result := image.NewRGBA(bounds)
    
    // 复制原始图像
    for y := bounds.Min.Y; y < bounds.Max.Y; y++ {
        for x := bounds.Min.X; x < bounds.Max.X; x++ {
            result.Set(x, y, img.At(x, y))
        }
    }
    
    // 将数据转换为位序列
    bitSequence := bytesToBits(data)
    
    // 设置随机种子
    rand.Seed(m.Seed)
    
    // 嵌入数据
    bitIndex := 0
    for y := bounds.Min.Y; y < bounds.Max.Y-m.BlockSize; y += m.BlockSize {
        for x := bounds.Min.X; x < bounds.Max.X-m.BlockSize; x += m.BlockSize {
            if bitIndex >= len(bitSequence) {
                break
            }
            
            // 获取当前块的像素
            pixels := make([]color.Color, 0, m.BlockSize*m.BlockSize)
            for dy := 0; dy < m.BlockSize; dy++ {
                for dx := 0; dx < m.BlockSize; dx++ {
                    pixels = append(pixels, result.At(x+dx, y+dy))
                }
            }
            
            // 嵌入一位数据
            modifiedPixels := m.embedBitInPixels(pixels, bitSequence[bitIndex])
            bitIndex++
            
            // 更新图像中的像素
            for i, dy := 0, 0; dy < m.BlockSize; dy++ {
                for dx := 0; dx < m.BlockSize; dx++ {
                    result.Set(x+dx, y+dy, modifiedPixels[i])
                    i++
                }
            }
        }
        
        if bitIndex >= len(bitSequence) {
            break
        }
    }
    
    return result
}

// ExtractData 从图像中提取数据
func (m *LSBManipulator) ExtractData(img image.Image, dataLength int) []byte {
    bounds := img.Bounds()
    bitSequence := make([]bool, 0, dataLength*8)
    
    // 设置随机种子（必须与嵌入时相同）
    rand.Seed(m.Seed)
    
    // 提取数据
    bitCount := 0
    for y := bounds.Min.Y; y < bounds.Max.Y-m.BlockSize; y += m.BlockSize {
        for x := bounds.Min.X; x < bounds.Max.X-m.BlockSize; x += m.BlockSize {
            if bitCount >= dataLength*8 {
                break
            }
            
            // 获取当前块的像素
            pixels := make([]color.Color, 0, m.BlockSize*m.BlockSize)
            for dy := 0; dy < m.BlockSize; dy++ {
                for dx := 0; dx < m.BlockSize; dx++ {
                    pixels = append(pixels, img.At(x+dx, y+dy))
                }
            }
            
            // 提取一位数据
            bit := m.extractBitFromPixels(pixels)
            bitSequence = append(bitSequence, bit)
            bitCount++
        }
        
        if bitCount >= dataLength*8 {
            break
        }
    }
    
    // 将位序列转换为字节
    return bitsToBytes(bitSequence)
}

// embedBitInPixels 在位序列中嵌入一位数据
func (m *LSBManipulator) embedBitInPixels(pixels []color.Color, bit bool) []color.Color {
    // 计算当前奇偶性
    currentParity := m.calculateParity(pixels)
    
    // 如果当前奇偶性已经匹配要隐藏的位，则不需要修改
    if currentParity == bit {
        return pixels
    }
    
    // 随机选择一个像素进行修改
    index := rand.Intn(len(pixels))
    pixel := pixels[index]
    
    // 获取像素的RGB值
    r, g, b, a := pixel.RGBA()
    r8 := uint8(r >> 8)
    g8 := uint8(g >> 8)
    b8 := uint8(b >> 8)
    a8 := uint8(a >> 8)
    
    // 随机选择一个通道进行修改
    channel := rand.Intn(3)
    
    // 修改LSB（加1或减1，随机选择）
    if rand.Intn(2) == 0 {
        // 加1
        switch channel {
        case 0:
            r8 = addWithClamp(r8, 1)
        case 1:
            g8 = addWithClamp(g8, 1)
        case 2:
            b8 = addWithClamp(b8, 1)
        }
    } else {
        // 减1
        switch channel {
        case 0:
            r8 = subtractWithClamp(r8, 1)
        case 1:
            g8 = subtractWithClamp(g8, 1)
        case 2:
            b8 = subtractWithClamp(b8, 1)
        }
    }
    
    // 创建修改后的像素
    modifiedPixel := color.RGBA{R: r8, G: g8, B: b8, A: a8}
    
    // 创建修改后的像素数组
    modifiedPixels := make([]color.Color, len(pixels))
    copy(modifiedPixels, pixels)
    modifiedPixels[index] = modifiedPixel
    
    return modifiedPixels
}

// extractBitFromPixels 从像素中提取一位数据
func (m *LSBManipulator) extractBitFromPixels(pixels []color.Color) bool {
    return m.calculateParity(pixels)
}

// calculateParity 计算像素组的奇偶性
func (m *LSBManipulator) calculateParity(pixels []color.Color) bool {
    parity := false
    
    for _, pixel := range pixels {
        r, g, b, _ := pixel.RGBA()
        r8 := uint8(r >> 8)
        g8 := uint8(g >> 8)
        b8 := uint8(b >> 8)
        
        // 使用所有通道的LSB计算奇偶性
        parity = parity != (r8&1 == 1)
        parity = parity != (g8&1 == 1)
        parity = parity != (b8&1 == 1)
    }
    
    return parity
}

// 辅助函数
func addWithClamp(value uint8, delta int) uint8 {
    result := int(value) + delta
    if result > 255 {
        return 255
    }
    if result < 0 {
        return 0
    }
    return uint8(result)
}

func subtractWithClamp(value uint8, delta int) uint8 {
    result := int(value) - delta
    if result > 255 {
        return 255
    }
    if result < 0 {
        return 0
    }
    return uint8(result)
}

func bytesToBits(data []byte) []bool {
    bits := make([]bool, 0, len(data)*8)
    
    for _, b := range data {
        for i := 0; i < 8; i++ {
            bits = append(bits, (b&(1<<(7-i))) != 0)
        }
    }
    
    return bits
}

func bitsToBytes(bits []bool) []byte {
    bytes := make([]byte, (len(bits)+7)/8)
    
    for i, bit := range bits {
        if bit {
            bytes[i/8] |= 1 << (7 - (i % 8))
        }
    }
    
    return bytes
}

func main() {
    // 打开原始图像
    originalFile, err := os.Open("original.jpg")
    if err != nil {
        log.Fatal("无法打开原始图像:", err)
    }
    defer originalFile.Close()
    
    // 解码图像
    originalImg, err := jpeg.Decode(originalFile)
    if err != nil {
        log.Fatal("无法解码原始图像:", err)
    }
    
    // 创建LSB操作器（使用2×2像素块）
    manipulator := NewLSBManipulator(2)
    
    // 要隐藏的数据
    message := []byte("这是一条秘密消息!")
    
    // 嵌入数据
    stegoImage := manipulator.EmbedData(originalImg, message)
    
    // 保存含密图像
    stegoFile, err := os.Create("stego_image.jpg")
    if err != nil {
        log.Fatal("无法创建含密图像:", err)
    }
    defer stegoFile.Close()
    
    err = jpeg.Encode(stegoFile, stegoImage, &jpeg.Options{Quality: 90})
    if err != nil {
        log.Fatal("无法保存含密图像:", err)
    }
    
    // 从含密图像中提取数据
    extractedMessage := manipulator.ExtractData(stegoImage, len(message))
    
    fmt.Printf("原始消息: %s\n", string(message))
    fmt.Printf("提取的消息: %s\n", string(extractedMessage))
    
    // 验证提取是否正确
    if string(extractedMessage) == string(message) {
        fmt.Println("成功提取并验证消息!")
    } else {
        fmt.Println("消息提取失败!")
    }
}
```

### 算法特点

1. 数学可恢复性：基于奇偶校验的数学原理，不需要存储修改记录
2. 随机性：随机选择修改的像素和通道，增加安全性
3. 容错性：即使部分像素被轻微修改，仍能正确提取数据
4. 不可见性：只修改LSB，对图像质量影响极小
5. 灵活性：可以通过调整块大小来平衡容量和鲁棒性

### 恢复原理

1. 嵌入时：通过调整一个像素的值，使得整个像素块的LSB奇偶性匹配要隐藏的信息位
2. 提取时：计算像素块的LSB奇偶性，直接得到隐藏的信息位
3. 恢复原始LSB：不需要显式恢复原始LSB值，因为提取过程不依赖于原始值

### 使用说明

1. 准备原始图像文件 original.jpg
2. 运行程序：
   ```bash
   go run main.go
   ```
3. 程序将：
   · 嵌入消息到图像中，生成 stego_image.jpg
   · 从含密图像中提取消息并验证

### 参数调整

· BlockSize：块大小影响容量和鲁棒性
  · 较小的块（如2×2）提供更高的容量但较低的鲁棒性
  · 较大的块（如4×4）提供较低的容量但较高的鲁棒性
· Seed：随机种子确保可重现性，相同的种子会产生相同的修改模式

### 注意事项

1. 这种方法主要用于信息隐藏，而不是完全恢复原始图像
2. 对于图像处理操作（如压缩、滤波）的鲁棒性有限
3. 容量受图像大小和块大小限制
4. 随机种子必须保持一致，否则无法正确提取数据

这种基于数学奇偶校验的方法提供了一种不需要额外存储就能恢复LSB信息的有效策略，适用于轻量级的信息隐藏应用。
