
- github.com/ncruces/go-dns

## 查看 DNS 缓存

### Windows
```shell
# 显示当前 DNS 缓存内容
ipconfig /displaydns
```
### Linux (systemd-resolved)
```shell
sudo apt install dnsutils
sudo apt install resolvconf

# 查看缓存统计
resolvectl statistics

# 查看特定域名的缓存条目（需要知道记录类型）
resolvectl query httpbin.org
```
