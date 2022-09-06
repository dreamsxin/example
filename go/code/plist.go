package main

import (
    "fmt"
    "os"
    "strings"
    "bytes"
    "io/ioutil"
    "path"
    "encoding/base64"
    "encoding/xml"
    "encoding/hex"
    "compress/gzip"
)

type PList struct {
    Dict Dict `xml:"dict"`
}
type Dict struct{
	Key    string `xml:"key"`
	Value string `xml:"string"`
}

//gzip解密
func GzipDecode(in []byte) ([]byte, error) {
    reader, err := gzip.NewReader(bytes.NewReader(in))
    if err != nil {
         var out []byte
        return out, err
    }
    defer reader.Close()
    return ioutil.ReadAll(reader)
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
                        //fmt.Println(plist.Dict.Value)
                        fileBytes, err := base64.StdEncoding.DecodeString(plist.Dict.Value)
                        if err != nil {
                            fmt.Println(v.Name() + "base64", err.Error())
                            continue
                        }

                        if len(fileBytes) < 16 {
                            continue
                        }
                        fmt.Println(hex.EncodeToString(fileBytes))

                        //fileBytes = fileBytes[15:]
                        //fmt.Println(string(fileBytes))
                        //fmt.Println(hex.EncodeToString(fileBytes[0:16]))

                        //判断压缩算法
                        //gzip [1F 8B 0B] [H4sl]
                        //lzma xz [FD 37 7A] [/Td6]
                        //lzma alone [5D 00 00] [XQAA]
                        //zlib alone [78 9C] [eJwr]
                        fileBytes, err = GzipDecode(fileBytes)
                        if err != nil {
                            fmt.Println(v.Name() + "GzipDecode", err.Error())
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
