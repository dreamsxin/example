## Dubbo 节点角色说明

- Provider	暴露服务的服务提供方
- Consumer	调用远程服务的服务消费方
- Registry	服务注册与发现的注册中心
- Monitor	统计服务的调用次数和调用时间的监控中心
- Container	服务运行容器

## 发布-订阅的过程

1. 启动容器，加载，运行服务提供者。
2. 服务提供者在启动时，在注册中心发布注册自己提供的服务。
3. 服务消费者在启动时，在注册中心订阅自己所需的服务。
4. 注册中心返回服务提供者地址列表给消费者，如果有变更，注册中心将基于长连接推送变更数据给消费者。
5. 服务消费者，从提供者地址列表中，基于软负载均衡算法，选一台提供者进行调用，如果调用失败，再选另一台调用。
6. 服务消费者和提供者，在内存中累计调用次数和调用时间，定时每分钟发送一次统计数据到监控中心。

## dubbo框架级限流
### 客户端限流

信号量限流 （通过统计的方式）
连接数限流 (socket->tcp)

### 服务端限流

线程池限流 (隔离手段)
信号量限流 (非隔离手段)
接收数限流 (socket->tcp)

### 线程池设置
多线程并发操作一定离不开线程池，Dubbo自身提供了支持了四种线程池类型支持。生产者<dubbo:protocol>标签中可配置线程池关键参数，线程池类型、阻塞队列大小、核心线程数量等，通过配置生产端的线程池数量可以在一定程度上起到限流的效果。

## 编写服务端
首先，我们先把服务端的接口写好，因为其实 dubbo 的作用简单来说就是给消费端提供接口。

接口定义
```java
/**
 * xml方式服务提供者接口
 */
public interface ProviderService {

    String SayHello(String word);
}
```
这个接口非常简单，只是包含一个 SayHello 的方法。

接着，定义它的实现类。
```java
/**
 * xml方式服务提供者实现类
 */
public class ProviderServiceImpl implements ProviderService{

    public String SayHello(String word) {
        return word;
    }
}
```
这样我们就把我们的接口写好了，那么我们应该怎么将我们的服务暴露出去呢？

导入 maven 依赖
```java
<?xml version="1.0" encoding="UTF-8"?>
<project xmlns="http://maven.apache.org/POM/4.0.0"
         xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
         xsi:schemaLocation="http://maven.apache.org/POM/4.0.0 http://maven.apache.org/xsd/maven-4.0.0.xsd">
    <modelVersion>4.0.0</modelVersion>

    <groupId>com.ouyangsihai</groupId>
    <artifactId>dubbo-provider</artifactId>
    <version>1.0-SNAPSHOT</version>

    <dependencies>
        <dependency>
            <groupId>junit</groupId>
            <artifactId>junit</artifactId>
            <version>3.8.1</version>
            <scope>test</scope>
        </dependency>
        <!-- https://mvnrepository.com/artifact/com.alibaba/dubbo -->
        <dependency>
            <groupId>com.alibaba</groupId>
            <artifactId>dubbo</artifactId>
            <version>2.6.6</version>
        </dependency>
        <dependency>
            <groupId>org.apache.zookeeper</groupId>
            <artifactId>zookeeper</artifactId>
            <version>3.4.10</version>
        </dependency>
        <dependency>
            <groupId>com.101tec</groupId>
            <artifactId>zkclient</artifactId>
            <version>0.5</version>
        </dependency>
        <dependency>
            <groupId>io.netty</groupId>
            <artifactId>netty-all</artifactId>
            <version>4.1.32.Final</version>
        </dependency>
        <dependency>
            <groupId>org.apache.curator</groupId>
            <artifactId>curator-framework</artifactId>
            <version>2.8.0</version>
        </dependency>
        <dependency>
            <groupId>org.apache.curator</groupId>
            <artifactId>curator-recipes</artifactId>
            <version>2.8.0</version>
        </dependency>

    </dependencies>
</project>
```
这里使用的 dubbo 的版本是 2.6.6 ，需要注意的是，如果你只导入 dubbo 的包的时候是会报错的，找不到 netty(异步事件驱动的网络应用框架) 和 curator（分布式锁） 的依赖，所以，在这里我们需要把这两个的依赖加上，就不会报错了。

