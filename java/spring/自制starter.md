# 原理

`SpringBoot Starter` 他将常用的依赖分组进行了整合，将其合并到一个依赖中，这样就可以一次性添加到项目的 Maven 或 Gradle 构建中。
`SpringBoot` 采用 `JavaConfig` 的方式对 `Spring` 进行配置，并且提供了大量的注解，极大的提高了工作效率，比如 `@Configuration` 和 `@bean` 注解结合，基于 `@Configuration` 完成类扫描，基于 `@bean` 注解把返回值注入 `IOC` 容器。
`SpringBoot` 的自动配置特性利用了 `Spring` 对条件化配置的支持，合理地推测应用所需的bean并自动化配置他们。
`SpringBoot` 内置了三种 `Servlet` 容器（Tomcat，Jetty,undertow）只需要一个Java的运行环境就可以运行项目了。
通过引入 `spring-boot-devtools` 插件，可以实现不重启服务器情况下，对项目进行即时编译，对于第三方jar包采用 `base classloader` 来加载，对于开发人员自己开发的代码则使用 `restart classloader` 来进行加载。

https://github.com/lets-mica/mica-auto

## 注解
### SpringBootApplication
由 `SpringBootConfiguration`、`EnableAutoConfiguration`、`ComponentScan` 三个注解组成
```java
//第一个注解
@SpringBootConfiguration
//第二个注解
@EnableAutoConfiguration
//第三个注解
@ComponentScan(excludeFilters = { @Filter(type = FilterType.CUSTOM, classes = TypeExcludeFilter.class), @Filter(type = FilterType.CUSTOM, classes = AutoConfigurationExcludeFilter.class) })
public @interface SpringBootApplication {
}
```

### SpringBootConfiguration
这个注解标注在某个类上，表示这是一个 `Spring Boot` 的配置类。
他的核心代码中，内部有一个核心注解`@Configuration`来表明当前类是配置类，并且可以被组件扫描器扫到，所以`@SpringBootConfiguration`与`@Configuration`具有相同作用，只是前者又做了一次封装。
```java
@Target(ElementType.TYPE)
@Retention(RetentionPolicy.RUNTIME)
@Documented
@Configuration
public @interface SpringBootConfiguration {
}
```
### EnableAutoConfiguration
`Spring` 中有很多以 `Enable` 开头的注解，其作用就是借助 `@Import` 来收集并注册特定场景相关的 `Bean` ，并加载到 `IOC` 容器。
而这个注解就是借助`@Import`来收集所有符合自动配置条件的bean定义，并加载到IoC容器，其中包括两个重要注解：`@AutoConfigurationPackage` 和 `@Import({AutoConfigurationImportSelector.class})`
他的核心源码如下：
```java
@Target(ElementType.TYPE)
@Retention(RetentionPolicy.RUNTIME)
@Documented
@Inherited
@AutoConfigurationPackage
@Import(AutoConfigurationImportSelector.class)
public @interface EnableAutoConfiguration {
```

#### @Import
这个注解的作用：
- 给 `Spring` 容器自动注入导入的类。
- 给容器导入一个 `ImportSelector`，比如上文讲的那个 `AutoConfigurationImportSelector`。通过字符串数组的方式返回配置类的全限定名，然后把这些类注入到Spring容器中，我们也就可以直接在Spring容器中使用这些类了。

### AutoConfigurationImportSelector
可以帮助 `Springboot` 应用将所有符合条件的 `@Configuration` 配置都加载到当前`Spring Boot`创建并使用的IOC容器( ApplicationContext )中。
该注解实现了实现了 DeferredImportSelector 接口和各种Aware 接口。

### spring.factories
定义了实现类和对应的接口之间的关系，其定义格式如下：

`example.MyService=example.MyServiceImpl1,example.MyServiceImpl2`
其中的 `example.MyService` 为接口全称，`example.MyServiceImpl1,example.MyServiceImpl2` 为两个实现类的全称。

## 创建工程 scott-spring-boot-starter

```xml
<groupId>scott-spring-boot-starter</groupId>
<artifactId>scottspringbootstarter</artifactId>
<version>0.0.1-SNAPSHOT</version>
```
`pom.xml`
```xml
    <dependencies>
        <dependency>
            <groupId>com.spring.starter</groupId>
            <artifactId>scott-spring-boot-starter-autoconfigure</artifactId>
            <version>1.0-SNAPSHOT</version>
        </dependency>

    </dependencies>
```

### 创建模块 autoconfigure
如下：
```xml
<groupId>com.spring.starter</groupId>
<artifactId>scott-spring-boot-starter-autoconfigure</artifactId>
<version>0.0.1-SNAPSHOT</version>
```
在`pom.xml`文件中引入如下：
```xml
<parent>
    <groupId>org.springframework.boot</groupId>
    <artifactId>spring-boot-starter-parent</artifactId>
    <version>2.1.8.RELEASE</version>
    <relativePath></relativePath>
</parent>
<dependencies>
    <dependency>
	  <groupId>org.springframework.boot</groupId>
	  <artifactId>spring-boot-starter</artifactId>
    </dependency>
</dependencies>
```
创建 `HelloService`。
```java
public class HelloService {
    HelloProperties helloProperties ;

    public String sayHello(String name){
        return helloProperties.getPrefix()+"-"+name+helloProperties.getSuffix() ;
    }
    public HelloProperties getHelloProperties() {
        return helloProperties;
    }
    public void setHelloProperties(HelloProperties helloProperties) {
        this.helloProperties = helloProperties;
    }
}
```
创建 `HelloProperties`。
```java
@ConfigurationProperties(prefix="scott.hello")
public class HelloProperties {
      
      private String prefix ;
      
      private String suffix ;
      public String getPrefix() {
            return prefix;
      }
      public void setPrefix(String prefix) {
            this.prefix = prefix;
      }
      public String getSuffix() {
            return suffix;
      }
      public void setSuffix(String suffix) {
            this.suffix = suffix;
      }
}
```
创建配置类如下：
```java
@Configuration
@ConditionalOnWebApplication // 在web环境下才生效
@EnableConfigurationProperties(HelloProperties.class) // 属性文件生效
public class HelloServiceAutoConfiguration {
      @Autowired
      HelloProperties helloProperties;
   
      @Bean
      public HelloService helloService() {
            HelloService service = new HelloService();
            service.setHelloProperties(helloProperties);
            return service;
      };
}
```
旧版 在 `META-INF` 文件夹下创建 `spring.factories` 文件，写入如下自己的配置类 。Spring Boot自动配置规约，约定大于规范，配置：
`org.springframework.boot.autoconfigure.EnableAutoConfigureation=com.scott.HelloServiceAutoConfigureation`

新版2.7以后要新建文件 `/META-INF/spring/org.springframework.boot.autoconfigure.AutoConfiguration.imports` 配置：
`com.scott.HelloServiceAutoConfigureation`
