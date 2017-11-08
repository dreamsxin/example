# GoAccess

GoAccess 是一個開放原始碼的網頁伺服器記錄檔分析工具，可以產生網頁版或文字版的動態統計報表，讓系統管理者可以查看即時的系統狀態，其涵蓋的資訊非常詳細，而且產生報表的方式相當簡潔，只要一行指令就可以馬上產生報表，對於 Linux 系統與指令比較熟悉的人而言，是一個非常方便的工具。

## 安裝 GoAccess

我這裡以 Ubuntu Linux 的環境來示範如何安裝 GoAccess。最簡單的安裝方式就是直接使用 apt 從 Ubuntu Linux 官方的套件庫安裝：

```shell
sudo apt-get install goaccess
```

若要安裝最新版的 GoAccess，可以從 GoAccess 的 Debian/Ubuntu 套件庫來下載安裝，首先設定 GoAccess 官方的套件庫：
```shell
echo "deb http://deb.goaccess.io/ $(lsb_release -cs) main" | sudo tee -a /etc/apt/sources.list.d/goaccess.list
wget -O - http://deb.goaccess.io/gnugpg.key | sudo apt-key add -

sudo apt-get update
sudo apt-get install goaccess
```

如果您會需要訪客來源國家的資訊，就要再安裝 GeoIP 資料庫：

```shell
sudo apt-get install geoip-database
```

## 使用 GoAccess 分析 Nginx 記錄檔

```shell
goaccess -f access.log
```

会出现日志格式选择。

在使用 GoAccess 之前，我們可以配置 GoAccess 的解析設定調整成適合自己伺服器的格式。

`/etc/goaccess.conf`

首先調整時間格式：
```conf
# The following time format works with any of the
# Apache/NGINX's log formats below.
time-format %H:%M:%S
```

接著是日期格式：
```conf
# The following date format works with any of the
# Apache/NGINX's log formats below.
date-format %d/%b/%Y
```

記錄內容的格式：
```conf
# NCSA Combined Log Format
log-format %h %^[%d:%t %^] "%r" %s %b "%R" "%u"
```

設定好記錄檔的格式之後，就可以開始使用 GoAccess 分析資料了。

- Nginx Variable                            GoAccess Variable
- $remote_addr                              %r
- $remote_user                              %^ (ignored)
- $time_local                               %d:%^
- $upstream_cache_status                    %^ (ignored)
- $request                                  %r
- $status                                   %s
- $body_bytes_sent                          %b
- $http_referer                             %R
- $http_user_agent                          %u
- $request_time                             %D

