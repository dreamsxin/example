package main
 
import (
    "fmt"
    "unsafe"
)
 
func main() {
    bytes := []byte("I am byte array !")
    str := string(bytes)
    bytes[0] = 'i' //注意这一行，bytes在这里修改了数据，但是str打印出来的依然没变化，
    fmt.Println(str) // I am byte array !

    bytes2 := []byte("I am byte array !")
    str2 := (*string)(unsafe.Pointer(&bytes2))
    bytes2[0] = 'i'
    fmt.Println(*str2) // i am byte array !
}
