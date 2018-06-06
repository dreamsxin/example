# SpringCloud

https://github.com/spring-projects/spring-boot

## Maven 安装

```shell
sudo apt-get install maven
```

Spring Boot 依赖使用的`groupId`为`org.springframework.boot`。通常，你的 Maven POM 文件会继承 `spring-boot-starter-parent` 工程，并声明一个或多个“Starter POMs”依赖。

```xml
<?xml version="1.0" encoding="UTF-8"?>
<project xmlns="http://maven.apache.org/POM/4.0.0" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
    xsi:schemaLocation="http://maven.apache.org/POM/4.0.0 http://maven.apache.org/xsd/maven-4.0.0.xsd">
    <modelVersion>4.0.0</modelVersion>

    <groupId>com.example</groupId>
    <artifactId>myproject</artifactId>
    <version>0.0.1-SNAPSHOT</version>

    <!-- Inherit defaults from Spring Boot -->
    <parent>
        <groupId>org.springframework.boot</groupId>
        <artifactId>spring-boot-starter-parent</artifactId>
        <version>1.4.0.BUILD-SNAPSHOT</version>
    </parent>

    <!-- Add typical dependencies for a web application -->
    <dependencies>
        <dependency>
            <groupId>org.springframework.boot</groupId>
            <artifactId>spring-boot-starter-web</artifactId>
        </dependency>
    </dependencies>

    <!-- Package as an executable jar -->
    <build>
        <plugins>
            <plugin>
                <groupId>org.springframework.boot</groupId>
                <artifactId>spring-boot-maven-plugin</artifactId>
            </plugin>
        </plugins>
    </build>

    <!-- Add Spring repositories -->
    <!-- (you don't need this if you are using a .RELEASE version) -->
    <repositories>
        <repository>
            <id>spring-snapshots</id>
            <url>http://repo.spring.io/snapshot</url>
            <snapshots><enabled>true</enabled></snapshots>
        </repository>
        <repository>
            <id>spring-milestones</id>
            <url>http://repo.spring.io/milestone</url>
        </repository>
    </repositories>
    <pluginRepositories>
        <pluginRepository>
            <id>spring-snapshots</id>
            <url>http://repo.spring.io/snapshot</url>
        </pluginRepository>
        <pluginRepository>
            <id>spring-milestones</id>
            <url>http://repo.spring.io/milestone</url>
        </pluginRepository>
    </pluginRepositories>
</project>
```

## 通过 Gradle 安装

下面是一个典型的`build.gradle`文件：

```text
buildscript {
    repositories {
        jcenter()
        maven { url "http://repo.spring.io/snapshot" }
        maven { url "http://repo.spring.io/milestone" }
    }
    dependencies {
        classpath("org.springframework.boot:spring-boot-gradle-plugin:1.4.0.BUILD-SNAPSHOT")
    }
}

apply plugin: 'java'
apply plugin: 'spring-boot'

jar {
    baseName = 'myproject'
    version =  '0.0.1-SNAPSHOT'
}

repositories {
    jcenter()
    maven { url "http://repo.spring.io/snapshot" }
    maven { url "http://repo.spring.io/milestone" }
}

dependencies {
    compile("org.springframework.boot:spring-boot-starter-web")
    testCompile("org.springframework.boot:spring-boot-starter-test")
}
```

## 通过 SDKMAN

```shell
curl -s "https://get.sdkman.io" | bash 
source ".sdkman/bin/sdkman-init.sh"
sdk install springboot
spring --version
```
如果你正在为CLI开发新的特性，并想轻松获取刚构建的版本，可以使用以下命令：

```shell
sdk install springboot dev /path/to/spring-boot/spring-boot-cli/target/spring-boot-cli-1.4.0.BUILD-SNAPSHOT-bin/spring-1.4.0.BUILD-SNAPSHOT/
sdk default springboot dev
spring --version
```

## 命令行下使用 Spring

