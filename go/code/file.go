package main

import (
    "fmt"
    "io/ioutil"
    "os"
    "path"
    "strings"
    "encoding/json"
)

func WalkDir(filePath string) (list []string) {

    files, err := ioutil.ReadDir(filePath)
    if err != nil {
        fmt.Println(err.Error())
    } else {
        for _, v := range files {
            if v.IsDir() {
                //WalkDir(v.Name())
            } else {
                //fmt.Println(v.Name())
                //获取文件名称带后缀
                fileNameWithSuffix:=path.Base(v.Name())
                //获取文件的后缀(文件类型)
                fileType:=path.Ext(fileNameWithSuffix)
                //获取文件名称(不带后缀)
                fileNameOnly := strings.TrimSuffix(fileNameWithSuffix, fileType)
                //fmt.Println(fileNameOnly)
                list = append(list, fileNameOnly)
            }
        }
    }
    return
}
func main() {
    PthSep := string(os.PathSeparator)
    fmt.Println(PthSep)
    list := WalkDir(`D:\workspace\src\Poker-Admin\payui\public\data`)
    js,_ := json.Marshal(list)
    fmt.Println(string(js))
    jsIndent,_ := json.MarshalIndent(list, "", "\t")
    fmt.Println(string(jsIndent))
}