另外，这里我们使用 zookeeper 作为注册中心。

到目前为止，dubbo 需要的环境就已经可以了，下面，我们就把上面刚刚定义的接口暴露出去。

暴露接口（xml 配置方法）
首先，我们在我们项目的 resource 目录下创建 META-INF.spring 包，然后再创建 provider.xml 文件名字可以任取。

```xml
<?xml version="1.0" encoding="UTF-8"?>
<beans xmlns="http://www.springframework.org/schema/beans"
       xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
       xmlns:dubbo="http://code.alibabatech.com/schema/dubbo"
       xsi:schemaLocation="http://www.springframework.org/schema/beans        http://www.springframework.org/schema/beans/spring-beans.xsd        http://code.alibabatech.com/schema/dubbo        http://code.alibabatech.com/schema/dubbo/dubbo.xsd">

    <!--当前项目在整个分布式架构里面的唯一名称，计算依赖关系的标签-->
    <dubbo:application name="provider" owner="sihai">
        <dubbo:parameter key="qos.enable" value="true"/>
        <dubbo:parameter key="qos.accept.foreign.ip" value="false"/>
        <dubbo:parameter key="qos.port" value="55555"/>
    </dubbo:application>

    <dubbo:monitor protocol="registry"/>

    <!--dubbo这个服务所要暴露的服务地址所对应的注册中心-->
    <!--<dubbo:registry address="N/A"/>-->
    <dubbo:registry address="N/A" />

    <!--当前服务发布所依赖的协议；webserovice、Thrift、Hessain、http-->
    <dubbo:protocol name="dubbo" port="20880"/>

    <!--服务发布的配置，需要暴露的服务接口-->
    <dubbo:service
            interface="com.sihai.dubbo.provider.service.ProviderService"
            ref="providerService"/>

    <!--Bean bean定义-->
    <bean id="providerService" class="com.sihai.dubbo.provider.service.ProviderServiceImpl"/>

</beans>
```
① 上面的文件其实就是类似 spring 的配置文件，而且，dubbo 底层就是 spring。
② 节点：dubbo:application
就是整个项目在分布式架构中的唯一名称，可以在 name 属性中配置，另外还可以配置 owner 字段，表示属于谁。
下面的参数是可以不配置的，这里配置是因为出现了端口的冲突，所以配置。
③ 节点：dubbo:monitor
监控中心配置， 用于配置连接监控中心相关信息，可以不配置，不是必须的参数。
④ 节点：dubbo:registry
配置注册中心的信息，比如，这里我们可以配置 zookeeper 作为我们的注册中心。address 是注册中心的地址，这里我们配置的是 N/A 表示由 dubbo 自动分配地址。或者说是一种直连的方式，不通过注册中心。
⑤ 节点：dubbo:protocol
服务发布的时候 dubbo 依赖什么协议，可以配置 dubbo、webserovice、Thrift、Hessain、http等协议。
⑥ 节点：dubbo:service
这个节点就是我们的重点了，当我们服务发布的时候，我们就是通过这个配置将我们的服务发布出去的。interface 是接口的包路径，ref 是第 ⑦ 点配置的接口的 bean。
⑦ 最后，我们需要像配置 spring 的接口一样，配置接口的 bean。

到这一步，关于服务端的配置就完成了，下面我们通过 main 方法将接口发布出去。

发布接口
```java
package com.sihai.dubbo.provider;

import com.alibaba.dubbo.config.ApplicationConfig;
import com.alibaba.dubbo.config.ProtocolConfig;
import com.alibaba.dubbo.config.RegistryConfig;
import com.alibaba.dubbo.config.ServiceConfig;
import com.alibaba.dubbo.container.Main;
import com.sihai.dubbo.provider.service.ProviderService;
import com.sihai.dubbo.provider.service.ProviderServiceImpl;
import org.springframework.context.support.ClassPathXmlApplicationContext;

import java.io.IOException;

/**
 * xml方式启动
 *
 */
public class App 
{
    public static void main( String[] args ) throws IOException {
        //加载xml配置文件启动
        ClassPathXmlApplicationContext context = new ClassPathXmlApplicationContext("META-INF/spring/provider.xml");
        context.start();
        System.in.read(); // 按任意键退出
    }
}
```
发布接口非常简单，因为 dubbo 底层就是依赖 spring 的，所以，我们只需要通过 ClassPathXmlApplicationContext 拿到我们刚刚配置好的 xml ，然后调用 context.start() 方法就启动了。