Spring Boot CLI启动脚本为BASH和zsh shells提供完整的命令行实现。你可以在任何shell中source脚本（名称也是spring），或将它放到用户或系统范围内的bash初始化脚本里。
在Debian系统中，系统级的脚本位于/shell-completion/bash下，当新的shell启动时该目录下的所有脚本都会被执行。如果想要手动运行脚本，假如你已经安装了 SDKMAN，可以使用以下命令注册脚本到shell：

```shell
. ~/.sdkman/candidates/springboot/current/shell-completion/bash/spring
```

这将会安装一个名叫dev的本地spring实例，它指向你的目标构建位置，所以每次你重新构建Spring Boot，spring都会更新为最新的。

### Spring CLI示例快速入门

下面是一个相当简单的web应用，你可以用它测试Spring CLI安装是否成功。创建一个名叫`app.groovy`的文件：
```groovy
@RestController
class ThisWillActuallyRun {

    @RequestMapping("/")
    String home() {
        "Hello World!"
    }

}
```
然后只需在shell中运行以下命令：

```shell
spring run app.groovy
```

注：首次运行该应用将会花费一些时间，因为需要下载依赖，后续运行将会快很多。

使用你最喜欢的浏览器打开localhost:8080，然后就可以看到如下输出：
```output
Hello World!
```

## 开发你的第一个Spring Boot应用

我们将使用Java开发一个简单的"Hello World" web应用，以此强调下Spring Boot的一些关键特性。项目采用Maven进行构建，因为大多数IDEs都支持它。
在开始前，你需要打开终端检查下安装的 Java 和 Maven 版本是否可用：
```text
$ java -version
java version "1.7.0_51"
Java(TM) SE Runtime Environment (build 1.7.0_51-b13)
Java HotSpot(TM) 64-Bit Server VM (build 24.51-b03, mixed mode)
```
```text
$ mvn -v
Apache Maven 3.2.3 (33f8c3e1027c3ddde99d3cdebad2656a31e8fdf4; 2014-08-11T13:58:10-07:00)
Maven home: /Users/user/tools/apache-maven-3.1.1
Java version: 1.7.0_51, vendor: Oracle Corporation
```

* 创建目录

```shell
mkdir first
```

* 创建一个 pom.xml 文件

```xml
<?xml version="1.0" encoding="UTF-8"?>
<project xmlns="http://maven.apache.org/POM/4.0.0" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
    xsi:schemaLocation="http://maven.apache.org/POM/4.0.0 http://maven.apache.org/xsd/maven-4.0.0.xsd">
    <modelVersion>4.0.0</modelVersion>

    <groupId>com.example</groupId>
    <artifactId>myproject</artifactId>
    <version>0.0.1-SNAPSHOT</version>

    <parent>
        <groupId>org.springframework.boot</groupId>
        <artifactId>spring-boot-starter-parent</artifactId>
        <version>1.4.1.BUILD-SNAPSHOT</version>
    </parent>

    <!-- Additional lines to be added here... -->

    <!-- (you don't need this if you are using a .RELEASE version) -->
    <repositories>
        <repository>
            <id>spring-snapshots</id>
            <url>http://repo.spring.io/snapshot</url>
            <snapshots><enabled>true</enabled></snapshots>
        </repository>
        <repository>
            <id>spring-milestones</id>
            <url>http://repo.spring.io/milestone</url>
        </repository>
    </repositories>
    <pluginRepositories>
        <pluginRepository>
            <id>spring-snapshots</id>
            <url>http://repo.spring.io/snapshot</url>
        </pluginRepository>
        <pluginRepository>
            <id>spring-milestones</id>
            <url>http://repo.spring.io/milestone</url>
        </pluginRepository>
    </pluginRepositories>
</project>
```

此刻，你可以将该项目导入到IDE中（大多数现代的 Java IDE 都包含对 Maven 的内建支持）。简单起见，我们将继续使用普通的文本编辑器完成该示例。

* 运行测试

```shell
mvn package
```

* 添加 classpath 依赖

Spring Boot 提供很多 "Starters"，用来简化添加 jars 到 classpath 的操作。在 POM 的 parent 节点使用了`spring-boot-starter-parent`，它是一个特殊的starter，提供了有用的Maven默认设置。同时，它也提供一个dependency-management节点，这样对于期望（”blessed“）的依赖就可以省略version标记了。

