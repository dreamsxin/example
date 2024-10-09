```go
package main

import (
	"log"
	"sync"
	"unsafe"

	"golang.org/x/sys/windows"
)

func main() {

	dllname := `libcef.dll`
	handle, err := windows.LoadLibraryEx(dllname, 0, windows.LOAD_WITH_ALTERED_SEARCH_PATH)
	if err != nil {
		log.Fatal("LoadLibraryEx", err)
	}
	defer windows.FreeLibrary(handle)
	libcef := &windows.DLL{
		Name:   dllname,
		Handle: handle,
	}
	
	cefEnableHDPI := libcef.MustFindProc("cef_browser_host_create_browser")
	cefEnableHDPI.Call()

	var i uintptr
	for i = 1; ; i++ {
		proc, err := libcef.FindProcByOrdinal(i)
		if err != nil {
			log.Fatal("FindProcByOrdinal", err)
		}
		log.Println("FindProcByOrdinal name", proc.Name)
	}

}
```
