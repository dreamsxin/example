# Eureka

Eureka 是 Netflix 开发的服务发现框架，本身是一个基于REST的服务，属于中间层服务，主要用于实现微服务架构中的微服务注册中心。
Netflix 公司的产品还有：Eureka、Zuul、Feign、Ribbon等。

## 搭建 eureka-server
在父工程的 `pom.xml` 中引入 `Spring Cloud`依赖（因为大部分服务都会用到Spring Cloud中的组件）。

```xml
<!--版本锁定-->
<properties>
    <spring.cloud-version>Hoxton.SR12</spring.cloud-version>
</properties>

<dependencyManagement>
    <dependencies>
        <!--spring Cloud-->
        <dependency>
            <groupId>org.springframework.cloud</groupId>
            <artifactId>spring-cloud-dependencies</artifactId>
            <version>${spring.cloud-version}</version>
            <type>pom</type>
            <scope>import</scope>
        </dependency>
    </dependencies>
</dependencyManagement>
```
### 创建模块
新建模块 `eureka-server`，是一个独立的微服务，不需要加入其他业务功能。
`pom.xml`加入：
```xml
<!--eureka-server依赖-->
<dependency>
    <groupId>org.springframework.cloud</groupId>
    <artifactId>spring-cloud-starter-netflix-eureka-server</artifactId>
</dependency>
```
`application.yml` 配置：
```yml
server:
  port: 10086

spring:
  application:
    name: eureka-server
eureka:
  instance:
    hostname: 127.0.0.1 # 服务地址
    
  client:
    #是否向注册中心注册自己，默认值：true。Eureka服务端是不需要再注册自己的。
    register-with-eureka: false
    #是否从Eureka获取注册信息，默认值：true。Eureka服务端是不需要的。
    fetch-registry: false
```
编写启动类，开启 Eureka 服务：
```java
@SpringBootApplication
//开启Eureka注册中心功能
@EnableEurekaServer
public class EurekaServerApplication {

    public static void main(String[] args) {
        SpringApplication.run(EurekaServerApplication.class,args);
    }
}
```

### 访问 eureka 服务
启动微服务，然后在浏览器访问：http://127.0.0.1:10086

### 配置双节点高可用 eureka 服务
`pom.xml` 添加依赖
```xml
<dependency>
    <groupId>org.springframework.cloud</groupId>
    <artifactId>spring-cloud-starter-netflix-eureka-client</artifactId>
</dependency>
```
在 Eureka Server 集群时，需要部署多个节点，则需要同一个项目，使用不同的配置。
`application.yml` 配置：
```yml
# 多环境配置
spring:
  profiles:
    active: eureka-server1
  application:
    name: eureka-server
```
`application-eureka-server1.yml`（启动参数添加 --spring.profiles.active=eureka-server1）
```yml
server:
  port: 10086

spring:
  application:
    name: eureka-server
eureka:
  instance:
    hostname: 127.0.0.1
    
  client:
    service-url:
      defaultZone: http://127.0.0.2:10086/eureka
```

`application-eureka-server2.yml`（启动参数添加 --spring.profiles.active=eureka-server2）
```yml
server:
  port: 10086

spring:
  application:
    name: eureka-server
eureka:
  instance:
    hostname: 127.0.0.2
    
  client:
    service-url:
      defaultZone: http://127.0.0.1:10086/eureka
```
修改启动类：
```java
package com.xcbeyond.springcloud.eurekacluster;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.cloud.client.discovery.EnableDiscoveryClient;
import org.springframework.cloud.netflix.eureka.server.EnableEurekaServer;

/**
 * Eureka集群启动类
 */
@SpringBootApplication
@EnableEurekaServer
//开启作为Eureka Server的客户端的支持
@EnableDiscoveryClient
public class EurekaClusterApplication {
	public static void main(String[] args) {
		SpringApplication.run(EurekaClusterApplication.class, args);
	}
}
```

## 新建微服务 user-service
下面，我们将 `user-service` 注册到 `eureka-server` 中去。
新建模块 `user-service`，修改 `pom.xml`：
```xml
<!--eureka客户端依赖-->
<dependency>
    <groupId>org.springframework.cloud</groupId>
    <artifactId>spring-cloud-starter-netflix-eureka-client</artifactId>
</dependency>
```
`application.yml`
```yml
spring:
  application:
    # 服务名称，如果不添加，服务端默认显示的服务名是UNKNOWN
    name: user-service

eureka:
  client:
    service-url:
      # EurekaServer地址，在eureka server的实现中存在一个web请求过滤器，其url模式就是【/eureka/*】。
      defaultZone: http://127.0.0.1:10086/eureka
```
启动类：
```java
package cn.springcloud.book;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.cloud.client.discovery.EnableDiscoveryClient;

@SpringBootApplication
@EnableDiscoveryClient
public class UserServiceApplication {
	
    public static void main(String[] args) {
        SpringApplication.run(UserServiceApplication.class, args);
    }
}
```
### 启动 user-service 测试
可以新建多个配置，不同端口

## 新建微服务 order-service 测试服务发现
`order-service` 向 `eureka-server` 拉取 `user-service` 的信息，实现服务发现。
添加依赖 `pom.xml`
```xml
<dependency>
    <groupId>org.springframework.cloud</groupId>
    <artifactId>spring-cloud-starter-netflix-eureka-client</artifactId>
</dependency>
```

`application.yml`：
```xml
spring:
  application:
    # 服务名称
    name: orders-ervice
eureka:
  client:
    service-url: 
      # 注册中心地址 多个地址逗号隔开
      defaultZone: http://localhost:10086/eureka
```

## 服务发现，负载均衡
最后，我们要去注册中心拉取user-service服务的实例列表，并且实现负载均衡。

不过这些动作不用我们去做，只需要添加一些注解即可。
```java
import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.cloud.client.discovery.EnableDiscoveryClient;
import org.springframework.cloud.client.loadbalancer.LoadBalanced;
import org.springframework.context.annotation.Bean;
import org.springframework.web.client.RestTemplate;

@SpringBootApplication
@EnableDiscoveryClient
public class OrderServiceApplication {

    @Bean
    @LoadBalanced
    public RestTemplate restTemplate() {
        RestTemplate restTemplate = new RestTemplate();
        return restTemplate;
    }
	
    public static void main(String[] args) {
        SpringApplication.run(HelloServiceApplication.class, args);
    }
   
}
// 在 controller 中使用
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RestController;
import org.springframework.web.client.RestTemplate;

@RestController
public class TestController {
	
    @Autowired
    private RestTemplate restTemplate;

	@GetMapping("/test")
	public String test(Integer a, Integer b) {
		String result = restTemplate
				.getForObject("http://user-service/test", String.class);
		System.out.println(result);
		return result;
	}
}
```