其他`Starters`只简单提供开发特定类型应用所需的依赖。由于正在开发web应用，我们将添加 spring-boot-starter-web 依赖-但在此之前，让我们先看下目前的依赖：

```shell
mvn dependency:tree
```
编辑 `pom.xml`，并在 parent 节点下添加 spring-boot-starter-web 依赖：
```xml
<dependencies>
    <dependency>
        <groupId>org.springframework.boot</groupId>
        <artifactId>spring-boot-starter-web</artifactId>
    </dependency>
</dependencies>
```

如果再次运行`mvn dependency:tree`，你将看到现在多了一些其他依赖，包括Tomcat web服务器和Spring Boot自身。

* 编写代码

为了完成应用程序，我们需要创建一个单独的Java文件。Maven默认会编译src/main/java下的源码，所以你需要创建那样的文件结构，并添加一个名为src/main/java/Example.java的文件：
```java
import org.springframework.boot.*;
import org.springframework.boot.autoconfigure.*;
import org.springframework.stereotype.*;
import org.springframework.web.bind.annotation.*;

@RestController
@EnableAutoConfiguration
public class Example {

    @RequestMapping("/")
    String home() {
        return "Hello World!";
    }

    public static void main(String[] args) throws Exception {
        SpringApplication.run(Example.class, args);
    }

}
```

* `@RestController`和`@RequestMapping`注解

`@RestController`和`@RequestMapping`是Spring MVC中的注解

类 `Example` 上使用的第一个注解是`@RestController`，这被称为构造型（stereotype）注解。它为阅读代码的人提供暗示（这是一个支持REST的控制器），对于 Spring，该类扮演了一个特殊角色。在本示例中，我们的类是一个web `@Controller`，所以当web请求进来时，Spring会考虑是否使用它来处理。

`@RequestMapping`注解提供路由信息，它告诉Spring任何来自"/"路径的HTTP请求都应该被映射到home方法。`@RestController`注解告诉 Spring 以字符串的形式渲染结果，并直接返回给调用者。

* `@EnableAutoConfiguration`注解

这个注解告诉Spring Boot根据添加的jar依赖猜测你想如何配置Spring。由于`spring-boot-starter-web`添加了`Tomcat`和`Spring MVC`，所以`auto-configuration`将假定你正在开发一个`web`应用，并对`Spring`进行相应地设置。

`Starters`和`Auto-Configuration`：`Auto-configuration`设计成可以跟"Starters"一起很好的使用，但这两个概念没有直接的联系。你可以自由地挑选`starters`以外的`jar`依赖，Spring Boot仍会尽最大努力去自动配置你的应用。

* main 方法

应用程序的最后部分是main方法，这是一个标准的方法，它遵循Java对于一个应用程序入口点的约定。我们的main方法通过调用run，将业务委托给了Spring Boot的SpringApplication类。SpringApplication将引导我们的应用，启动Spring，相应地启动被自动配置的Tomcat web服务器。我们需要将`Example.class`作为参数传递给run方法，以此告诉`SpringApplication`谁是主要的Spring组件，并传递args数组以暴露所有的命令行参数。

* 运行示例

到此，示例应用可以工作了。由于使用了spring-boot-starter-parent POM，这样我们就有了一个非常有用的`run`目标来启动程序。启动应用：

```shell
mvn spring-boot:run
```

使用浏览器打开 `localhost:8080` 可以查看输出。

## 创建可执行 jar

让我们通过创建一个完全自包含，并可以在生产环境运行的可执行jar来结束示例吧！可执行jars（有时被称为胖jars "fat jars"）是包含编译后的类及代码运行所需依赖jar的存档。

为了创建可执行的jar，我们需要将spring-boot-maven-plugin添加到pom.xml中，在dependencies节点后面插入以下内容：
```xml
<build>
    <plugins>
        <plugin>
            <groupId>org.springframework.boot</groupId>
            <artifactId>spring-boot-maven-plugin</artifactId>
        </plugin>
    </plugins>
</build>
```

