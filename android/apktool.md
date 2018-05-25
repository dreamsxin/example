# Apktool

https://github.com/iBotPeaches/Apktool

```shell
apktool d -f xxx.apk
apktool b -f xxx
```

```shell
keytool -genkey -keystore ks2.keystore -keyalg RSA -validity 10000 -alias ks2.keystore
jarsigner -verbose -digestalg SHA1 -sigalg MD5withRSA -keystore ks2.keystore -signedjar xxx_signed.apk xxx.apk ks2.keystore
jarsigner -verify -verbose -certs xxx_signed.apk
zipalign -v 4 xxx_signed.apk xxx_align.apk
```


```shell
git clone https://github.com/AlexeySoshin/smali2java.git
go build
smali2java -path_to_smali=<smali_directory>
```