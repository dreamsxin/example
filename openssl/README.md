# OpenSSL

https://github.com/openssl/openssl


## 对消息进行加密和解密

使用`Base64`加密、解密

加密
```
$ echo "this is a test" | openssl enc -base64
$ dGhpcyBpcyBhIHRlc3QK
```

解密
```
$ echo "dGhpcyBpcyBhIHRlc3QK" | openssl enc -base64 -d
$ this is a test
```

使用`AES`加密、解密 `-a` 基于 `ASCII`文本 `-d` 解密

加密
```
$ echo "test" | openssl enc -aes-256-cbc -a
$ U2FsdGVkX1+xr/JzOQ8JodqznjjCvbJvIpcKD2hE7/M=
```

解密
```
$ echo "U2FsdGVkX1+xr/JzOQ8JodqznjjCvbJvIpcKD2hE7/M=" | openssl enc -aes-256-cbc -d -a
$ test
```

## 对文件进行加密和解密

加密
```
$ echo "test" | openssl enc -aes-256-cbc > openssl.dat
$ openssl.dat
```

解密
```
$ $ openssl enc -aes-256-cbc -in openssl.dat -out seopenssl.dat
$ openssl.dat
```

## 对目录进行加密和解密

加密
```
$ tar cz shell | openssl enc -aes-256-cbc -out shell.tar.gz.dat
```

解密
```
$ openssl enc -aes-256-cbc -d -in shell.tar.gz.dat | tar xz
```

##

```
//对文件进行base64编码
openssl enc -base64 -in plain.txt -out base64.txt
//对base64格式文件进行解密操作
openssl enc -base64 -d -in base64.txt -out plain2.txt
//使用diff命令查看可知解码前后明文一样
diff plain.txt plain2.txt
```

## 不同方式的密码输入方式

```shell
//命令行输入，密码123456
openssl enc -aes-128-cbc -in plain.txt -out out.txt -pass pass:123456

//文件输入，密码123456
echo 123456 > passwd.txt
openssl enc -aes-128-cbc -in plain.txt -out out.txt -pass file:passwd.txt

//环境变量输入，密码123456
passwd=123456
export passwd
openssl enc -aes-128-cbc -in plain.txt -out out.txt -pass env:passwd

//从文件描述输入
openssl enc -aes-128-cbc -in plain.txt -out out.txt -pass fd:1  
//从标准输入输入
openssl enc -aes-128-cbc -in plain.txt -out out.txt -pass stdin
```

## 固定salt值加密
```text
$ openssl enc -aes-128-cbc -in plain.txt -out encrypt.txt -pass pass:123456 -P
salt=32F5C360F21FC12D
key=D7E1499A578490DF940D99CAE2E29EB1
iv =78EEB538897CAF045F807A97F3CFF498

$ openssl enc -aes-128-cbc -in plain.txt -out encrypt.txt -pass pass:123456 -P
salt=DAA482697BECAB46
key=9FF8A41E4AC011FA84032F14B5B88BAE
iv =202E38A43573F752CCD294EB8A0583E7

$ openssl enc -aes-128-cbc -in plain.txt -out encrypt.txt -pass pass:123456 -P -S 123
salt=1230000000000000
key=50E1723DC328D98F133E321FC2908B78
iv =1528E9AD498FF118AB7ECB3025AD0DC6

$ openssl enc -aes-128-cbc -in plain.txt -out encrypt.txt -pass pass:123456 -P -S 123
salt=1230000000000000
key=50E1723DC328D98F133E321FC2908B78
iv =1528E9AD498FF118AB7ECB3025AD0DC6
```

可以看到，不使用-S参数，salt参数随机生成，key和iv值也不断变化，当slat值固定时，key和iv值也是固定的。