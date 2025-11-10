# CURL


## 查看返回头信息

```shell
curl -I http://localhost
```

## 指定情求头信息

```shell
curl -I -H "Accept-Language: es" http://localhost
```

让 nginx 返回 content-length 头信息
```shell
curl -v -H "Connection:keep-alive" http://localhost
```

## Post 数据

```shell
curl -I -d "name=test&password=test" http://localhost
```

## 传送二进制数据

```shell
curl -v --data-binary @xxx.bin  --header "application/octet-stream" http://localhost/
```

## 指定请求方法

```shell
curl -I -X DELETE http://localhost
```

输出所有发送到服务器的请求信息及返回信息

```shell
curl -v http://localhost
```

```shell
curl -X POST \
  "http://localhost:3001/api/users/login" \
  --header 'Content-Type: application/json' \
  --header 'Accept: application/json' \
  -d '
  {
    "email": "frog@example.com",
    "password": "fishfish"
  }'
```

## 使用 python
`usersLogin.py`
```python
#!/usr/bin/env bash
# Users
usersLogin() {
  email=$1
  password=$2
  curl -X POST \
    "$URL/users/login" \
    --header 'Content-Type: application/json' \
    --header 'Accept: application/json' \
    -d '
    {
      "email": "'"$email"'",
      "password": "'"$password"'"
    }'
}
# Variables
URL="http://localhost:3001/api"
```
执行
```shell
usersLogin.py 'frog@example.com' 'fishfish' | jq
```
`usersFindById.py`
```python
#!/usr/bin/env bash
# Users
usersLogin() {
  email=$1
  password=$2
  curl -X POST \
    "$URL/users/login" \
    --header 'Content-Type: application/json' \
    --header 'Accept: application/json' \
    -d '
    {
      "email": "'"$email"'",
      "password": "'"$password"'"
    }'
}
usersFindById() {
  id=${1:-1}
  curl -X GET \
    "$URL/users/$id" \
    --header "Authorization: Bearer $TOKEN"
}
# Variables
URL="http://localhost:3001/api"
TOKEN="$(usersLogin 'frog@example.com' 'fishfish' | jq -r .token)"
```
使用
```shell
usersFindById 1 | jq
```
## 代理主机和端口

```shell
curl -x proxysever.test.com:3128 http://google.co.in
```

## 网络限速

下载速度最大不会超过1000B/second
```shell
curl --limit-rate 1000B -O http://www.gnu.org/software/gettext/manual/gettext.html
```

## 分段下载 range

```shell
curl -r 0-1024000 -o new_divide_1.mp3 http://localhost/test.mp3
curl -r 1024001-2048000 -o new_divide_2.mp3 http://localhost/test.mp3
curl -r 2048001- -o new_divide_3.mp3 http://localhost/test.mp3

cat new_divide_1.mp3 new_divide_2.mp3 new_divide_3.mp3 > test.mp3

```

当文件在下载完成之前结束该进程，通过添加`-C`选项继续对该文件进行下载，已经下载过的文件不会被重新下载
```shell
curl -C - -O http://www.gnu.org/software/gettext/manual/gettext.html
```

## 上传文件到FTP服务器

将myfile.txt文件上传到服务器
```shell
curl -u ftpuser:ftppass -T myfile.txt ftp://ftp.testserver.com
```
同时上传多个文件
```shell
curl -u ftpuser:ftppass -T "{file1,file2}" ftp://ftp.testserver.com
```
从标准输入获取内容保存到服务器指定的文件中
```shell
curl -u ftpuser:ftppass -T - ftp://ftp.testserver.com/myfile_1.txt
```

## curl命令测试网络请求中DNS解析、响应时间

经查遇到需要测量网络请求时间的问题，包括DNS解析、连接、传输等时间。Linux下的curl命令可以很好的测量网络请求。

如下
```shell
curl -o /dev/null -s -w %{time_connect}:%{time_starttransfer}:%{time_total} http://www.baidu.com
curl -o /dev/null -s -w %{time_connect}-Start:%{time_starttransfer}-Seep:%{speed_download}-Total:%{time_total}-DNS:%{time_namelookup} http://www.baidu.com
```


- -o 表示输出结果到 /dev/null
- -s 表示去除状态信息
- -w 表示列出后面的参数的结果

curl命令支持的参数，有如下
```text
time_connect        建立到服务器的 TCP 连接所用的时间
time_starttransfer  在发出请求之后,Web 服务器返回数据的第一个字节所用的时间
time_total          完成请求所用的时间
time_namelookup     DNS解析时间,从请求开始到DNS解析完毕所用时间(记得关掉 Linux 的 nscd 的服务测试)
speed_download      下载速度，单位-字节每秒。
```

通过该命令，可以计算网络请求中DNS解析、连接、传输及总的时间，进行初步的故障排查。

