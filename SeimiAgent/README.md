```shell
wget http://seimidl.wanghaomiao.cn/seimiagent_linux_v1.1.0_x86_64.tar.gz
tar zxvf seimiagent_linux_v1.1.0_x86_64.tar.gz
cd seimiagent/ && ./bin/seimiagent -p 8000
wget -O baidu.png --post-data "url=http://baidu.com&contentType=img" "http://localhost:8000/doload"
curl -o baidu.png -d "url=http://baidu.com&contentType=img" "http://localhost:8000/doload"
```