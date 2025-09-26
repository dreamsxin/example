```shell
docker run -e HTTP_PROXY=http://proxy.example.com:8080/ -e HTTPS_PROXY=http://proxy.example.com:8080/ -e NO_PROXY=localhost,127.0.0.1,docker-registry.example.com,.example.com <image>
```
