package main
 
import (
    "fmt"
    "os"
    "io"
    "io/ioutil"
    "path"
    "strings"
    "log"
    "crypto/md5"
    "encoding/hex"
)
 
func main()  {

    filepath := "E:\\"
 
    files, err := ioutil.ReadDir(filepath)
    if err != nil {
        log.Fatal(err)
    }
 
    for _, f := range files {
        fmt.Println(f.Name())
        if f.IsDir() {
            fmt.Println(f.Name(), " is dir")
            continue
        }
        name := f.Name()
        ext := path.Ext(name)

        file, err := os.Open(filepath + f.Name())
        if err != nil {
            fmt.Println(f.Name(), "open error")
            continue
        }
        md5 := md5.New()
        io.Copy(md5, file)
        file.Close()
        md5str := hex.EncodeToString(md5.Sum(nil))
        fmt.Println(filepath + md5str + ext)
        err = os.Rename(filepath + f.Name(), filepath + md5str + "." + ext)
        if err != nil {
            fmt.Println(f.Name(), err.Error())
            continue
        }
    }


    filepath = "E:\\"
    list := ""

    files2 := strings.Split(list, " | ")
    for _, name := range files2 {
        filename := filepath +"\\" + name
        log.Println("-----", filename)
        err := os.Remove(filename)
        if err != nil {
            log.Println(err.Error())
        }
    }
}
