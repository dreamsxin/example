# Nginx 日志

http://nginx.org/en/docs/http/ngx_http_log_module.html

## 格式

```conf
log_format compression '$remote_addr - $remote_user [$time_local] '
                       '"$request" $status $bytes_sent '
                       '"$http_referer" "$http_user_agent" "$gzip_ratio"';

access_log /spool/logs/nginx-access.log compression buffer=32k;
```

## 按时间分隔 nginx generate log file by date

```conf
map $time_iso8601 $logdate {
	'~^(?\d{4}-\d{2}-\d{2})' $ymd;
	default 'nodate';
}

accesslog '/var/log/nginx/access${logdate}.log';
open_log_file_cache max=10;
```