将 nginx 日志格式转换为 goaccess 格式：
```shell
#!/bin/bash
#
# Convert from this:
#   http://nginx.org/en/docs/http/ngx_http_log_module.html
# To this:
#   https://goaccess.io/man
#
# Conversion table:
#   $time_local         %d:%t %^
#   $host               %v
#   $http_host          %v
#   $remote_addr        %h
#   $request_time       %T
#   $request_method     %m
#   $request_uri        %U
#   $server_protocol    %H
#   $request            %r
#   $status             %s
#   $body_bytes_sent    %b
#   $bytes_sent         %b
#   $http_referer       %R
#   $http_user_agent    %u
#
# Samples:
#
# log_format combined '$remote_addr - $remote_user [$time_local] '
# '"$request" $status $body_bytes_sent '
# '"$http_referer" "$http_user_agent"';
#   ./nginx2goaccess.sh '$remote_addr - $remote_user [$time_local] "$request" $status $body_bytes_sent "$http_referer" "$http_user_agent"'
#
# log_format compression '$remote_addr - $remote_user [$time_local] '
# '"$request" $status $bytes_sent '
# '"$http_referer" "$http_user_agent" "$gzip_ratio"';
#   ./nginx2goaccess.sh '$remote_addr - $remote_user [$time_local] "$request" $status $bytes_sent "$http_referer" "$http_user_agent" "$gzip_ratio"'
#
# log_format main
# '$remote_addr\t$time_local\t$host\t$request\t$http_referer\t$http_x_mobile_group\t'
# 'Local:\t$status\t$body_bytes_sent\t$request_time\t'
# 'Proxy:\t$upstream_cache_status\t$upstream_status\t$upstream_response_length\t$upstream_response_time\t'
# 'Agent:\t$http_user_agent\t'
# 'Fwd:\t$http_x_forwarded_for';
#   ./nginx2goaccess.sh '$remote_addr\t$time_local\t$host\t$request\t$http_referer\t$http_x_mobile_group\tLocal:\t$status\t$body_bytes_sent\t$request_time\tProxy:\t$upstream_cache_status\t$upstream_status\t$upstream_response_length\t$upstream_response_time\tAgent:\t$http_user_agent\tFwd:\t$http_x_forwarded_for'
#
# log_format main
# '${time_local}\t${remote_addr}\t${host}\t${request_method}\t${request_uri}\t${server_protocol}\t'
# '${http_referer}\t${http_x_mobile_group}\t'
# 'Local:\t${status}\t*${connection}\t${body_bytes_sent}\t${request_time}\t'
# 'Proxy:\t${upstream_status}\t${upstream_cache_status}\t'
# '${upstream_response_length}\t${upstream_response_time}\t${uri}${log_args}\t'
# 'Agent:\t${http_user_agent}\t'
# 'Fwd:\t${http_x_forwarded_for}';
#   ./nginx2goaccess.sh '${time_local}\t${remote_addr}\t${host}\t${request_method}\t${request_uri}\t${server_protocol}\t${http_referer}\t${http_x_mobile_group}\tLocal:\t${status}\t*${connection}\t${body_bytes_sent}\t${request_time}\tProxy:\t${upstream_status}\t${upstream_cache_status}\t${upstream_response_length}\t${upstream_response_time}\t${uri}${log_args}\tAgent:\t${http_user_agent}\tFwd:\t${http_x_forwarded_for}'
#
# Author: Rogério Carvalho Schneider <stockrt@gmail.com>

# Params
log_format="$1"

# Usage
if [[ -z "$log_format" ]]; then
    echo "Usage: $0 '<log_format>'"
    exit 1
fi

# Variables map
conversion_table="time_local,%d:%t_%^
host,%v
http_host,%v
remote_addr,%h
request_time,%T
request_method,%m
request_uri,%U
server_protocol,%H
request,%r
status,%s
body_bytes_sent,%b
bytes_sent,%b
http_referer,%R
http_user_agent,%u"

# Conversion
for item in $conversion_table; do
    nginx_var=${item%%,*}
    goaccess_var=${item##*,}
    goaccess_var=${goaccess_var//_/ }
    log_format=${log_format//\$\{$nginx_var\}/$goaccess_var}
    log_format=${log_format//\$$nginx_var/$goaccess_var}
done
log_format=$(echo "$log_format" | sed 's/${[a-z_]*}/%^/g')
log_format=$(echo "$log_format" | sed 's/$[a-z_]*/%^/g')

# Config output
echo "
- Generated goaccess config:

time-format %T
date-format %d/%b/%Y
log_format $log_format
"

# EOF
```

执行：
```shell
./nginx2goaccess.sh '$remote_addr - $remote_user [$time_local] "$request" $status $body_bytes_sent "$http_referer" "$http_user_agent"'
```
### GoAccess 文字報表

GoAccess 最直接的方式就是使用 -f 參數指定要分析的記錄檔：
```shell
goaccess -f /var/log/nginx/access.log
```

這個報表的內容很詳細，包含訪客人數、請求網址、靜態檔案、404 錯誤網址、來源 IP 位址、訪客作業系統與瀏覽器版本、瀏覽時間、引介網站（referring sites）、訪客來源國家等各種統計資料，使用者可以用上下鍵捲動來查看。

另外它的內容是會動態更新的（類似 top 指令的畫面），所以管理者可以使用 SSH 這類的連線方式連上主機之後，執行這樣的指令後放著監看即時的系統狀態，而不要看的時候就直接關閉，完全不需要為了監控伺服器而啟動任何背景的系統服務，不會造成系統不必要的負擔。

如果要看比較長期性的統計數據，可以將 /var/log/nginx/ 下面的 access.log.* 檔整理出來，一次全部丟給 GoAccess 來分析：
```shell
cat access.log.* | goaccess
```

也可以配合 zcat 一次將所有的記錄檔全部交給 GoAccess 分析：
```shell
zcat -f /var/log/nginx/access.log* | goaccess
```

### GoAccess 網頁報表

如果您感覺文字模式的報表不好看，GoAccess 也可以產生漂亮的網頁報表，只要把 goaccess 指令的輸出導向檔案，就會輸出 html 的報表：
```shell
cat access.log.* | goaccess > output.html
```

## GoAccess 進階使用方式

GoAccess 還有提供很多功能參數可以使用，以下是一些常用參數的使用教學與範例指令。

### 排除 IP 位址

若要排除某些 IP 位址（例如將自己電腦的 IP 位址排除），可以使用 -e 參數指定要排除的 IP 位址範圍：
```shell
goaccess -f /var/log/nginx/access.log -e 192.168.0.1-192.168.0.10
```

這樣就會將 `192.168.0.1-192.168.0.10` 這 10 個 IP 位址的記錄排除。

### 更改配色

GoAccess 文字版的報表可以用 --color-scheme 參數選擇配色，預設是灰色（1），可以改成綠色（2）：
```shell
goaccess -f /var/log/nginx/access.log --color-scheme=2
```
要關閉所有的顏色顯示功能，可以加上 `--no-color`。