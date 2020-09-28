# Vert.x

Eclipse软件基金会顶级java开源项目之一，它基于netty的、运行在jvm之上的、支持多种编程语言的高性能异步、非阻塞、响应式全栈java web框架。

https://vertx.io/
https://github.com/VertxChina/vertx-translation-chinese

## 概要

- 采用单一组件结构设计，即 Verticle，所有业务功能都使用 Verticle 这种单一的组件编程完成，克服以往应用框架和平台包含众多类型的组件模式,使得开发人员能极快适应Vert.x编程，加快项目的开发速度。
- 所有的Verticle组件都是完全解耦合的，任何组件之间不能直接调用，只能通过在Vert.x的事件总线上发送事件来完成，彻底解决了传统应用系统中管理组件间相互依赖的复杂性,最终使得Vert. x应用编程极其简单高效。
- 使用单线程事件驱动的异步工作模式，编写Vert.x组件时，不需要考虑复杂的多线程编程难题，并不需要关注线程之间的调用、同步、加锁等繁琐处理编程,简化了编程代码，提高了编程效率。
- 通过提供一整套的异步编程API实现异步编程模型，在Vert. x中所有的请求处理都是通过注册事件监听处理器机制完成的。编程TCP处理服务器Verticle，通过注册TCPSocket的数据到达事件监听器，实现数据到达后的回调处理，而不是采用一直等待数据读取的阻塞模式,实现的是非阻塞的异步工作模式。
- 核心运行机制是事件循环，当Vert.x实例启动后，Vert.x框架在每个CPU的内核创建一个事件循环线程。此事件循环线程永不结束，它不断监听出现的各种事件，如事件总线的事件到达WebSocket上的数据接收，HTTP上的请求到达HTTP响应结束，定时器触发等等，并把事件分发到注册了监听此事件的Verticle，再继续监听其他的事件，如此反复直到Vert.x实例停止。

## Verticles 是什么

Vert.x 提供了一个简单的，可伸缩的，类似actor的部署和并发模型，你可以使用它来编写自己的代码。

3种不同类型的verticle：

* Standard Verticles

这是最常见和通用的类型-他们总是使用一个 event loop thread(事件循环线程) 执行。

我们将会在接下来的章节继续讨论。

* Worker Verticles

使用来自 worker pool(工作者线程池) 中的线程运行。一个实例永远不会被多个线程同时执行。

* Multi-threaded worker verticles

### 使用 vert.x 运行命令来运行 verticles

```shell
# Run a JavaScript verticle 
vertx run my_verticle.js

# Run a Ruby verticle
vertx run a_n_other_verticle.rb

# Run a Groovy script verticle, clustered
vertx run FooVerticle.groovy -cluster
```
你甚至可以运行Java源代码而无需先编译它们！
```shell
vertx run SomeJavaSourceFile.java
```

## 与 spring的对比

- spring是单体架构设计
- vert.x面向分布式设计
在vert.x的概念中，没有MVC，没有AOP，没有ORM。

二者的生态框架对比图如下

<table>
<thead>
<tr>
<th style="text-align:center">vert.x</th>
<th style="text-align:center">spring</th>
</tr>
</thead>
<tbody>
<tr>
<td style="text-align:center">Vert.x Core</td>
<td style="text-align:center">Spring Framework</td>
</tr>
<tr>
<td style="text-align:center">Vert.x Web</td>
<td style="text-align:center">SpringBoot</td>
</tr>
<tr>
<td style="text-align:center">Vert.x Data Access</td>
<td style="text-align:center">Spring Data Jpa</td>
</tr>
<tr>
<td style="text-align:center">Vert.x Reactive</td>
<td style="text-align:center">Project Reactor</td>
</tr>
<tr>
<td style="text-align:center">Vert.x Microservices</td>
<td style="text-align:center">Spring Cloud</td>
</tr>
<tr>
<td style="text-align:center">Vert.x Authentication and Authorisation</td>
<td style="text-align:center">Spring Security</td>
</tr>
<tr>
<td style="text-align:center">Vert.x MQTT</td>
<td style="text-align:center"></td>
</tr>
<tr>
<td style="text-align:center">Vert.x Messaging</td>
<td style="text-align:center">Spring AMQP</td>
</tr>
<tr>
<td style="text-align:center">Vert.x Devops</td>
<td style="text-align:center">Spring  Devops</td>
</tr>
</tbody>
</table>

## 第一个vert.x

* 目录结构
```text
├── src
│   ├── main
│   │   ├── java
│   │   └── resources
│   └── test
│       └── java
├── build.gradle
├── settings.gradle
```

修改`build.gradle`为：
```gradle
group 'io.example'
version '1.0-SNAPSHOT'

apply plugin: 'java'

sourceCompatibility = 1.8
targetCompatibility = 1.8//新增

repositories {
    mavenCentral()
}

dependencies {
    compile 'io.vertx:vertx-core:3.4.2'
    testCompile group: 'junit', name: 'junit', version: '4.12'
}

jar {
    manifest {
        attributes "Main-Class": "io.example.Main"
    }

    from {
        configurations.compile.collect { it.isDirectory() ? it : zipTree(it) }
    }
}
```

* 在`src/main/java/io/example`目录下新建两个java文件：
`Main.java`
```java
package io.example;

import io.vertx.core.Vertx;

public class Main {
    public static void main(String[] args){
        Vertx vertx = Vertx.vertx();

        vertx.deployVerticle(MyFirstVerticle.class.getName());
    }
}
```

`MyFirstVerticle.java`
```java
package io.example;

import io.vertx.core.AbstractVerticle;

public class MyFirstVerticle extends AbstractVerticle {
    public void start() {
        vertx.createHttpServer().requestHandler(req -> {
            req.response()
                    .putHeader("content-type", "text/plain")
                    .end("Hello World!");
        }).listen(8080);
    }
}
```

* 编译
```shell
gradle build
```

* 运行
`java -jar VertxExample-1.0-SNAPSHOT.jar`

## 启动器

使用`Launcher`来替代`Main`类，这也是官方推荐的方式，修改`build.gradle`如下：

```gradle
group 'io.example'
version '1.0-SNAPSHOT'

apply plugin: 'java'

sourceCompatibility = 1.8
targetCompatibility = 1.8

repositories {
    mavenCentral()
}

dependencies {
    compile 'io.vertx:vertx-core:3.4.2'
    testCompile group: 'junit', name: 'junit', version: '4.12'
}

jar {
    manifest {
        attributes "Main-Class": "io.vertx.core.Launcher",//改为Launcher
                "Main-Verticle": "io.example.MainVerticle"//新增Main Verticle属性，对应 MainVerticle 类，来替换 Main 类
   }

    from {
        configurations.compile.collect { it.isDirectory() ? it : zipTree(it) }
    }
}
```

`MainVerticle.java`：
```java
package io.example;

import io.vertx.core.AbstractVerticle;

public class MainVerticle extends AbstractVerticle {
    public void start() {
        vertx.deployVerticle(MyFirstVerticle.class.getName());
    }
}
```
