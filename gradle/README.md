# Gradle

https://gradle.org/install/

## 安装

* 确认环境

```shell
java -version
```

* 使用 SDKMAN 安装

https://github.com/sdkman/sdkman-cli

```shell
curl -s "https://get.sdkman.io" | bash 
sdk install gradle 4.7
```

## 使用 Gradle Wrapper

生成

```shell
gradle wrapper
```

产生以下目录结构：
```text
.
├── build.gradle
├── settings.gradle
├── gradle
│   └── wrapper
│       ├── gradle-wrapper.jar
│       └── gradle-wrapper.properties
├── gradlew
└── gradlew.bat
```

* 选项

- --gradle-version

The Gradle version used for downloading and executing the Wrapper.

- --distribution-type

The Gradle distribution type used for the Wrapper. Available options are bin and all. The default value is bin.

- --gradle-distribution-url

The full URL pointing to Gradle distribution ZIP file. Using this option makes --gradle-version and --distribution-type obsolete as the URL already contains this information. This option is extremely valuable if you want to host the Gradle distribution inside your company’s network.

- --gradle-distribution-sha256-sum

The SHA256 hash sum used for verifying the downloaded Gradle distribution.

```shell
gradle wrapper --gradle-version 4.0 --distribution-type all
```

* 提供 `build.gradle`、`settings.gradle`

项目中一般会出现2个或者多个build.gradle文件，一个在最完全的目录下，一个在app目录下。如果切换到Android模式下则全部在Gradle Scripts。

```text
.
├── build.gradle
├── settings.gradle
├── gradle
│   └── wrapper
│       ├── gradle-wrapper.jar
│       └── gradle-wrapper.properties
├── gradlew
└── gradlew.bat
```

## 使用 Gradle 构建 Java 项目

* 创建目录结构

在项目主目录下，创建以下子目录;在 *nix 系统下可以使用命令：

```shell
mkdir -p src/main/java/hello
```

在 src/main/java/hello 目录中，你可以创建任何 Java 类。为简单起见并且为了与指南的其余部分保持一致，我们建议创建两个类 HelloWorld.java 和 Greeter.java

`src/main/java/hello/HelloWorld.java` 的源代码：

```java
package hello;

public class HelloWorld {
  public static void main(String[] args) {
    Greeter greeter = new Greeter();
    System.out.println(greeter.sayHello());
  }
}
```

`src/main/java/hello/Greeter.java` 的源代码：

```java
package hello;

public class Greeter {
  public String sayHello() {
    return "Hello world!";
  }
}
```

在我们为项目创建 build.gradle 文件之前，我们可以先问一下Gradle目前有哪些可用的任务（Tasks）：

```shell
gradle tasks
```

如果在 `build.gradle` 中配置了插件(plugins)上面的任务列表会变得更长，最好在配置插件后使用 gradle task 看看那些任务是可用的。

* 构建Java代码

先从简单的开始，创建一个最简单的只有一行的 build.gradle 文件：
```text
apply plugin: 'java'
```

这行行配置，用来编译java项目的任务、用来创建JavaDoc的任务、用来执行单元测试的任务。

我们经常使用的任务是`gradle build`，这个任务执行以下操作：编译、执行单元测试、组装Jar文件。

构建已经完成之后可以看到很多子目录，其中有三个我们需要特别注意：

- classes
保存被编译后的.class文件

- reports
构建报告（如：测试报告）

- lib
组装好的项目包（通常为：.jar或者.war文件）

* 申明依赖

假设我们希望更好的说”Hello World!”，我们希望应用程序能同时输出当前的日期和时间。
首先，把HelloWorld.jara类修改成下面这样：
```java
package hello;
 
import org.joda.time.LocalTime;
 
public class HelloWorld {
  public static void main(String[] args) {
    LocalTime currentTime = new LocalTime();
    System.out.println("The current local time is: " + currentTime);

    Greeter greeter = new Greeter();
    System.out.println(greeter.sayHello());
  }
}
```
这里我们的`HelloWorld`使用”Joda Time”的LocalTime类来获取和输出当前时间。
添加一些配置来定义第三方库的来源：
```text
repositories {
    mavenLocal()
    mavenCentral()
}
```
上面的repositories定义，告诉构建系统通过Maven中央库来检索项目依赖的软件包，现在我们可以使用第三方库了，但现需要定义：
```text
dependencies {
    compile "joda-time:joda-time:2.2"
}
```
使用dependencies块，我们定义了一条Joda Time的依赖项。这里，明确指定使用joda-time组内的版本为2.2的joda-time库。
我们在这个依赖定义中指定依赖是compile范围的。意思是，这个库在编译和运行时都需要。

另外值得注意的依赖类型包括：

- providedCompile
在编译期间需要这个依赖包，但在运行期间可能由容器提供相关组件（比如：Java Servlet API）

- testCompile
依赖项仅在构建和运行测试代码时需要，在项目运行时不需要这个依赖项。

我们来定义我们将生成的Jar文件的名字：
```text
jar {
    baseName = 'gs-gradle'
    version =  '0.1.0'
}
```
在上面的例子中，我们的JAR文件的名字为：`gs-gradle-0.1.0.jar`。

`build.gradle` 文件中增加以下代码用来指定 Gradle 版本用来初始化`wrapper`脚本：
```text
task wrapper(type: Wrapper) {
    gradleVersion = '1.11'
}
```

* 下载和初始化 wrapper 脚本

```text
gradle wrapper
```

命令执行完后，我们可以看到增加了一些新文件。有两个文件在根目录下，

wapper 的 jar 文件和 properties 文件在新增的 gradle/wrapper 目录下。

运行 wrapper 脚本来构建系统，跟我们之前的命令很像：
```shell
./gradlew build
```

要想让代码可以运行，我们可以使用Gradle的application插件。增加以下内容到build.gradle文件中。

```text
apply plugin: 'application'
mainClassName = 'hello.HelloWorld'
```
现在我们的app可以执行了。
```shell
./gradlew run
```

## gradle 移除依赖冲突

Gradle的依赖分析功能

```shell
./gradlew dependencyInsight --dependency slf4j-api
```
解决冲突办法
```gradle
// project用括号包裹住
compile (project(':uisdk:Library:facebook')) {
    exclude group: 'com.android.support', module: 'appcompat-v7'
}
```
遇到了exclude无效的问题，使用全局移除就正常了:
```gradle
// 排除掉V4包中引入的android.arch.lifecycle:runtime导致构建失败
configurations.all {
    exclude group: 'android.arch.lifecycle', module: 'runtime'
}

compile ('org.avaje.ebeanorm:avaje-ebeanorm:7.9.1') {
  exclude group: 'org.slf4j', module: 'slf4j-api'
  exclude group: 'ch.qos.logback', module: 'logback-classic'
}
```
