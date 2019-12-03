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
set DEFAULT_JVM_OPTS="-Xmx512m"
// 或添加
set GRADLE_OPTS="$GRADLE_OPTS -Xmx512m"
```