# Javabridge server

```shell
sudo apt-get install openjdk-8-jdk
sudo apt-get install tomcat8
``

```text
# 1. Download and unzip the latest php-java-bridge sources
$ wget https://github.com/belgattitude/php-java-bridge/archive/7.1.3.zip -O pjb.zip
$ unzip pjb.zip && cd php-java-bridge-7.1.3
# 2. Customize and build your own bridge:
#    Example below contains some pre-made gradle init scripts
#    to include jasperreports and mysql-connector libraries to the
#    build. They are optional, remove the (-I) parts or provide
#    your own.       
$ ./gradlew war \
         -I init-scripts/init.jasperreports.gradle \
         -I init-scripts/init.mysql.gradle
# The build files are generated in the '/build/libs' folder.
$ cp ./build/libs/JavaBridgeTemplate.war /var/lib/tomcat8/webapps/MyJavaBridge.war
```

Tomcat will automatically deploy and register the servlet. Wait few seconds and point your browser to http://localhost:8080/MyJavaBridge (note the /MyJavaBridge uri corresponds to the deployed war filename, easily changeable)

## 使用

```php
<?php

$options = [     
    'servlet_address' => 'http://localhost:8080/MyJavaBridge/servlet.phpjavabridge'
];

try {
	$ba = new Soluble\Japha\Bridge\Adapter($options);

	// Some standard JVM classes
	$hashMap = $ba->java('java.util.HashMap', [         
		'message' => 'Hello world',                 
		'value'   => $ba->java('java.math.BigInteger', PHP_INT_MAX)
	]);
	$hashMap->put('message', '你好，世界');
	echo $hashMap->get('message');  
} catch (Soluble\Japha\Bridge\Exception\ClassNotFoundException $e) { 
    echo $e->getMessage(); 
} catch (Soluble\Japha\Bridge\Exception\JavaException $e) { 
    echo $e->getMessage() . ' [' . $e->getJavaClassName() . ']';
    echo $e->getStackTrace(); 
}
```