**dubbo 暴露的 url**

`dubbo://192.168.234.1:20880/com.sihai.dubbo.provider.service.ProviderService?anyhost=true&application=provider&bean.name=com.sihai.dubbo.provider.service.ProviderService&bind.ip=192.168.234.1&bind.port=20880&dubbo=2.0.2&generic=false&interface=com.sihai.dubbo.provider.service.ProviderService&methods=SayHello&owner=sihai&pid=8412&qos.accept.foreign.ip=false&qos.enable=true&qos.port=55555&side=provider&timestamp=1562077289380`
**分析**

① 首先，在形式上我们发现，其实这么牛逼的 dubbo 也是用类似于 http 的协议发布自己的服务的，只是这里我们用的是 dubbo 协议。
② dubbo://192.168.234.1:20880/com.sihai.dubbo.provider.service.ProviderService
上面这段链接就是 ? 之前的链接，构成：协议://ip:端口/接口。发现是不是也没有什么神秘的。
③ anyhost=true&application=provider&bean.name=com.sihai.dubbo.provider.service.ProviderService&bind.ip=192.168.234.1&bind.port=20880&dubbo=2.0.2&generic=false&interface=com.sihai.dubbo.provider.service.ProviderService&methods=SayHello&owner=sihai&pid=8412&qos.accept.foreign.ip=false&qos.enable=true&qos.port=55555&side=provider&timestamp=1562077289380
? 之后的字符串，分析后你发现，这些都是刚刚在 provider.xml 中配置的字段，然后通过 & 拼接而成的。

## 消费端
上面提到，我们在服务端提供的只是点对点的方式提供服务，并没有使用注册中心，所以，下面的配置也是会有一些不一样的。

消费端环境配置
首先，我们在消费端的 resource 下建立配置文件 consumer.xml。
```xml
<?xml version="1.0" encoding="UTF-8"?>
<beans xmlns="http://www.springframework.org/schema/beans"
       xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
       xmlns:dubbo="http://code.alibabatech.com/schema/dubbo"
       xsi:schemaLocation="http://www.springframework.org/schema/beans        http://www.springframework.org/schema/beans/spring-beans.xsd        http://code.alibabatech.com/schema/dubbo        http://code.alibabatech.com/schema/dubbo/dubbo.xsd">

    <!--当前项目在整个分布式架构里面的唯一名称，计算依赖关系的标签-->
    <dubbo:application name="consumer" owner="sihai"/>

    <!--dubbo这个服务所要暴露的服务地址所对应的注册中心-->
    <!--点对点的方式-->
    <dubbo:registry address="N/A" />
    <!--<dubbo:registry address="zookeeper://localhost:2181" check="false"/>-->

    <!--生成一个远程服务的调用代理-->
    <!--点对点方式-->
    <dubbo:reference id="providerService"
                     interface="com.sihai.dubbo.provider.service.ProviderService"
                     url="dubbo://192.168.234.1:20880/com.sihai.dubbo.provider.service.ProviderService"/>

    <!--<dubbo:reference id="providerService"
                     interface="com.sihai.dubbo.provider.service.ProviderService"/>-->

</beans>
```
分析

① 发现这里的 dubbo:application 和 dubbo:registry 是一致的。
② dubbo:reference ：我们这里采用点对点的方式，所以，需要配置在服务端暴露的 url 。

