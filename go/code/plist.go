package main

import (
    "fmt"
    "os"
    "strings"
    "io/ioutil"
    "path"
    "encoding/base64"
    "encoding/xml"
    "encoding/hex"
)

type PList struct {
    Dict Dict `xml:"dict"`
}
type Dict struct{
	Key    string `xml:"key"`
	Value string `xml:"string"`
}

func main() {
    filepath := `./`
    files, err := ioutil.ReadDir(filepath)
    if err != nil {
        fmt.Println(err.Error())
    } else {
        for _, v := range files {
            if !v.IsDir() {
                //获取文件名称带后缀
                fileNameWithSuffix:=path.Base(v.Name())
                //获取文件的后缀(文件类型)
                fileType := path.Ext(fileNameWithSuffix)
                if fileType == ".plist" {
                    fmt.Println(v.Name())
                    //读取文件内容
                    file, err := os.Open(filepath + v.Name())
                    if err != nil {
                        fmt.Println(v.Name(), err.Error())
                        continue
                    }
                    defer file.Close()
                    content, err := ioutil.ReadAll(file)
     
                    plist := PList{}
                    err = xml.Unmarshal(content, &plist)
                    if err != nil {
                        fmt.Println(v.Name() + "xml", err.Error())
                        continue
                    }

                    if plist.Dict.Value != "" {
                        fileBytes, err := base64.StdEncoding.DecodeString(plist.Dict.Value)
                        fileBytes = fileBytes[15:]
                        //fmt.Println(string(fileBytes))
                        fmt.Println(hex.EncodeToString(fileBytes[0:16]))

                        if err != nil {
                            fmt.Println(v.Name() + "base64", err.Error())
                            continue
                        }

                        //获取文件名称(不带后缀)
                        fileNameOnly := strings.TrimSuffix(fileNameWithSuffix, fileType)

                        if err = ioutil.WriteFile(filepath + fileNameOnly+".png", fileBytes, 0755); err != nil {
                            fmt.Println(v.Name() + "save error", err.Error())
                            continue
                        }
                    }
                }
            }
        }
    }
}
