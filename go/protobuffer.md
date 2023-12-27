
https://github.com/protocolbuffers/protobuf/releases
## windows
`include` 里的文件放到 go\bin\protoc.exe 目录
```shell
go install google.golang.org/protobuf/cmd/protoc-gen-go@latest
protoc -I=$SRC_DIR --go_out=$DST_DIR $SRC_DIR/addressbook.proto
protoc -I ../proto ../proto/msg.proto --go_out=../../
```
