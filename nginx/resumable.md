```conf
server {
	[...]
	location /resumable_upload {
	       upload_resumable on;
	       upload_state_store /usr/local/nginx/upload_temp ;
	       upload_pass @drivers_upload_handler;
	       upload_store /usr/local/nginx/upload_temp;
	       upload_set_form_field $upload_field_name.path "$upload_tmp_path";
	}

	location @resumable_upload_handler {
	       proxy_pass http://localhost:8002;
	}
	[...]
}
```
### 上传文件第一个片段
```txt
POST /upload HTTP/1.1
Host: example.com
Content-Length: 51201
Content-Type: application/octet-stream
Content-Disposition: attachment; filename="big.TXT"
X-Content-Range: bytes 0-51200/511920
Session-ID: 1111215056
    
<0-51200的字节文件数据>
```
### 上传文件第一个片段服务器响应
```txt
HTTP/1.1 201 Created
Date: Thu, 02 Sep 2010 12:54:40 GMT
Content-Length: 14
Connection: close
Range: 0-51200/511920
     
0-51200/511920
上传文件最后一个片段
POST /upload HTTP/1.1
Host: example.com
Content-Length: 51111
Content-Type: application/octet-stream
Content-Disposition: attachment; filename="big.TXT"
X-Content-Range: bytes 460809-511919/511920
Session-ID: 1111215056

<460809-511919字节文件数据>
```
### 上传文件最后一个片段服务器响应
```txt
HTTP/1.1 200 OK
Date: Thu, 02 Sep 2010 12:54:43 GMT
Content-Type: text/html
Connection: close
Content-Length: 2270
     
< 响应的内容>
```
### 请求头说明
```txt
  请求头 	                      说明
Content-Disposition  attachment, filename=“上传的文件名”
Content-Type 	     待上传文件的mime type,如application/octet-stream(注:不能为multipart/form-data)
X-Content-Range      待上传文件字节范围,如第一片段bytes 0-51200/511920,最后一个片段bytes 460809-511919/511920(注:文件第一个字节标号为0,最后一个字节标号为n-1,其中n为文件字节大小)
X-Session-ID 	     上传文件的标识,由客户端随机指定.因为是断点续传,客户端必须确保同一个文件的所有片段上传标识一致
Content-Length 	     上传片段的大小
```
## Python上传demo
```python
#!/usr/bin/python
# -*- coding: utf-8 -*- 

import os.path
import requests
import hashlib
     
# 待上传文件路径
FILE_UPLOAD = "/tmp/testfile"
# 上传接口地址
UPLOAD_URL = "http://host/drivers_upload"
# 单个片段上传的字节数
SEGMENT_SIZE = 1048576
    
def upload(fp, file_pos, size, file_size):
        session_id = get_session_id()
        fp.seek(file_pos)
        payload = fp.read(size)
        content_range = "bytes {file_pos}-{pos_end}/{file_size}".format(file_pos=file_pos,
                        pos_end=file_pos+size-1,file_size=file_size)
        headers = {'Content-Disposition': 'attachment; filename="big.TXT"','Content-Type': 'application/octet-stream',
                    'X-Content-Range':content_range,'Session-ID': session_id,'Content-Length': size}
        res = requests.post(UPLOAD_URL, data=payload, headers=headers)
        print(res.text)
     
     
# 根据文件名hash获得session id
def get_session_id():
  m = hashlib.md5()
  file_name = os.path.basename(FILE_UPLOAD)
  m.update(file_name)
  return m.hexdigest()
     
def main():
  file_pos = 0
  file_size = os.path.getsize(FILE_UPLOAD)
  fp = open(FILE_UPLOAD,"r")
     
  while True:
   if file_pos + SEGMENT_SIZE >= file_size:
       upload(fp, file_pos, file_size - file_pos, file_size)
       fp.close()
       break
   else:
     upload(fp, file_pos, SEGMENT_SIZE, file_size)
     file_pos = file_pos + SEGMENT_SIZE
     
if __name__ == "__main__":
        main()
```
