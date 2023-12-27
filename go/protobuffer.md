

```shell
go install google.golang.org/protobuf/cmd/protoc-gen-go@latest
protoc -I=$SRC_DIR --go_out=$DST_DIR $SRC_DIR/addressbook.proto
protoc -I ../proto ../proto/msg.proto --go_out=../../
```