```txt
# 调试类
-v, --verbose                          输出信息
-q, --disable                          在第一个参数位置设置后 .curlrc 的设置直接失效，这个参数会影响到 -K, --config 
-A, --user-agent                       指定客户端的用户代理
-e, --referer                          设置请求头 Referer
-k,--insecure                          允许连接到 SSL 站点，而不使用证书
-K, --config FILE                      指定配置文件
-L, --location                         跟踪重定向 (H)

# CLI显示设置
-s, --silent                           Silent模式。不输出任务内容
-S, --show-error                       显示错误. 在选项 -s 中，当 curl 出现错误时将显示
-f, --fail                             不显示 连接失败时HTTP错误信息
-i, --include                          显示 response的header (H/F)
-I, --head                             仅显示 响应文档头
-l, --list-only                        只列出FTP目录的名称 (F)
-#, --progress-bar                     以进度条 显示传输进度# 数据传输类
-X, --request [GET|POST|PUT|DELETE|…]  使用指定的 http method 例如 -X POST
-H, --header <header>                  设定 request里的header 例如 -H "Content-Type: application/json"
-e, --referer                          设定 referer (H)
-d, --data <data>                      设定 http body 默认使用 content-type application/x-www-form-urlencoded (H)    
--data-raw <data>                      ASCII 编码 HTTP POST 数据 (H)    
--data-binary <data>                   binary 编码 HTTP POST 数据 (H)    
--data-urlencode <data>                url 编码 HTTP POST 数据 (H)-G, 
--get                                  使用 HTTP GET 方法发送 -d 数据 (H)
-F, --form <name=string>               模拟 HTTP 表单数据提交 multipart POST (H)    
--form-string <name=string>            模拟 HTTP 表单数据提交 (H)
-u, --user <user:password>             使用帐户，密码 例如 admin:password
-b, --cookie <data>                    cookie 文件 (H)
-j, --junk-session-cookies             读取文件中但忽略会话cookie (H)
-A, --user-agent                       user-agent设置 (H)

# 传输设置
-C, --continue-at OFFSET               断点续转
-x, --proxy [PROTOCOL://]HOST[:PORT]   在指定的端口上使用代理
-U, --proxy-user USER[:PASSWORD]       代理用户名及密码

# 文件操作
-T, --upload-file <file>               上传文件
-a, --append                           添加要上传的文件 (F/SFTP)# 输出设置
-o, --output <file>                    将输出写入文件，而非 stdout
-O, --remote-name                      将输出写入远程文件
-D, --dump-header <file>               将头信息写入指定的文件
-c, --cookie-jar <file>                操作结束后，要写入 Cookies 的文件位
```

```shell
curl -o /dev/null -s -w %{http_code}:%{http_connect}:%{content_type}:%{time_namelookup}:%{time_redirect}:%{time_pretransfer}:%{time_connect}:%{time_starttransfer}:%{time_total}:%{speed_download} www.baidu.com
```

`curl_time.txt`
```plainText
              http: %{http_code}\n
               dns: %{time_namelookup}s\n
          redirect: %{time_redirect}s\n
      time_connect: %{time_connect}s\n
   time_appconnect: %{time_appconnect}s\n
  time_pretransfer: %{time_pretransfer}s\n
time_starttransfer: %{time_starttransfer}s\n
     size_download: %{size_download}bytes\n
    speed_download: %{speed_download}B/s\n
               ----------\n
        time_total: %{time_total}s\n
```

```shell
curl -w "@~/curl_time.txt" -s -H "Content-Type: application/json" www.baidu.com
```
`curl -w "\n\n时间摘要：\n-----------\n总时间: %{time_total}s\nDNS解析: %{time_namelookup}s\n建立TCP连接: %{time_connect}s\nSSL握手: %{time_appconnect}s\n传输开始: %{time_pretransfer}s\n重定向: %{time_redirect}s\n首字节时间: %{time_starttransfer}s\n" -o /dev/null -s "https://你的域名"`

### 关键指标分析：

- time_appconnect： 这是SSL握手完成所花费的总时间。如果这个值非常高（例如 > 0.5s），那么问题极有可能出在SSL握手环节。
- time_starttransfer： 从请求开始到服务器返回第一个字节的时间。如果这个值高但 time_appconnect 也高，说明延迟主要耗在SSL握手上。
- time_connect： 建立TCP连接的时间。这个时间应该很短（ms级），如果它很高，可能是网络问题。

### 测试代理

```sh
curl -x http://xxx:xxx@xxxx:xxxx -o /dev/null -s -w "DNS解析: %{time_namelookup}s\n建立连接: %{time_connect}s\nSSL握手: %{time_appconnect}s\n准备传输: %{time_pretransfer}s\n开始传输: %{time_starttransfer}s\n总时间: %{time_total}s\n"  'https://www.google.com/search?q=test1xxx'
```

以下是循环测试 100 次的几种方法：

## 1. 基础循环测试