maven 依赖和服务端一样
```xml
<?xml version="1.0" encoding="UTF-8"?>
<project xmlns="http://maven.apache.org/POM/4.0.0"
         xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
         xsi:schemaLocation="http://maven.apache.org/POM/4.0.0 http://maven.apache.org/xsd/maven-4.0.0.xsd">
    <modelVersion>4.0.0</modelVersion>

    <groupId>com.ouyangsihai</groupId>
    <artifactId>dubbo-consumer</artifactId>
    <version>1.0-SNAPSHOT</version>

    <dependencies>
        <dependency>
            <groupId>com.ouyangsihai</groupId>
            <artifactId>dubbo-provider</artifactId>
            <version>1.0-SNAPSHOT</version>
        </dependency>
        <dependency>
            <groupId>junit</groupId>
            <artifactId>junit</artifactId>
            <version>3.8.1</version>
            <scope>test</scope>
        </dependency>
        <!-- https://mvnrepository.com/artifact/com.alibaba/dubbo -->
        <dependency>
            <groupId>com.alibaba</groupId>
            <artifactId>dubbo</artifactId>
            <version>2.6.6</version>
        </dependency>
        <dependency>
            <groupId>org.apache.zookeeper</groupId>
            <artifactId>zookeeper</artifactId>
            <version>3.4.10</version>
        </dependency>
        <dependency>
            <groupId>com.101tec</groupId>
            <artifactId>zkclient</artifactId>
            <version>0.5</version>
        </dependency>
        <dependency>
            <groupId>io.netty</groupId>
            <artifactId>netty-all</artifactId>
            <version>4.1.32.Final</version>
        </dependency>
        <dependency>
            <groupId>org.apache.curator</groupId>
            <artifactId>curator-framework</artifactId>
            <version>2.8.0</version>
        </dependency>
        <dependency>
            <groupId>org.apache.curator</groupId>
            <artifactId>curator-recipes</artifactId>
            <version>2.8.0</version>
        </dependency>
    </dependencies>
</project>
```
`app.java`
```java
package com.sihai.dubbo.consumer;

import com.alibaba.dubbo.config.ApplicationConfig;
import com.alibaba.dubbo.config.ReferenceConfig;
import com.alibaba.dubbo.config.RegistryConfig;
import com.sihai.dubbo.provider.service.ProviderService;
import org.springframework.context.support.ClassPathXmlApplicationContext;

import java.io.IOException;

/**
 * xml的方式调用
 *
 */
public class App 
{
    public static void main( String[] args ) throws IOException {

        ClassPathXmlApplicationContext context=new ClassPathXmlApplicationContext("consumer.xml");
        context.start();
        ProviderService providerService = (ProviderService) context.getBean("providerService");
        String str = providerService.SayHello("hello");
        System.out.println(str);
        System.in.read();

    }
}
```
## 加入 zookeeper 作为注册中心

服务端中，我们只需要修改 provider.xml 即可
```xml
<?xml version="1.0" encoding="UTF-8"?>
<beans xmlns="http://www.springframework.org/schema/beans"
       xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
       xmlns:dubbo="http://code.alibabatech.com/schema/dubbo"
       xsi:schemaLocation="http://www.springframework.org/schema/beans        http://www.springframework.org/schema/beans/spring-beans.xsd        http://code.alibabatech.com/schema/dubbo        http://code.alibabatech.com/schema/dubbo/dubbo.xsd">

    <!--当前项目在整个分布式架构里面的唯一名称，计算依赖关系的标签-->
    <dubbo:application name="provider" owner="sihai">
        <dubbo:parameter key="qos.enable" value="true"/>
        <dubbo:parameter key="qos.accept.foreign.ip" value="false"/>
        <dubbo:parameter key="qos.port" value="55555"/>
    </dubbo:application>

    <dubbo:monitor protocol="registry"/>

    <!--dubbo这个服务所要暴露的服务地址所对应的注册中心-->
    <!--<dubbo:registry address="N/A"/>-->
    <dubbo:registry address="zookeeper://localhost:2181" check="false"/>

    <!--当前服务发布所依赖的协议；webserovice、Thrift、Hessain、http-->
    <dubbo:protocol name="dubbo" port="20880"/>

    <!--服务发布的配置，需要暴露的服务接口-->
    <dubbo:service
            interface="com.sihai.dubbo.provider.service.ProviderService"
            ref="providerService"/>

    <!--Bean bean定义-->
    <bean id="providerService" class="com.sihai.dubbo.provider.service.ProviderServiceImpl"/>

</beans>
```
重点关注这句话
`<dubbo:registry address="zookeeper://localhost:2181" />`
在 address 中，使用我们的 zookeeper 的地址。

如果是 zookeeper 集群的话，使用下面的方式。

`<dubbo:registry protocol="zookeeper" address="192.168.11.129:2181,192.168.11.137:2181,192.168.11.138:2181"/>`
服务端的配置就好了，其他的跟 入门案例 一样。

