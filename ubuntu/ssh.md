# 配置ssh密钥登录(无需密码)

## 生成密钥对

```shell
ssh-keygen
```

## 拷贝公钥到服务器

方式一：

```shell
ssh-copy-id -i ~/.ssh/id_rsa.pub xxx@host
```

方式二：

手动拷贝id_rsa.pub到服务器上，并收到添加认证文件。

```shell
cat id_rsa.pub >> ~/.ssh/authorized_keys
```

## 配置服务器

`/etc/ssh/sshd_config`文件，修改以下配置项
```conf
RSAAuthentication yes
PubkeyAuthentication yes
AuthorizedKeysFile  .ssh/authorized_keys
```