```bash
for i in {1..100}; do
    echo "测试第 $i 次:"
    curl -w "\
DNS 解析: %{time_namelookup}s\n\
连接建立: %{time_connect}s\n\
SSL握手: %{time_appconnect}s\n\
第一字节: %{time_starttransfer}s\n\
总时间: %{time_total}s\n\n" \
    -o /dev/null -s "https://example.com"
done
```

## 2. 只输出关键数据（便于分析）

```bash
for i in {1..100}; do
    curl -w "第$i次:dns:%{time_namelookup},connect:%{time_connect},ssl:%{time_appconnect},first_byte:%{time_starttransfer},total:%{time_total}\n" \
    -o /dev/null -s "https://example.com"
done
```

## 3. 保存到文件并统计

```bash
# 清空或创建结果文件
> curl_results.txt

# 执行100次测试
for i in {1..100}; do
    echo "第 $i 次测试..." 
    curl -w "dns:%{time_namelookup},connect:%{time_connect},ssl:%{time_appconnect},first_byte:%{time_starttransfer},total:%{time_total}\n" \
    -o /dev/null -s "https://example.com" >> curl_results.txt
done

echo "测试完成！结果已保存到 curl_results.txt"
```

## 4. 带进度显示的版本

```bash
total=100
for i in $(seq 1 $total); do
    # 显示进度
    echo -n "进度: $i/$total "
    
    # 执行测试并输出结果
    curl -w "dns:%.3f,connect:%.3f,ssl:%.3f,first_byte:%.3f,total:%.3f\n" \
    -o /dev/null -s "https://example.com"
    
    # 可选：添加延迟，避免对服务器造成压力
    sleep 0.1
done
```

## 5. 使用函数封装（推荐）

```bash
#!/bin/bash

test_url() {
    local url="$1"
    local count="${2:-100}"
    
    echo "开始测试 $url, 共 $count 次..."
    echo "=========================================="
    
    # 测试头
    echo "次数 DNS解析 TCP连接 SSL握手 第一字节 总时间"
    echo "---- -------- -------- -------- -------- --------"
    
    for i in $(seq 1 $count); do
        printf "%-4d" $i
        curl -w " %{time_namelookup} %{time_connect} %{time_appconnect} %{time_starttransfer} %{time_total}\n" \
        -o /dev/null -s "$url"
    done
}

# 使用示例
test_url "https://example.com" 100
```

## 6. 生成统计报告

```bash
#!/bin/bash

URL="https://example.com"
COUNT=100
RESULTS_FILE="curl_test_results.txt"

# 清空结果文件
> "$RESULTS_FILE"

echo "开始性能测试: $URL"
echo "测试次数: $COUNT"
echo "开始时间: $(date)"
echo "=" * 50

# 执行测试
for i in $(seq 1 $COUNT); do
    echo -n "."
    curl -w "%{time_namelookup} %{time_connect} %{time_appconnect} %{time_starttransfer} %{time_total}\n" \
    -o /dev/null -s "$URL" >> "$RESULTS_FILE"
    
    # 每10次换行显示进度
    if [ $((i % 10)) -eq 0 ]; then
        echo " $i/$COUNT"
    fi
done

echo
echo "测试完成!"
echo "结果文件: $RESULTS_FILE"

# 生成统计信息
echo
echo "统计信息:"
echo "---------"
awk '{
    dns_sum += $1; connect_sum += $2; ssl_sum += $3; 
    first_byte_sum += $4; total_sum += $5;
    dns_arr[NR] = $1; connect_arr[NR] = $2; ssl_arr[NR] = $3;
    first_byte_arr[NR] = $4; total_arr[NR] = $5;
} 
END {
    asort(dns_arr); asort(connect_arr); asort(ssl_arr);
    asort(first_byte_arr); asort(total_arr);
    
    print "DNS解析:   平均 " dns_sum/NR "s, 最小 " dns_arr[1] "s, 最大 " dns_arr[NR] "s, 中位数 " dns_arr[int(NR/2)] "s"
    print "TCP连接:   平均 " connect_sum/NR "s, 最小 " connect_arr[1] "s, 最大 " connect_arr[NR] "s, 中位数 " connect_arr[int(NR/2)] "s"
    print "SSL握手:   平均 " ssl_sum/NR "s, 最小 " ssl_arr[1] "s, 最大 " ssl_arr[NR] "s, 中位数 " ssl_arr[int(NR/2)] "s"
    print "第一字节:  平均 " first_byte_sum/NR "s, 最小 " first_byte_arr[1] "s, 最大 " first_byte_arr[NR] "s, 中位数 " first_byte_arr[int(NR/2)] "s"
    print "总时间:    平均 " total_sum/NR "s, 最小 " total_arr[1] "s, 最大 " total_arr[NR] "s, 中位数 " total_arr[int(NR/2)] "s"
}' "$RESULTS_FILE"
```

## 7. 简单的一行命令

```bash
for i in {1..100}; do curl -w "dns:%{time_namelookup} total:%{time_total}\n" -so /dev/null https://example.com; done
```