消费端
跟服务端一样，在消费端，我们也只需要修改 consumer.xml 即可。
```xml
<?xml version="1.0" encoding="UTF-8"?>
<beans xmlns="http://www.springframework.org/schema/beans"
       xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
       xmlns:dubbo="http://code.alibabatech.com/schema/dubbo"
       xsi:schemaLocation="http://www.springframework.org/schema/beans        http://www.springframework.org/schema/beans/spring-beans.xsd        http://code.alibabatech.com/schema/dubbo        http://code.alibabatech.com/schema/dubbo/dubbo.xsd">

    <!--当前项目在整个分布式架构里面的唯一名称，计算依赖关系的标签-->
    <dubbo:application name="consumer" owner="sihai"/>

    <!--dubbo这个服务所要暴露的服务地址所对应的注册中心-->
    <!--点对点的方式-->
    <!--<dubbo:registry address="N/A" />-->
    <dubbo:registry address="zookeeper://localhost:2181" check="false"/>

    <!--生成一个远程服务的调用代理-->
    <!--点对点方式-->
    <!--<dubbo:reference id="providerService"
                     interface="com.sihai.dubbo.provider.service.ProviderService"
                     url="dubbo://192.168.234.1:20880/com.sihai.dubbo.provider.service.ProviderService"/>-->

    <dubbo:reference id="providerService"
                     interface="com.sihai.dubbo.provider.service.ProviderService"/>

</beans>
```
① 注册中心配置跟服务端一样。

`<dubbo:registry address="zookeeper://localhost:2181"/>`
② dubbo:reference
由于我们这里使用 zookeeper 作为注册中心，所以，跟点对点的方式是不一样的，这里不再需要 dubbo 服务端提供的 url 了，只需要直接引用服务端提供的接口即可。

`<dubbo:reference id="providerService" interface="com.sihai.dubbo.provider.service.ProviderService"/>`

## API配置方式
直接在代码中实例化。
```java
package com.sihai.dubbo.provider;

import com.alibaba.dubbo.config.ApplicationConfig;
import com.alibaba.dubbo.config.ProtocolConfig;
import com.alibaba.dubbo.config.RegistryConfig;
import com.alibaba.dubbo.config.ServiceConfig;
import com.sihai.dubbo.provider.service.ProviderService;
import com.sihai.dubbo.provider.service.ProviderServiceImpl;

import java.io.IOException;

/**
 * Api方式启动
 * api的方式调用不需要其他的配置，只需要下面的代码即可。
 * 但是需要注意，官方建议：
 * Api方式用于测试用例使用，推荐xml的方式
 */
public class AppApi
{
    public static void main( String[] args ) throws IOException {

        // 服务实现
        ProviderService providerService = new ProviderServiceImpl();

        // 当前应用配置
        ApplicationConfig application = new ApplicationConfig();
        application.setName("provider");
        application.setOwner("sihai");

        // 连接注册中心配置
        RegistryConfig registry = new RegistryConfig();
        registry.setAddress("zookeeper://localhost:2181");
//        registry.setUsername("aaa");
//        registry.setPassword("bbb");

        // 服务提供者协议配置
        ProtocolConfig protocol = new ProtocolConfig();
        protocol.setName("dubbo");
        protocol.setPort(20880);
        //protocol.setThreads(200);

        // 注意：ServiceConfig为重对象，内部封装了与注册中心的连接，以及开启服务端口

        // 服务提供者暴露服务配置
        ServiceConfig<ProviderService> service = new ServiceConfig<ProviderService>(); // 此实例很重，封装了与注册中心的连接，请自行缓存，否则可能造成内存和连接泄漏
        service.setApplication(application);
        service.setRegistry(registry); // 多个注册中心可以用setRegistries()
        service.setProtocol(protocol); // 多个协议可以用setProtocols()
        service.setInterface(ProviderService.class);
        service.setRef(providerService);
        service.setVersion("1.0.0");

        // 暴露及注册服务
        service.export();
    }
}
```
同样，消费端我们不需要 consumer.xml 配置文件了，只需要在 main 方法中启动即可。
```java
package com.sihai.dubbo.consumer;

import com.alibaba.dubbo.config.ApplicationConfig;
import com.alibaba.dubbo.config.ReferenceConfig;
import com.alibaba.dubbo.config.RegistryConfig;
import com.sihai.dubbo.provider.service.ProviderService;

/**
 * api的方式调用
 * api的方式调用不需要其他的配置，只需要下面的代码即可。
 * 但是需要注意，官方建议：
 * Api方式用于测试用例使用，推荐xml的方式
 */
public class AppApi {

    public static void main(String[] args) {
        // 当前应用配置
        ApplicationConfig application = new ApplicationConfig();
        application.setName("consumer");
        application.setOwner("sihai");

        // 连接注册中心配置
        RegistryConfig registry = new RegistryConfig();
        registry.setAddress("zookeeper://localhost:2181");

        // 注意：ReferenceConfig为重对象，内部封装了与注册中心的连接，以及与服务提供方的连接

        // 引用远程服务
        ReferenceConfig<ProviderService> reference = new ReferenceConfig<ProviderService>(); // 此实例很重，封装了与注册中心的连接以及与提供者的连接，请自行缓存，否则可能造成内存和连接泄漏
        reference.setApplication(application);
        reference.setRegistry(registry); // 多个注册中心可以用setRegistries()
        reference.setInterface(ProviderService.class);

        // 和本地bean一样使用xxxService
        ProviderService providerService = reference.get(); // 注意：此代理对象内部封装了所有通讯细节，对象较重，请缓存复用
        providerService.SayHello("hello dubbo! I am sihai!");
    }
}
```

