# Nchan



## 配置

```conf
server {
        listen 80 default_server;
        listen [::]:80 default_server;

        root /var/www/html;

        # Add index.php to the list if you are using PHP
        index index.html index.htm index.nginx-debian.html index.php;

        server_name localhost;

        location / {
                # First attempt to serve request as file, then
                # as directory, then fall back to displaying a 404.
                try_files $uri $uri/ /index.php?$query_string;
        }

        # pass the PHP scripts to FastCGI server listening on 127.0.0.1:9000
        location ~ \.php$ {
                include snippets/fastcgi-php.conf;
                fastcgi_pass 127.0.0.1:9000;
        }
}

server {
        listen 9999 default_server;
        listen [::]:9999 default_server;

        location ~ /pub/(.*) {
                nchan_publisher;
                nchan_channel_id $1;
                nchan_message_buffer_length 10;
                nchan_message_timeout 5m;
        }

        location ~ /sub/(.*) {
                nchan_subscriber;
                nchan_subscriber_first_message newest;
                nchan_channel_id $1;
        }
}
```