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

因為不同的網頁伺服器會有不同的記錄檔格式，在使用 GoAccess 之前，我們要將 GoAccess 的解析設定調整成適合自己伺服器的格式。

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