## 注解配置方式

### 定义接口
```java
package com.sihai.dubbo.provider.service.annotation;

/**
 * 注解方式接口
 */
public interface ProviderServiceAnnotation {
    String SayHelloAnnotation(String word);
}
```
实现类 `@Service` 用来配置 Dubbo 的服务提供方。
```java
package com.sihai.dubbo.provider.service.annotation;

import com.alibaba.dubbo.config.annotation.Service;

/**
 * 注解方式实现类
 */
@Service(timeout = 5000)
public class ProviderServiceImplAnnotation implements ProviderServiceAnnotation{

    public String SayHelloAnnotation(String word) {
        return word;
    }
}
```
### 组装服务提供方
通过 Spring 中 Java Config 的技术（@Configuration）和 annotation 扫描（@EnableDubbo）来发现、组装、并向外提供 Dubbo 的服务。
```java
package com.sihai.dubbo.provider.configuration;

import com.alibaba.dubbo.config.ApplicationConfig;
import com.alibaba.dubbo.config.ProtocolConfig;
import com.alibaba.dubbo.config.ProviderConfig;
import com.alibaba.dubbo.config.RegistryConfig;
import com.alibaba.dubbo.config.spring.context.annotation.EnableDubbo;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;

/**
 * 注解方式配置
 */
@Configuration
@EnableDubbo(scanBasePackages = "com.sihai.dubbo.provider.service.annotation")
public class DubboConfiguration {

    @Bean // #1 服务提供者信息配置
    public ProviderConfig providerConfig() {
        ProviderConfig providerConfig = new ProviderConfig();
        providerConfig.setTimeout(1000);
        return providerConfig;
    }

    @Bean // #2 分布式应用信息配置
    public ApplicationConfig applicationConfig() {
        ApplicationConfig applicationConfig = new ApplicationConfig();
        applicationConfig.setName("dubbo-annotation-provider");
        return applicationConfig;
    }

    @Bean // #3 注册中心信息配置
    public RegistryConfig registryConfig() {
        RegistryConfig registryConfig = new RegistryConfig();
        registryConfig.setProtocol("zookeeper");
        registryConfig.setAddress("localhost");
        registryConfig.setPort(2181);
        return registryConfig;
    }

    @Bean // #4 使用协议配置，这里使用 dubbo
    public ProtocolConfig protocolConfig() {
        ProtocolConfig protocolConfig = new ProtocolConfig();
        protocolConfig.setName("dubbo");
        protocolConfig.setPort(20880);
        return protocolConfig;
    }
}
```

### 启动服务

