# 编译 Android 项目

## 查看可执行任务列表

```shell
./gradlew tasks
```

## 生成 apk
```shell
./gradlew assembleRelease
```

## 常见错误

Expiring Daemon because JVM heap space is exhausted
Daemon will be stopped at the end of the build after running out of JVM memory

```shell
export DEFAULT_JVM_OPTS="-Xmx512m"
// 或添加
export GRADLE_OPTS="$GRADLE_OPTS -Xmx512m"
```
在 ~/.gradle 目录下的 `gradle.properties` 文件中新增下面的配置即可：

```ini
// 增加jvm的内存
org.gradle.jvmargs=-Xmx4096m -XX:MaxPermSize=512m
// 并行
org.gradle.parallel=true
/ 守护进程(复用每个gradle进程)
org.gradle.daemon=true
```

## 生成签名

```shell
keytool -genkey -keystore ./keystore.sign -keyalg RSA -validity 10000 -alias phalcon7
//注：validity为天数，keyfile为生成key存放的文件，RSA为指定的加密算法(可用RSA或DSA)
```
`signing.properties`
```shell
STORE_FILE = ./keystore.sign
STORE_PASSWORD = 123456
KEY_ALIAS = 
```