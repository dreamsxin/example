```go
package main

import (
	"fmt"
	"io"
	"net/http"
	"os"
)

const uploadPath = "./upload"

func handleUploadFile(w http.ResponseWriter, r *http.Request) {
	r.ParseMultipartForm(100)
	mForm := r.MultipartForm

	for k, _ := range mForm.File {
		// k is the key of file part
		file, fileHeader, err := r.FormFile(k)
		if err != nil {
			fmt.Println("inovke FormFile error:", err)
			return
		}
		defer file.Close()
		fmt.Printf("the uploaded file: name[%s], size[%d], header[%#v]\n",
		fileHeader.Filename, fileHeader.Size, fileHeader.Header)

		// store uploaded file into local path
		localFileName := uploadPath + "/" + fileHeader.Filename
		out, err := os.Create(localFileName)
		if err != nil {
			fmt.Printf("failed to open the file %s for writing", localFileName)
			return
		}
		defer out.Close()
		_, err = io.Copy(out, file)
		if err != nil {
			fmt.Printf("copy file err:%s\n", err)
			return
		}
		fmt.Printf("file %s uploaded ok\n", fileHeader.Filename)
	}
}

func main() {
	http.HandleFunc("/upload", handleUploadFile)
	http.ListenAndServe(":8080", nil)
}
```
Gin
```go
func upload(c *gin.Context) {
	// Multipart form
	form, _ := c.MultipartForm()
	num := 0
	for _, files := range form.File {
		for _, file := range files {

			err := c.SaveUploadedFile(file, "./upload/"+"/"+file.Filename)
			if err != nil {
				log.Println("upload err filename:", file.Filename)
			} else {
				log.Println("upload success filename:", file.Filename)
				num++
			}
		}
	}
	c.JSON(http.StatusOK, gin.H{"num": num})
}
```
