## prerequisites 

```shell
sudo apt-get install gcc make libpcre3-dev zlib1g-dev
```
## Obtain NGINX Open Source.
```shell
wget https://nginx.org/download/nginx-1.18.0.tar.gz
tar zxvf nginx-1.18.0.tar.gz
```
## Obtain the source for the dynamic module.
```shell
git clone https://github.com/perusio/nginx-hello-world-module.git
```

## Compile the dynamic module.
```shell
cd nginx-1.25.1/
./configure --with-compat --add-dynamic-module=../<MODULE-SOURCES>
make modules

ls objs/*.so
objs/ngx_http_hello_world.so
cp objs/ngx_http_hello_world.so ./ngx_http_hello_world_1.25.1.so
```

## Enabling Dynamic Modules

In the main (top-level) context in /etc/nginx/nginx.conf, add a load_module directive for each dynamically loadable module you installed.

```shell
load_module modules/<MODULE-NAME>.so;
```
Check the new configuration for syntactic validity and reload NGINX Plus.

```shell
nginx -t && nginx -s reload
```