从命令行运行`mvn package`，如果查看target目录，你应该可以看到myproject-0.0.1-SNAPSHOT.jar，该文件大概有10Mb。想查看内部结构，可以运行`jar tvf target/myproject-0.0.1-SNAPSHOT.jar`

在该目录下，你应该还能看到一个很小的名为myproject-0.0.1-SNAPSHOT.jar.original的文件，这是在Spring Boot重新打包前，Maven创建的原始jar文件。

可以使用`java -jar target/myproject-0.0.1-SNAPSHOT.jar`命令运行该应用程序。

## 整合 PHP 示例

https://github.com/KeoZmy/SpringCloudDemo

Spring Cloud Netflix Sidecar 受 Netflix Prana 启发，它包含一个简单的HTTP API去获取所有注册的实例信息(包括host和port信息)。
也可以通过依赖 Eureka 的嵌入式 Zuul 代理器代理服务调用。
The Spring Cloud Config Server 可以通过 host 查找 或Zuul代理直接进入，非JVM应用程序提供健康检查实现即可让 Sidecar 向 eureka 同步应用程序 up 还是 down。

Sidecar的原理就是侦听该应用所运行的端口，然后检测该程序的运行状态。

* 创建 PHP 项目

```text
.
├── health.json
├── index.php
```
health.json 就是非JVM应用程序提供模仿SpringBoot健康检查接口的可访问的uri. 它应该返回一个json文档类似如下:
```json
{"status":"UP"}
```

部署地址假设是 http://localhost/php/

* 创建 Spring Boot 项目 EurekaApplication

1. 使用 Maven 构建项目

- 通过SPRING INITIALIZR工具产生基础项目
	访问：http://start.spring.io/
	选择构建工具Maven Project、Spring Boot版本1.3.2以及一些工程基本信息.

- 点击Generate Project下载项目压缩包
- 解压项目包，并用IDE以Maven项目导入

Spring Boot的基础结构共三个文件（具体路径根据用户生成项目时填写的Group所有差异）：

- src/main/java
程序入口：EurekaApplication

- src/main/resources
配置文件：application.properties

- src/test/
测试入口：EurekaApplicationTests

由于目前该项目未配合任何数据访问或Web模块，程序会在加载完Spring之后结束运行。

修改pom文件如下：
```xml
<?xml version="1.0" encoding="UTF-8"?>
<project xmlns="http://maven.apache.org/POM/4.0.0" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
		 xsi:schemaLocation="http://maven.apache.org/POM/4.0.0 http://maven.apache.org/xsd/maven-4.0.0.xsd">
	<modelVersion>4.0.0</modelVersion>

	<groupId>org.test</groupId>
	<artifactId>eureka</artifactId>
	<version>0.0.1-SNAPSHOT</version>
	<packaging>jar</packaging>

	<name>eureka</name>
	<description>Demo project for Spring Boot</description>

	<parent>
		<groupId>org.springframework.boot</groupId>
		<artifactId>spring-boot-starter-parent</artifactId>
		<version>1.2.5.RELEASE</version>
		<relativePath/> <!-- lookup parent from repository -->
	</parent>

	<properties>
		<project.build.sourceEncoding>UTF-8</project.build.sourceEncoding>
		<java.version>1.8</java.version>
	</properties>

	<dependencies>
		<dependency>
			<groupId>org.springframework.boot</groupId>
			<artifactId>spring-boot-starter-actuator</artifactId>
		</dependency>
		<dependency>
			<groupId>org.springframework.cloud</groupId>
			<artifactId>spring-cloud-starter-eureka-server</artifactId>
		</dependency>
		<dependency>
			<groupId>org.springframework.boot</groupId>
			<artifactId>spring-boot-starter-web</artifactId>
		</dependency>

		<dependency>
			<groupId>org.springframework.boot</groupId>
			<artifactId>spring-boot-starter-test</artifactId>
			<scope>test</scope>
		</dependency>
		<dependency>
			<groupId>org.springframework.boot</groupId>
			<artifactId>spring-boot-test</artifactId>
			<version>RELEASE</version>
		</dependency>
		<dependency>
			<groupId>org.springframework</groupId>
			<artifactId>spring-test</artifactId>
			<version>RELEASE</version>
		</dependency>
	</dependencies>

	<dependencyManagement>
		<dependencies>
			<dependency>
				<groupId>org.springframework.cloud</groupId>
				<artifactId>spring-cloud-starter-parent</artifactId>
				<version>Angel.SR3</version>
				<type>pom</type>
				<scope>import</scope>
			</dependency>
		</dependencies>
	</dependencyManagement>

	<build>
		<plugins>
			<plugin>
				<groupId>org.springframework.boot</groupId>
				<artifactId>spring-boot-maven-plugin</artifactId>
			</plugin>
		</plugins>
	</build>


</project>
```

