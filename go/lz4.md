https://github.com/pierrec/lz4

## 使用
```go
package main

import (
	"io"
	"log"
	"os"

	"github.com/pierrec/lz4/v4"
)

func main() {
	source := "test.lz4"
	log.Println("source", source)
	file, err := os.Open(source)
	if err != nil {
		log.Fatalf("source", source, err)
	}
	defer file.Close()
	zr := lz4.NewReader(file)

	_, err = io.Copy(os.Stdout, zr)

	log.Println("output", err)
}
```

## mozlz4
mozilla 中增加了自定义头。
```go
package main

import (
	"bytes"
	"encoding/binary"
	"errors"
	"fmt"

	"github.com/pierrec/lz4/v4"
)

var HEADER = []byte("mozLz40\x00")

var ErrInvalid = errors.New("not a mozlz4 file")

func Compress(data []byte) ([]byte, error) {
	buf := make([]byte, 0, len(HEADER)+4+lz4.CompressBlockBound(len(data)))

	buf = append(buf, HEADER...)

	buf, err := binary.Append(buf, binary.LittleEndian, uint32(len(data)))
	if err != nil {
		return nil, err
	}

	if len(data) != 0 {
		var c lz4.Compressor
		n, err := c.CompressBlock(data, buf[len(buf):cap(buf)])
		if err != nil {
			return nil, err
		}
		buf = buf[:len(buf)+n]
	}

	return buf, nil
}

func Decompress(data []byte) ([]byte, error) {
	data, found := bytes.CutPrefix(data, HEADER)
	if !found {
		return nil, ErrInvalid
	}

	var size uint32
	if n, err := binary.Decode(data, binary.LittleEndian, &size); err != nil {
		return nil, ErrInvalid
	} else {
		data = data[n:]
	}

	if size == 0 {
		return []byte{}, nil
	}

	buf := make([]byte, size)
	if n, err := lz4.UncompressBlock(data, buf); err != nil {
		return nil, err
	} else if uint32(n) != size {
		return nil, fmt.Errorf("uncompressed size mismatch: expected %v, but got %v", size, n)
	}

	return buf, nil
}

func main() {
	source := "addonStartup.json.lz4"
	data, _ := os.ReadFile(source)
	if data == nil {
		log.Fatal("no data")
	}
	result, err := Decompress(data)
	log.Println("output", string(result), err)
}
```