```java
package com.sihai.dubbo.provider;

import com.alibaba.dubbo.config.spring.context.annotation.DubboComponentScan;
import com.sihai.dubbo.provider.configuration.DubboConfiguration;
import org.springframework.context.annotation.AnnotationConfigApplicationContext;
import sun.applet.Main;

import java.io.IOException;

/**
 * 注解启动方式
 */
public class AppAnnotation {

    public static void main(String[] args) throws IOException {
        AnnotationConfigApplicationContext context = new AnnotationConfigApplicationContext(DubboConfiguration.class); 
        context.start();
        System.in.read(); 
    }
}
```
### 消费端
`pom.xml`
```xml
<dependency>
    <groupId>com.ouyangsihai</groupId>
    <artifactId>dubbo-provider</artifactId>
    <version>1.0-SNAPSHOT</version>
</dependency>
```
引用服务
```java
package com.sihai.dubbo.consumer.Annotation;

import com.alibaba.dubbo.config.annotation.Reference;
import com.sihai.dubbo.provider.service.annotation.ProviderServiceAnnotation;
import org.springframework.stereotype.Component;

/**
 * 注解方式的service
 */
@Component("annotatedConsumer")
public class ConsumerAnnotationService {

    @Reference
    private ProviderServiceAnnotation providerServiceAnnotation;

    public String doSayHello(String name) {
        return providerServiceAnnotation.SayHelloAnnotation(name);
    }
}
```
如果这里找不到 ProviderServiceAnnotation 类，请在服务端先把服务端工程用 Maven intall 一下，然后将服务端的依赖放到消费端的 pom 中。如下：
### 组装服务消费者
```java
package com.sihai.dubbo.consumer.configuration;

import com.alibaba.dubbo.config.ApplicationConfig;
import com.alibaba.dubbo.config.ConsumerConfig;
import com.alibaba.dubbo.config.RegistryConfig;
import com.alibaba.dubbo.config.spring.context.annotation.EnableDubbo;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.ComponentScan;
import org.springframework.context.annotation.Configuration;

import java.util.HashMap;
import java.util.Map;

/**
 * 注解配置类
 */
@Configuration
@EnableDubbo(scanBasePackages = "com.sihai.dubbo.consumer.Annotation")
@ComponentScan(value = {"com.sihai.dubbo.consumer.Annotation"})
public class ConsumerConfiguration {
    @Bean // 应用配置
    public ApplicationConfig applicationConfig() {
        ApplicationConfig applicationConfig = new ApplicationConfig();
        applicationConfig.setName("dubbo-annotation-consumer");
        Map<String, String> stringStringMap = new HashMap<String, String>();
        stringStringMap.put("qos.enable","true");
        stringStringMap.put("qos.accept.foreign.ip","false");
        stringStringMap.put("qos.port","33333");
        applicationConfig.setParameters(stringStringMap);
        return applicationConfig;
    }

    @Bean // 服务消费者配置
    public ConsumerConfig consumerConfig() {
        ConsumerConfig consumerConfig = new ConsumerConfig();
        consumerConfig.setTimeout(3000);
        return consumerConfig;
    }

    @Bean // 配置注册中心
    public RegistryConfig registryConfig() {
        RegistryConfig registryConfig = new RegistryConfig();
        registryConfig.setProtocol("zookeeper");
        registryConfig.setAddress("localhost");
        registryConfig.setPort(2181);
        return registryConfig;
    }
}
```
#### 发起远程调用

在 main 方法中通过启动一个 Spring Context，从其中查找到组装好的 Dubbo 的服务消费者，并发起一次远程调用。
```java
package com.sihai.dubbo.consumer;

import com.sihai.dubbo.consumer.Annotation.ConsumerAnnotationService;
import com.sihai.dubbo.consumer.configuration.ConsumerConfiguration;
import com.sihai.dubbo.provider.service.ProviderService;
import org.springframework.context.annotation.AnnotationConfigApplicationContext;
import org.springframework.context.support.ClassPathXmlApplicationContext;

import java.io.IOException;

/**
 * 注解方式启动
 *
 */
public class AppAnnotation
{
    public static void main( String[] args ) throws IOException {

        AnnotationConfigApplicationContext context = new AnnotationConfigApplicationContext(ConsumerConfiguration.class); 
        context.start(); // 启动
        ConsumerAnnotationService consumerAnnotationService = context.getBean(ConsumerAnnotationService.class); 
        String hello = consumerAnnotationService.doSayHello("annotation"); // 调用方法
        System.out.println("result: " + hello); // 输出结果

    }
}
```
