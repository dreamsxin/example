package main

import (
	"archive/zip"
	"io"
	"log"
	"os"
	"path/filepath"
	"strings"
)

func main() {
	// 源档案（准备压缩的文件或目录）
	var src = "./env/"
	// 目标文件，压缩后的文件
	var dst = "env@yunlogin.com.xpi"

	if err := Zip(dst, src); err != nil {
		log.Fatalln(err)
	}
}

func Zip(dst, src string) (err error) {
	// 创建准备写入的文件
	fw, err := os.Create(dst)
	if err != nil {
		return err
	}
	defer fw.Close()

	// 通过 fw 来创建 zip.Write
	zw := zip.NewWriter(fw)
	defer func() {
		// 检测一下是否成功关闭
		if err := zw.Close(); err != nil {
			log.Fatalln(err)
		}
	}()

	// 下面来将文件写入 zw ，因为有可能会有很多个目录及文件，所以递归处理
	return filepath.Walk(src, func(fpath string, fi os.FileInfo, errBack error) (err error) {
		if errBack != nil {
			return errBack
		}

		// 通过文件信息，创建 zip 的文件信息
		fh, err := zip.FileInfoHeader(fi)
		if err != nil {
			return
		}

		// 替换文件信息中的文件名
		fh.Name = strings.TrimPrefix(fpath, filepath.Dir(src))
		fh.Name = strings.TrimPrefix(fh.Name, string(filepath.Separator))
		log.Printf("fpath %s fh.Name %s\n", fpath, fh.Name)

		// 这步开始没有加，会发现解压的时候说它不是个目录
		if fi.IsDir() {
			fh.Name += "/"
		}

		// 写入文件信息，并返回一个 Write 结构
		w, err := zw.CreateHeader(fh)
		if err != nil {
			return
		}

		// 检测，如果不是标准文件就只写入头信息，不写入文件数据到 w
		// 如目录，也没有数据需要写
		if !fh.Mode().IsRegular() {
			return nil
		}

		// 打开要压缩的文件
		fr, err := os.Open(fpath)
		if err != nil {
			return
		}
		defer fr.Close()

		// 将打开的文件 Copy 到 w
		n, err := io.Copy(w, fr)
		if err != nil {
			return
		}
		// 输出压缩的内容
		log.Printf("成功压缩文件： %s, 共写入了 %d 个字符的数据\n", fpath, n)

		return nil
	})
}