修改`application.properties`配置文件
```text
server.port=8761
eureka.client.fetch-registry=false
eureka.client.register-with-eureka=false
```

最后，我们在 EurekaApplication 中添加注解 @EnableEurekaServer，开启服务的注册中心：
```java
package com.keozhao;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.cloud.netflix.eureka.server.EnableEurekaServer;

/**
 * 创建注册中心
 */
@SpringBootApplication
@EnableEurekaServer
public class EurekaApplication {

	public static void main(String[] args) {
		SpringApplication.run(EurekaApplication.class, args);
	}
}
```

* 创建 Spring Boot 项目 Configserver


修改pom文件如下
```xml
<?xml version="1.0" encoding="UTF-8"?>
<project xmlns="http://maven.apache.org/POM/4.0.0" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
		 xsi:schemaLocation="http://maven.apache.org/POM/4.0.0 http://maven.apache.org/xsd/maven-4.0.0.xsd">
	<modelVersion>4.0.0</modelVersion>

	<groupId>org.test</groupId>
	<artifactId>configserver</artifactId>
	<version>0.0.1-SNAPSHOT</version>
	<packaging>jar</packaging>

	<name>configserver</name>
	<description>Demo project for Spring Boot</description>

	<parent>
		<groupId>org.springframework.boot</groupId>
		<artifactId>spring-boot-starter-parent</artifactId>
		<version>1.2.5.RELEASE</version>
		<relativePath/> <!-- lookup parent from repository -->
	</parent>

	<properties>
		<project.build.sourceEncoding>UTF-8</project.build.sourceEncoding>
		<java.version>1.8</java.version>
	</properties>

	<dependencies>
		<dependency>
			<groupId>org.springframework.boot</groupId>
			<artifactId>spring-boot-starter-actuator</artifactId>
		</dependency>
		<dependency>
			<groupId>org.springframework.cloud</groupId>
			<artifactId>spring-cloud-config-server</artifactId>
			<exclusions>
				<exclusion>
					<groupId>org.springframework.security</groupId>
					<artifactId>spring-security-rsa</artifactId>
				</exclusion>
			</exclusions>
		</dependency>
		<dependency>
			<groupId>org.springframework.boot</groupId>
			<artifactId>spring-boot-starter-web</artifactId>
		</dependency>
		<dependency>
			<groupId>org.springframework.cloud</groupId>
			<artifactId>spring-cloud-starter-eureka</artifactId>
		</dependency>

		<dependency>
			<groupId>org.springframework.boot</groupId>
			<artifactId>spring-boot-starter-test</artifactId>
			<scope>test</scope>
		</dependency>
		<dependency>
			<groupId>org.springframework.boot</groupId>
			<artifactId>spring-boot-test</artifactId>
			<version>RELEASE</version>
		</dependency>
		<dependency>
			<groupId>org.springframework</groupId>
			<artifactId>spring-test</artifactId>
			<version>RELEASE</version>
		</dependency>
	</dependencies>

	<dependencyManagement>
		<dependencies>
			<dependency>
				<groupId>org.springframework.cloud</groupId>
				<artifactId>spring-cloud-starter-parent</artifactId>
				<version>Angel.SR3</version>
				<type>pom</type>
				<scope>import</scope>
			</dependency>
		</dependencies>
	</dependencyManagement>

	<build>
		<plugins>
			<plugin>
				<groupId>org.springframework.boot</groupId>
				<artifactId>spring-boot-maven-plugin</artifactId>
			</plugin>
		</plugins>
	</build>


</project>
```

