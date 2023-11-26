#  Spring Boot

从 http://start.spring.io 生成初始项目后，下载到本地，然后导入到 Netbeans。

`File | Import Project | From ZIP`

## 
- hystrix 实现了断路器模式
- guava RateLimiter
- sentinel 面向分布式服务架构的高可用流量防护组件
## REST

### 将 Spring Data REST 添加到 Spring Boot 项目

如果您正在构建Spring Boot应用程序，最简单的方法就是开始。这是因为Spring Data REST同时具有启动器以及自动配置。

* Spring Boot配置与Gradle
```test
dependencies {
  ...
  compile("org.springframework.boot:spring-boot-starter-data-rest")
  ...
}
```
* Spring Boot配置与Maven
```xml
<dependencies>
  ...
  <dependency>
    <groupId>org.springframework.boot</groupId>
    <artifactId>spring-boot-starter-data-rest</artifactId>
  </dependency>
  ...
</dependencies>
```

### 将Spring Data REST添加到Gradle项目

要将Spring Data REST添加到基于Gradle的项目中，请将spring-data-rest-webmvc工件添加到编译时依赖项中：

dependencies {
  … other project dependencies
  compile("org.springframework.data:spring-data-rest-webmvc:2.6.6.RELEASE")
}

### 将Spring Data REST添加到Maven项目中

要将Spring Data REST添加到基于Maven的项目中，请将spring-data-rest-webmvc工件添加到编译时依赖项中：

<dependency>
  <groupId>org.springframework.data</groupId>
  <artifactId>spring-data-rest-webmvc</artifactId>
  <version>2.6.6.RELEASE</version>
</dependency>

### 配置Spring Data RES

在现有的Spring MVC应用程序旁边安装Spring Data REST，您需要包含适当的MVC配置。Spring Data REST配置在名为RepositoryRestMvcConfiguration的类中定义，该类可以导入到应用程序配置中。

### Spring Data REST的基本设置

* 默认情况下哪些存储库被暴露？

Spring Data REST使用RepositoryDetectionStrategy来确定是否将资源库导出为REST资源。

* 更改基本URI

默认情况下，Spring Data REST以根URI“/”提供REST资源。有多种方法来改变基本路径。

使用Spring Boot 1.2+，所有需要的是application.properties中的一个属性：
`spring.data.rest.basePath=/api`

### Repository资源

* 基本面

Spring Data REST的核心功能是导出Spring Data repositories的资源。因此，查看和潜在调整的核心工件可以自定义导出工作的方式是存储库接口。假设以下存储库接口：

`public interface OrderRepository extends CrudRepository<Order, Long> { }`

对于此存储库，Spring Data REST在`/orders`显示集合资源。该路径是从被管理的域类的未资本化，多元化，简单的类名派生的。它还为URI模板`/orders/{id}`下的存储库管理的每个项目公开了一个项目资源。

默认情况下，与这些资源交互的HTTP方法映射到 CrudRepository 的相应方法。

* 资源可发现性

HATEOAS （超媒体作为应用状态的引擎(Hypermedia as the engine of application state)）的核心原则是通过发布指向可用资源的链接来发现资源。有几个竞争的事实上的标准如何表示JSON中的链接。默认情况下，Spring Data REST使用HAL来呈现响应。HAL定义要包含在返回文档的属性中的链接。



资源发现从应用程序的顶层开始。通过向部署了Spring Data REST应用程序的根URL发出请求，客户端可以从返回的JSON对象中提取一组链接，这些链接表示客户端可用的下一级资源。

例如，要发现应用程序根目录中有哪些资源可用，请向根URL发出HTTP GET：
```text
curl -v http://localhost:8080/

< HTTP/1.1 200 OK
< Content-Type: application/hal+json

{ "_links" : {
    "orders" : {
      "href" : "http://localhost:8080/orders"
    },
    "profile" : {
      "href" : "http://localhost:8080/api/alps"
    }
  }
}
```
结果文档的属性本身是由代表HAL中指定的嵌套链接对象的关系类型的键组成的。
