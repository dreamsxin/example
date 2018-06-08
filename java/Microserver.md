# Microserver

Microserver 是一个零配置、基于标准的身经百战的库，用来运行 Java REST 微服务，通过 Java 标准 main 类执行。从 2014 年开始就一直在 AOL 生产环境中使用。

```shell
https://github.com/aol/micro-server.git
cd micro-server
./gradlew :micro-tomcat-with-jersey:tasks install
```
```shell
mkdir demo
gradle wrapper
```
编写`build.gradle`
```gradle
repositories { jcenter() }

buildscript {
	repositories { maven { url "https://plugins.gradle.org/m2/" } }
	dependencies { classpath "com.github.jengelman.gradle.plugins:shadow:1.2.4" }
}
apply plugin: "com.github.johnrengelman.shadow"
apply plugin: 'java'
dependencies {
	compile group: 'com.oath.microservices', name:'micro-grizzly-with-jersey', version:'0.87.1'
}
shadowJar { mergeServiceFiles()  }

```

编写代码

`mkdir -p src/main/java/hello`

构建运行

```shell
gradle tasks
./gradlew bootRun
```
