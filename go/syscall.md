## 调用 DLL 函数注意事项

### 指针传递问题

针对32位和64位dll，指针类型要转换位对应的类型。

```go
#32位
ptr := *((*int32)(unsafe.Pointer(&byte)))
#64位
ptr := *((*int64)(unsafe.Pointer(&byte)))
#字符串指针
keystr := []byte{49,50,51,0}
str := *((*int32)(unsafe.Pointer(&keystr)))
```

### 返回值字符串的指针
```go
//根据DLL返回的指针，逐个取出字节，到0字节时判断为字符串结尾，返回字节数组转成的字符串
func prttostr(vcode uintptr) string {
   var vbyte []byte
   for i:=0;i<10;i++{
      sbyte:=*((*byte)(unsafe.Pointer(vcode)))
      if sbyte==0{
         break
      }
      vbyte=append(vbyte,sbyte)
      vcode += 1
   }
   return string(vbyte)
}
```

```shell
g++ -shared main.cpp -o test.dll
```
命令
`go build -buildmode=c-shared -o exportgo.dll main.go`
导出的 DLL文件会导出 Go 语言所有 package 中的 public 函数，这通常是不合适的。如有必要，我们可以利用DEF文件导出指定的函数。

首先生成main.a静态链接库：

`go build -buildmode=c-archive main.go`
然后编写DEF文件：

```
EXPORTS
    PrintBye
    Sum
```
DEF文件以EXPORTS开头，第二行开始每行一个导出函数名称，与Go源文件中带有//export注释的导出函数相对应。
然后用GCC生成DLL文件：
```shell
gcc test.def main.a -shared -lwinmm -lWs2_32 -o test.dll -Wl,--out-implib,test.lib
```

执行成功后会在当前目录生成test.dll和test.lib文件。此时，再次使用dumpbin查看，会发现DLL文件仅导出了指定的函数
```go
package main

import (
	"fmt"
	"log"
	"os"
	"path/filepath"
	"syscall"
	"unsafe"
)

func main() {
	log.SetFlags(log.Llongfile | log.Lmicroseconds | log.Ldate)
	dllPath := "sunday_x64.dll"
	libPath := "OCR.lib"

	libPath, err := filepath.Abs(libPath)
	tmp1 := []byte(libPath)
	libPathPtr := *((*int64)(unsafe.Pointer(&tmp1)))

	if err != nil {
		log.Fatal(err)
	}
	log.Println(libPath)

	dll, err := syscall.LoadDLL(dllPath)
	if err != nil {
		log.Println("LoadDLL err", err)
		return
	}

	proc, err := dll.FindProc("LoadLibFromFile")
	if err != nil {
		log.Println("FindProc err", err)
		return
	}
	log.Printf("FindProc:%#v\n", proc)

	//ret, tmp, err := proc.Call(uintptr(unsafe.Pointer(&libPath)))
	//if err != nil {
	//	log.Println("LoadLibFromFile1 err", ret, tmp, err)
	//	return
	//}

	pwd := []byte("123")
	pwdPtr := *((*int64)(unsafe.Pointer(&pwd)))
	ret, tmp, err := proc.Call(uintptr(libPathPtr), uintptr(pwdPtr)) //unsafe.Pointer(&pwd)
	if err != nil && err.Error() != "The operation completed successfully." {
		log.Println("LoadLibFromFile err", ret, tmp, err)
		return
	}
	fmt.Println("Call", ret, tmp, err)

	b, _ := os.ReadFile("1.jpg")
	bPtr := *((*int64)(unsafe.Pointer(&b)))

	code := make([]byte, 8)
	codePtr := *((*int64)(unsafe.Pointer(&code)))

	proc, err = dll.FindProc("GetCodeFromBuffer")
	fmt.Println(err)

	ret2, _, _ := proc.Call(ret, uintptr(bPtr), uintptr(len(b)), uintptr(codePtr))
	fmt.Println(ret2, string(code))
}

func prt2Str(vCode uintptr) string {
	if vCode <= 0 {
		return ""
	}
	var vBytes []byte
	for {
		sByte := *((*byte)(unsafe.Pointer(vCode)))
		if sByte == 0 {
			break
		}
		vBytes = append(vBytes, sByte)
		vCode += 1
	}
	return string(vBytes)
}
```
