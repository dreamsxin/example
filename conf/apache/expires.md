```conf
Header set Cache-Control "private"
```
```conf
<IfModule mod_expires.c>
  ExpiresActive on
  ExpiresByType text/cache-manifest           "access plus 0 seconds"
  ExpiresByType text/html                     "access plus 0 seconds"
  # Data
  ExpiresByType text/xml                      "access plus 0 seconds"
  ExpiresByType application/xml               "access plus 0 seconds"
  ExpiresByType application/json              "access plus 0 seconds"
  # Feed
  ExpiresByType application/rss+xml           "access plus 1 hour"
  ExpiresByType application/atom+xml          "access plus 1 hour"
  # Media: images, video, audio
  ExpiresByType image/gif                     "access plus 1 month"
  ExpiresByType image/png                     "access plus 1 month"
  ExpiresByType image/jpg                     "access plus 1 month"
  ExpiresByType image/jpeg                    "access plus 1 month"
  ExpiresByType image/x-icon                  "access plus 1 month"
  ExpiresByType video/ogg                     "access plus 1 month"
  ExpiresByType audio/ogg                     "access plus 1 month"
  ExpiresByType video/mp4                     "access plus 1 month"
  ExpiresByType video/webm                    "access plus 1 month"
  # HTC files  (css3pie)
  ExpiresByType text/x-component              "access plus 1 month"
  # Webfonts
  ExpiresByType application/x-font-ttf        "access plus 1 month"
  ExpiresByType font/opentype                 "access plus 1 month"
  ExpiresByType application/x-font-woff       "access plus 1 month"
  ExpiresByType image/svg+xml                 "access plus 1 month"
  ExpiresByType application/vnd.ms-fontobject "access plus 1 month"
  # CSS and JavaScript
  ExpiresByType text/css                      "access plus 1 year"
  ExpiresByType text/javescript               "access plus 1 year"
  ExpiresByType application/javascript        "access plus 1 year"
</IfModule>
```
PHP 需要修改配置
```conf
session.cache_limiter = public, max-age=
```