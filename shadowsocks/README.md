# 安裝服務端

* Ubuntu

```shell
# sudo apt-get install python-gevent python-pip
# sudo pip install shadowsocks
# sudo apt-get install python-m2crypto
sudo apt-get install shadowsocks
```

配置 `/etc/shadowsocks/config.json`

* CentOS

```shell
yum install python-setuptools
yum install -y python-pip
wget  https://github.com/shadowsocks/shadowsocks/archive/2.9.1.tar.gz
tar zxvf 2.9.1.tar.gz 
cd shadowsocks-2.9.1/
python setup.py install
```

创建文件
`/etc/shadowsocks.json`:
```json
{
    "server":"0.0.0.0",
    "server_port":8388,
    "local_port":1080,
    "password":"123456",
    "timeout":600,
    "method":"aes-256-cfb"
}
```

`/etc/rc.local`
```shell
nohup ssserver -c /etc/shadowsocks.json > /var/log/shadowsocks.log &
```
通过帮助提示我们知道各个参数怎么配置，比如 `sslocal -c` 后面加上我们的json配置文件，或者像下面这样直接命令参数写上运行。

比如
```shell
sslocal -s myleft.org -p 8388 -k "123456" -l 1080 -t 600 -m aes-256-cfb
```
-s表示服务IP, -p指的是服务端的端口，-l是本地端口默认是1080, -k 是密码（要加""）, -t超时默认300,-m是加密方法默认aes-256-cfb，

## 客户端

```shell
sudo pip3 install shadowsocks
```

## 强行升级 pip

```shell
wget https://bootstrap.pypa.io/get-pip.py  --no-check-certificate
sudo python3 get-pip.py --force-reinstall
```

##  undefined symbol: EVP_CIPHER_CTX_cleanup错误。

这个问题是由于在openssl1.1.0版本中，废弃了 `EVP_CIPHER_CTX_cleanup` 函数，如官网中所说：

>> EVP_CIPHER_CTX was made opaque in OpenSSL 1.1.0. As a result, EVP_CIPHER_CTX_reset() appeared and EVP_CIPHER_CTX_cleanup() disappeared. 
>> EVP_CIPHER_CTX_init() remains as an alias for EVP_CIPHER_CTX_reset().


修改方法：

`/usr/local/lib/python3.5/dist-packages/shadowsocks/crypto/openssl.py` 

找到包括的函数改为 
`libcrypto.EVP_CIPHER_CTX_reset.argtypes`
`libcrypto.EVP_CIPHER_CTX_reset(self._ctx)`