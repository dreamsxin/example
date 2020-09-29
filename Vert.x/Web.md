# Using Vert.x Web


* 初始化项目

方式1:去网站 `https://start.vertx.io/` 创建项目。

方式2:
```shell
git clone https://github.com/vert-x3/vertx-gradle-starter.git vertx-web
# OR
curl http://vertx.io/assets/starter-scripts/create-vertx-project-gradle.sh -o vertx-create-gradle-project.sh; bash vertx-create-gradle-project.sh
```

文件 `create-vertx-project-gradle.sh` 内容:
```shell
#!/usr/bin/env bash

export PROJECT_NAME="vertx-starter"
export VERSION="1.0-SNAPSHOT"

# Read project name
read -p "What's the name of your project? [$PROJECT_NAME] : " projectName
if [ ${#projectName} -ge 1 ]; then PROJECT_NAME=$projectName;
fi

read -p "What's the version of your project? [$VERSION] : " v
if [ ${#v} -ge 1 ]; then VERSION=$v;
fi

echo "Cloning project"
git clone https://github.com/vert-x3/vertx-gradle-starter.git ${PROJECT_NAME}

echo "Generating project"
rm -Rf ${PROJECT_NAME}/.git
sed -i -e "s/1.0-SNAPSHOT/${VERSION}/" ${PROJECT_NAME}/build.gradle

if [ -f "${PROJECT_NAME}/build.gradle-e" ];
then
   rm "${PROJECT_NAME}/build.gradle-e"
fi

echo "======================================"
echo " To check your generated project run:"
echo "     cd $PROJECT_NAME"
echo "     ./gradlew test run"
echo " and open your browser to 'http://localhost:8080'"
echo " Happy coding !"
echo "======================================"
```

方式3:
```shell
# 安装 SDKMAN
curl -s "https://get.sdkman.io" | bash
source "$HOME/.sdkman/bin/sdkman-init.sh"
sdk version

# 升级 gradle
sdk install gradle 6.6.1
mkdir vert-xweb
cd vert-xweb
gradle init
```

```output
Starting a Gradle Daemon (subsequent builds will be faster)

Select type of project to generate:
  1: basic
  2: application
  3: library
  4: Gradle plugin
Enter selection (default: basic) [1..4] 2

Select implementation language:
  1: C++
  2: Groovy
  3: Java
  4: Kotlin
  5: Swift
Enter selection (default: Java) [1..5] 3

Select build script DSL:
  1: Groovy
  2: Kotlin
Enter selection (default: Groovy) [1..2] 2

Select test framework:
  1: JUnit 4
  2: TestNG
  3: Spock
  4: JUnit Jupiter
Enter selection (default: JUnit 4) [1..4]

Project name (default: vertx-web): 
Source package (default: vertx.web): 

> Task :init
Get more help with your project: https://docs.gradle.org/6.6.1/userguide/tutorial_java_projects.html
```

* 显示目录结构

```shell
sudo apt install tree
tree
```
```output
.
├── build.gradle.kts
├── gradle
│   └── wrapper
│       ├── gradle-wrapper.jar
│       └── gradle-wrapper.properties
├── gradlew
├── gradlew.bat
├── settings.gradle.kts
└── src
    ├── main
    │   ├── java
    │   │   └── vertx
    │   │       └── web
    │   │           └── App.java
    │   └── resources
    └── test
        ├── java
        │   └── vertx
        │       └── web
        │           └── AppTest.java
        └── resources

13 directories, 8 files
```

* 创建文件 `src/main/java/vertx/web/MainVerticle.java`
```java
package vertx.web;

import io.vertx.core.AbstractVerticle;
import io.vertx.core.http.HttpServer;
import io.vertx.core.http.HttpServerResponse;

public class MainVerticle extends AbstractVerticle {
	@Override
	public void start() {
		vertx.createHttpServer()
			.requestHandler(req -> req.response().end("Hello Vert.x!"))
			.listen(8080);
		/*
		HttpServer server = vertx.createHttpServer();

		server.requestHandler(request -> {

			// This handler gets called for each request that arrives on the server
			HttpServerResponse response = request.response();
			response.putHeader("content-type", "text/plain");

			// Write to the response and end it
			response.end("Hello World!");
		});

		server.listen(8080)
		*/
	}
}
```

编辑文件 `build.gradle.kts`，添加 Vert.x plugin。
可以添加 shadow 插件将所有依赖一起打包。
```gradle
plugins {
  java
  application
  id("com.github.johnrengelman.shadow") version "4.0.3"
	id("io.vertx.vertx-plugin") version "1.1.1"
}
```
如果不使用 `io.vertx.core.Launcher`可以，删除 application 定义，增加 vertx 定义：
```gradle
vertx {
	mainVerticle = "vertx.web.MainVerticle"
}
```

完整的 build.gradle.kts 内容
```gradle
plugins {
  java
  id("io.vertx.vertx-plugin") version "1.1.1"
}

repositories {
    // Use jcenter for resolving dependencies.
    // You can declare any Maven/Ivy/file repository here.
    jcenter()
}

dependencies {
}

vertx {
	mainVerticle = "vertx.web.MainVerticle"
}
```

* 构建运行
```shell
gradle shadowJar
gradle vertxRun
```

使用 `io.vertx.core.Launcher`，需要添加依赖：
```gradle
import com.github.jengelman.gradle.plugins.shadow.tasks.ShadowJar

plugins {
	java
	application
	id("com.github.johnrengelman.shadow") version "4.0.3"
}

repositories {
    // Use jcenter for resolving dependencies.
    // You can declare any Maven/Ivy/file repository here.
    mavenCentral()
}

dependencies {
	implementation("io.vertx:vertx-core:3.9.3")
}

application {
	mainClassName = "io.vertx.core.Launcher"
}

val mainVerticleName = "vertx.web.MainVerticle"
val watchForChange = "src/**/*.java"
val doOnChange = "${projectDir}/gradlew classes"

tasks {

  getByName<JavaExec>("run") {
    args = listOf("run", mainVerticleName, "--redeploy=${watchForChange}", "--launcher-class=${application.mainClassName}", "--on-redeploy=${doOnChange}")
  }

  withType<ShadowJar> {
    classifier = "fat"
    manifest {
      attributes["Main-Verticle"] = mainVerticleName
    }
    mergeServiceFiles {
      include("META-INF/services/io.vertx.core.spi.VerticleFactory")
    }
  }
}
```

* 构建运行

```shell
gradle run
```
参数 `--redeploy` 配合 `--on-redeploy` 实现代码更新后自动部署。
