package pb

import (
    "encoding/binary"
)

func GetMsgTypeFromRaw(b []byte) (msgType int32) {
	if len(b) < 4 {
		return
	}
	return bytes2code(b[0:4])
}

func GetDataFromRaw(b []byte) []byte {
	if len(b) <= 4 {
		return nil
	}
	return b[4:]
}
func ComposeRaw(code uint32, b []byte) []byte {
	return append(code2bytes(code), b...)
}

func code2bytes(i uint32) (b []byte) {
	b = append(b, byte(i>>24), byte(i>>16), byte(i>>8), byte(i))
	return
}

/**
 * 按照大端存数据
 * 大端法在内存中按字节依次存放为：12 34 56 78
 * 小端法在内存中按字节依次存放为：78 56 34 12
 */
func bytes2code(b []byte) (i uint32) {
	i = uint32(b[0])<<24 | uint32(b[1])<<16 | uint32(b[2])<<8 | uint32(b[3])
	return
}