修改 application.properties 配置文件:
```text
server.port=8888
spring.cloud.config.server.git.uri=https://github.com/spencergibb/oscon-config-repo
```

这里还有一个配置文件bootstrap.properties：
```text
server.port=5678
sidecar.port=80
sidecar.health-uri=http://localhost:${sidecar.port}/php/health.json
```

* sidecar for php

编写一个为 PHP 应用对接的 sidecar 程序，pom.xml：
```xml
<?xml version="1.0" encoding="UTF-8"?>
<project xmlns="http://maven.apache.org/POM/4.0.0" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
		 xsi:schemaLocation="http://maven.apache.org/POM/4.0.0 http://maven.apache.org/xsd/maven-4.0.0.xsd">
	<modelVersion>4.0.0</modelVersion>

	<groupId>org.test</groupId>
	<artifactId>sidecar</artifactId>
	<version>0.0.1-SNAPSHOT</version>
	<packaging>jar</packaging>

	<name>sidecar</name>
	<description>Demo project for Spring Boot</description>

	<parent>
		<groupId>org.springframework.boot</groupId>
		<artifactId>spring-boot-starter-parent</artifactId>
		<version>1.2.5.RELEASE</version>
		<relativePath/> <!-- lookup parent from repository -->
	</parent>

	<properties>
		<project.build.sourceEncoding>UTF-8</project.build.sourceEncoding>
		<java.version>1.8</java.version>
	</properties>

	<dependencies>
		<dependency>
			<groupId>org.springframework.boot</groupId>
			<artifactId>spring-boot-starter-actuator</artifactId>
		</dependency>
		<dependency>
			<groupId>org.springframework.cloud</groupId>
			<artifactId>spring-cloud-starter-config</artifactId>
		</dependency>
		<dependency>
			<groupId>org.springframework.cloud</groupId>
			<artifactId>spring-cloud-starter-eureka</artifactId>
		</dependency>
		<dependency>
			<groupId>org.springframework.boot</groupId>
			<artifactId>spring-boot-starter-web</artifactId>
		</dependency>
		<dependency>
			<groupId>org.springframework.cloud</groupId>
			<artifactId>spring-cloud-netflix-sidecar</artifactId>
		</dependency>

		<dependency>
			<groupId>org.springframework.boot</groupId>
			<artifactId>spring-boot-starter-test</artifactId>
			<scope>test</scope>
		</dependency>
		<dependency>
			<groupId>org.springframework</groupId>
			<artifactId>spring-test</artifactId>
			<version>RELEASE</version>
		</dependency>
		<dependency>
			<groupId>org.springframework.boot</groupId>
			<artifactId>spring-boot-test</artifactId>
			<version>RELEASE</version>
		</dependency>
	</dependencies>

	<dependencyManagement>
		<dependencies>
			<dependency>
				<groupId>org.springframework.cloud</groupId>
				<artifactId>spring-cloud-starter-parent</artifactId>
				<version>Angel.SR3</version>
				<type>pom</type>
				<scope>import</scope>
			</dependency>
		</dependencies>
	</dependencyManagement>

	<build>
		<plugins>
			<plugin>
				<groupId>org.springframework.boot</groupId>
				<artifactId>spring-boot-maven-plugin</artifactId>
			</plugin>
		</plugins>
	</build>


</project>
```

在程序中添加注解 `@EnableSidecar`：
```java
package com.kaozhao;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.cloud.netflix.sidecar.EnableSidecar;

@EnableSidecar
@SpringBootApplication
public class SidecarApplication {

	public static void main(String[] args) {
		SpringApplication.run(SidecarApplication.class, args);
	}
}
```

改写`application.properties`
```text
server.port=5678    
sidecar.port=80
sidecar.health-uri=http://localhost:${sidecar.port}/php/health.json
text
