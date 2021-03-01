# 环境配置

`ANDROID_SDK_ROOT C:\Users\Administrator\AppData\Local\Android`
`JAVA_HOME C:\Program Files\Android\Android Studio\jre`
`PATH %JAVA_HOME%\bin;%JAVA_HOME%\bin\jre\bin;`

`gradle\wrapper\gradle-wrapper.properties`

```shell
cocos new popstar -p com.brianbaek.popstar -l cpp
cocos run -p android
```

`cmake` version

`C:\Users\Administrator\AppData\Local\Android\Sdk\cmake\3.18.1\bin`
`C:\Users\Administrator\AppData\Local\Android\Sdk\cmake\3.10.2\bin`
```gradle
    externalNativeBuild {
        cmake {
            path "../../CMakeLists.txt"
            version "3.10.2"
        }
    }
```

`gradle` version

`gradle sync failed`错误，更新版本

`distributionUrl=file:///D:/work/gradle-6.5-all.zip`
```gradle
        classpath 'com.android.tools.build:gradle:3.1.0'
```

https://dl.google.com/dl/android/studio/install/2.3.3.0/android-studio-bundle-162.4069837-windows.exe

## Android SDK在线更新镜像服务器

* `hosts`

74.125.237.1  dl-ssl.google.com
74.125.237.1  dl.google.com

203.208.46.146  dl.google.com
203.208.46.146  dl-ssl.google.com
127.0.0.1 servserv.generals.ea.com

* `proxy`

南阳理工学院镜像服务器地址:

mirror.nyist.edu.cn 端口:80

中国科学院开源协会镜像站地址:

IPV4/IPV6: mirrors.opencas.cn 端口:80
IPV4/IPV6: mirrors.opencas.org 端口:80
IPV4/IPV6: mirrors.opencas.ac.cn 端口:80

上海GDG镜像服务器地址:

sdk.gdgshanghai.com 端口:8000

北京化工大学镜像服务器地址:

IPv4: ubuntu.buct.edu.cn/ 端口:80

IPv4: ubuntu.buct.cn/ 端口:80

IPv6: ubuntu.buct6.edu.cn/ 端口:80

大连东软信息学院镜像服务器地址:

mirrors.neusoft.edu.cn 端口:80

腾讯Bugly 镜像:

android-mirror.bugly.qq.com 端口:8080

腾讯镜像使用方法: http://android-mirror.bugly.qq.com:8080/include/usage.html
