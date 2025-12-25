## APK文件简介

每个需要安装到 android 平台的应用都要被编译打包为一个单独的文件，后缀名为.apk（Android application package），其中包含了应用的二进制代码、资源、配置文件等。

![](https://www.biaodianfu.com/wp-content/uploads/2021/10/apk.png)

apk 文件实际是一个 zip 压缩包，可以通过解压缩工具解开（将后缀名改为.zip 后再用解压缩文件解压）。APK 文件的大致目录结构如下：

```
|– AndroidManifest.xml
|– META-INF
||– CERT.RSA
||– CERT.SF
|`– MANIFEST.MF
|– classes.dex
|– res
||– drawable
||`– icon.png
|`– layout
|`– main.xml
`– resources.arsc
```

### AndroidManifest.xml

AndroidManifest.xml 官方解释是应用清单，每个应用的根目录中都必须包含一个，并且文件名必须一模一样。这个文件中包含了 APP 的配置信息，系统需要根据里面的内容运行 APP 的代码，显示界面。

上述的功能是非常笼统的解释，具体到细节就是：

上面是官方的解释。很多东西现在还不能理解，也没有用到，先挑能理解的进行解释。

AndroidManifest.xml 文件示例：

```
<manifest xmlns:android="http://schemas.android.com/apk/res/android"
package="com.sample.teapot"
android:versionCode="1"
android:versionName="1.0.0.1">

<uses-feature android:glEsVersion="0x00020000"></uses-feature>

<application
android:allowBackup="false"
android:fullBackupContent="false"
android:supportsRtl="true"
android:icon="@mipmap/ic_launcher"
android:label="@string/app_name"
android:theme="@style/AppTheme"
android:name="com.sample.teapot.TeapotApplication"
>

<!-- Our activity is the built-in NativeActivity framework class.
This will take care of integrating with our NDK code. -->
<activity android:name="com.sample.teapot.TeapotNativeActivity"
android:label="@string/app_name"
android:configChanges="orientation|keyboardHidden">
<!-- Tell NativeActivity the name of our .so -->
<meta-data android:name="android.app.lib_name"
android:value="TeapotNativeActivity"/>
<intent-filter>
<action android:name="android.intent.action.MAIN"/>
<category android:name="android.intent.category.LAUNCHER"/>
</intent-filter>
</activity>
</application>
</manifest>
```

这是 Google 官方示例中的 teapots 项目中的一个文件，我们就针对这份文件来分析字段的意义。字段的意义参考的是[官方文档](https://developer.android.com/guide/topics/manifest/manifest-element?hl=zh-cn)。

**<manifest>元素：**所有的 xml 都必须包含 <manifest> 元素。这是文件的根节点。它必须要包含 <application> 元素，并且指明 xmlns:android 和 package 属性。

**<manifest>元素中的属性**：

**<manifest>元素中的元素**

```
<uses-feature android:name="android.hardware.bluetooth"/>
<uses-feature android:name="android.hardware.camera"/>
```

需要注意的是，直接通过 zip 解压出来的 AndroidManifest.xml 文件是经过压缩过的。如果直接用记事本打开是乱码。可以通过 AXMLPrinter2 工具解开。具体流程后面会详细介绍。

### Res 文件夹

在 Android 项目文件夹里面，主要的[资源文件](https://developer.android.com/guide/topics/resources/providing-resources.html#ResourceTypes) 是放在 res 文件夹，res 文件夹下为所有的资源文件。

### resources.arsc 文件

resource.arsc 文件是 Apk 打包过程中的产生的一个资源索引文件。在对 apk 进行解压或者使用 Android Studio 对 apk 进行分析时便可以看到 resource.arsc 文件。通过学习 resource.arsc 文件结构，可以帮助我们深入了解 apk 包体积优化中使用到的重复资源删除、资源文件名混淆技术。

**arsc 文件作用**

在 Java 中访问一个文件是需要提供文件路径，如：

```
new File("./res/drawable-xxhdpi/img.png");
```

而在 Android 中，却可以通过 drawableId 获得资源文件:

```
getDrawable(R.drawable.img);
```

这里凭一个 id 就能获取资源文件内容，省去了文件路径的手动输入，其背后就是通过读取 arsc 文件实现的。这些 R.drawable.xxx、R.layout.xxx、R.string.xxx 等的值（存储在 R.jar 或者 R.java 文件中）称之为资源索引，通过这些资源索引可以在 arsc 文件中查取实际的资源路径或资源值。例如：getDrawable(R.drawable.img) 在编译后成了 getDrawable(2131099964)，再将 id 转为十六进制：2131099964=0x7f06013c。这时的资源索引为 0x7f06013c。

资源索引具有固定的格式：0xPPTTEEEE，PackageId(2 位)+TypeId（2 位）+EntryId(4 位)：

所以 0x7f06013c 中 PackageId=0x7f、TypeId=0x06、EntryId=0x013c

最简单的我们可以将 arsc 函数想象成一个含有多个 Pair 数组的文件，且每个资源类型(TypeId)对应一个 Pair\[\]（或多个,为了便于理解先只认为是一个）。因此在 arsc 中查找 0x7f06013c 元素的值，就是去设法找到 TypeId=0x06 所对应的数组 Pair\[\]，然后找到其中的第 0X013c 号元素 Pair\[0X013c\]。这个元素恰好就是 Pair(“img”,”./res/drawable-xxhdpi/img.png”),左边是资源名称 img，右边是资源的文件路径 “./res/drawable-xxhdpi/img.png”，有了文件路径，程序便可以访问到对应的资源文件了。

当然实际的 arsc 文件在结构上要稍微复杂一点，下面开始分析 arsc 文件结构。

**chunk**为了便于理解，在正式介绍resource.arsc(以下简称arsc)文件前，需要对chunk进行解释一下，在其他文章中也多次使用了“chunk”这个词。chunk翻译为中文就是“块、部分(尤指大部分，一大块)”的意思，例如：一棵树，可以分为三个chunk(部分)：树冠、树茎、树根。也可以将一棵树视为一个chunk，这个chunk就是这棵树。

**arsc文件结构**

resources.arsc是一个二进制文件，其内部结构的定义在ResourceTypes.h，arsc文件结构：

![](https://www.biaodianfu.com/wp-content/uploads/2021/10/ResourceTypes.png)

图片整体描述了arsc文件中各个chunk的关系(注意结合图片左右两侧内容):

### META-INF目录

META-INF目录下存放的是签名信息，用来保证apk包的完整性和系统的安全。Android开发环境对每一个需要Release的APK都会进行签名，在APK文件被安装时，Android系统会对APK的签名信息进行比对，以此来判断程序的完整性，最终确定APK是否可以正常安装使用，一定程度上达到安全的目的。

META-INF目录下看到四个文件：MANIFEST.MF、CERT.SF、CERT.RSA

在APK进行安装时，可以通过MANIFEST.MF文件开始的环环相扣来保证APK的安全性。比如拿到一个apk包后，如果想要替换里面的一幅图片，一段代码，或一段版权信息，想直接解压缩、替换再重新打包，基本是不可能的。如此一来就给病毒感染和恶意修改增加了难度，有助于保护系统的安全。但这些文件或者密钥如果被攻击者得到或者被攻击者通过某些技术手段攻破，则Android操作系统无法验证其安全性。

### classes.dex文件

dex文件是Android系统中的一种文件，是一种特殊的数据格式，和APK、jar等格式文件类似。能够被DVM识别，加载并执行的文件格式。相对于PC上的java虚拟机能运行.class；android上的Davlik虚拟机能运行.dex。当Java程序编译成class后，还需要使用dx工具将所有的class文件整合到一个dex文件，目的是其中各个类能够共享数据，在一定程度上降低了冗余，同时也是文件结构更加经凑，实验表明，dex文件是传统jar文件大小的50%左右。

![](https://www.biaodianfu.com/wp-content/uploads/2021/10/dex.png)

在明白什么是Dex文件之前，要先了解一下JVM，Dalvik和ART。JVM是JAVA虚拟机，用来运行JAVA字节码程序。Dalvik是Google设计的用于Android平台的运行时环境，适合移动环境下内存和处理器速度有限的系统。ART即Android Runtime，是Google为了替换Dalvik设计的新Android运行时环境，在Android4.4推出。ART比Dalvik的性能更好。Android程序一般使用Java语言开发，但是Dalvik虚拟机并不支持直接执行JAVA字节码，所以会对编译生成的.class文件进行翻译、重构、解释、压缩等处理，这个处理过程是由dx进行处理，处理完成后生成的产物会以.dex结尾，称为Dex文件。Dex文件格式是专为Dalvik设计的一种压缩格式。所以可以简单的理解为：Dex文件是很多.class文件处理后的产物，最终可以在Android运行时环境执行。

Java代码转化为dex文件的流程如图所示，当然真的处理流程不会这么简单，这里只是一个形象的显示：

![](https://www.biaodianfu.com/wp-content/uploads/2021/10/java-to-dex.png)

### lib文件夹

引用的第三方sdk的so文件，有C/C++编译而成。

## Apk的打包流程

我们先对安卓的打包流程进行一个简单的了解，从而明白.java文件是一步步成为apk中的一部分的，在生成apk的过程中主要包含以下流程，括号中代表使用的工具：

![](https://www.biaodianfu.com/wp-content/uploads/2021/10/apk-packager.png)

## APK文件的反编译

### 资源文件获取

[apktool](https://ibotpeaches.github.io/Apktool/)主要用于资源文件的获取，Apktool的主要作用：

所以Apktool不光能够拆解apk，还能加已经拆解的apk资源重新组装成apk。

使用就比较简单了，直接执行如下命令就可以进行反编译：

```
apktool d bar.apk //直接解码
apktool d bar.apk -o baz //解码到baz的文件夹中
```

Apktool既然能够进行反编译，那也能重新编译成Apk文件：

```
apktool b bar //在父目录执行building
apktool b . //在当前bar目录执行building
apktool b bar -o new_bar.apk // 在父目录执行building并生成名为new_bar的apk文件
```

使用apktool大概有如下作用:

![](https://www.biaodianfu.com/wp-content/uploads/2021/10/apktool.png)

解压后获得AndroidManifest.xml文件、assets文件夹、res文件夹、smali文件夹等。original文件夹是原始的AndroidManifest.xml文件，res文件夹是反编译出来的所有资源，smali文件夹是反编译出来的代码。注意，smali文件夹下的结构和我们的源代码的package一模一样，只不过换成了smali语言。它有点类似于汇编的语法，是Android虚拟机所使用的寄存器语言。

使用apktool版本时如果版本过低会报如下错误：

```
.........apktool..........
I: Baksmaling...
testI: Loading resource table...
Exception in thread "main" brut.androlib.AndrolibException: Could not decode arsc file
at brut.androlib.res.decoder.ARSCDecoder.decode(ARSCDecoder.java:55)
at brut.androlib.res.AndrolibResources.getResPackagesFromApk(AndrolibResources.java:315)
at brut.androlib.res.AndrolibResources.loadMainPkg(AndrolibResources.java:50)
at brut.androlib.res.AndrolibResources.getResTable(AndrolibResources.java:43)
at brut.androlib.Androlib.getResTable(Androlib.java:44)
at brut.androlib.ApkDecoder.getResTable(ApkDecoder.java:148)
at brut.androlib.ApkDecoder.decode(ApkDecoder.java:98)
at brut.apktool.Main.cmdDecode(Main.java:120)
at brut.apktool.Main.main(Main.java:57)
Caused by: java.io.IOException: Expected: 0x001c0001, got: 0x00000000
at brut.util.ExtDataInput.skipCheckInt(ExtDataInput.java:48)
at brut.androlib.res.decoder.StringBlock.read(StringBlock.java:45)
at brut.androlib.res.decoder.ARSCDecoder.readPackage(ARSCDecoder.java:97)
at brut.androlib.res.decoder.ARSCDecoder.readTable(ARSCDecoder.java:82)
at brut.androlib.res.decoder.ARSCDecoder.decode(ARSCDecoder.java:48)
...8 more
```

解决方案是去官网下载最新版本，替换后如果发现如下错误：

```
.........apktool..........
Smali Debugging has been removed in 2.1.0 onward. Please see: https://github.com/
iBotPeaches/Apktool/issues/1061
```

原因是最新版的apktool已将SmaliDebugging移除，我的解决方案是下载2.0.9版本。

### XML文件的反编译

在apk中的xml文件是经过压缩的，可以通过[AXMLPrinter2工具](https://code.google.com/archive/p/android4me/downloads)解开，具体命令类似：

java -jar .\\AXMLPrinter2.jar .\\AndroidManifest.xml > .\\AndroidManifest.txt

apktool已经自带AndroidManifest.xml文件解压，所以这个工具价值不大。

### classes.dex文件反编译

classes.dex是java源码编译后生成的java字节码文件。但由于Android使用的dalvik虚拟机与标准的java虚拟机是不兼容的，dex文件与class文件相比，不论是文件结构还是opcode都不一样。目前常见的java反编译工具都不能处理dex文件。Android模拟器中提供了一个dex文件的反编译工具，dexdump。用法为首先启动Android模拟器，把要查看的dex文件用adb push上传的模拟器中，然后通过adb shell登录，找到要查看的dex文件，执行dexdump xxx.dex。但是这样得到的结果，其可读性是极差的。下面介绍一个可读性比较好的工具。

反编译的步骤：

dex2jar软件包中包含的工具：

[Enjarify](https://github.com/Storyyeller/enjarify)是由Google推出的一款基于Python3开发，类似dex2jar的反编译工具，它可以将Dalvik字节码转换成相对应的Java字节码，有比dex2jar更优秀的兼容性，准确性及更高的效率。

有时使用JD-GUI反编译的时候会有部分文件（Constants类即常量类）打开是// INTERNAL ERROR //，解决办法，使用其他[Luyten](https://github.com/deathmarine/Luyten)或[jadx](https://github.com/skylot/jadx)代替。

### 自动化工具汇总（一键反编译Apk）

**onekey-decompile-apk**

[onekey-decompile-apk](https://github.com/ufologist/onekey-decompile-apk)是先前我使用的 apk 反编译工具，功能比较简单，主要是 apktool/dex2jar/jd-gui 只需执行一步即可反编译出 apk 所有文件(资源文件和 jar 等等)。使用方法非常简单：

由于作者长时间没有更新，其实自己可升级 \_tool 目录下的工具，并在 \_onekey-decompile-apk.bat 中修改成最新版软件的路径。

**TTDeDroid**

[TTDeDroid](https://github.com/tp7309/TTDeDroid) 也是一键反编译工具，支持反编译 decompile apk/aar/dex/jar，主要集成了 jadx/dex2jar/enjarify（不需要手动安装 Python）

**Google 官方：android-classysh**

[android-classysh](https://github.com/google/android-classyshark/) 是 Google 推出的一键反编译工具，直接打开 Apk 文件，就可以看到 Apk 中所有的文件结构，甚至还集成了 dex 文件查看，java 代码查看，方法数分析、导入混淆 mapping 文件等一系列工具。谷歌推出这个工具的目的是为了让我们开发者更清楚的了解自己的 Apk 中都有什么文件、混淆前后有什么变化，并方便我们进一步优化自己的 Apk 打包实现。比较不好的体验是无法进行导出。

**Python 实现的工具：Androguard**

[Androguard](https://github.com/androguard/androguard/) 集成了反编译资源、代码等各种文件的工具包。需要安装 Python 环境来运行这个工具，这个工具按照不同的反编译需求，分别写成了不同的 py 功能模块，还有静态分析的功能。所以如果想要用 Python 开发一个解析 Apk 文件并进行静态扫描分析的服务，可以引用这个工具来实现。

- https://github.com/androguard/androguard
- https://github.com/androguard/goauld

**AndroidKiller**

[AndroidKiller](https://github.com/liaojack8/AndroidKiller) 集 Apk 反编译、Apk 打包、Apk 签名，编码互转，ADB 通信（应用安装-卸载-运行-设备文件管理）等特色功能于一身，支持 logcat 日志输出，语法高亮，基于关键字（支持单行代码或多行代码段）项目内搜索，可自定义外部工具；吸收融汇多种工具功能与特点，打造一站式逆向工具操作体验，大大简化了用户在安卓应用/游戏修改过程中的各类繁琐工作。

**GDA**

[GDA](https://github.com/charles2gan/GDA-android-reversing-Tool) 除了反编译外，还支持包过滤的分析功能，提供有算法工具，文件转换工具等等**Bytecode Viewer**

[Bytecode Viewer](https://github.com/Konloch/bytecode-viewer) 是一个强大的反编译工具，其集成了 6 个 Java 反编译库（包含 Fernflower 和 CFR），Andorid 反编译类库和字节码类库。

这个工具提供 GUI 界面，可以提升使用 CFR 的用户体验。

## APK 反编译高级篇

Android APK 中的 Java 代码可以被反编译到什么程度主要看 APK 的加密程度。

### Android 混淆处理：ProGuard

Android SDK 自带了混淆工具 Proguard。它位于 SDK 根目录\\tools\\proguard 下面。ProGuard 是一个免费的 Java 类文件收缩，优化，混淆和预校验器。它可以检测并删除未使用的类，字段，方法和属性。它可以优化字节码，并删除未使用的指令。它可以将类、字段和方法使用短无意义的名称进行重命名。最后，预校验的 Java 6 或针对 Java Micro Edition 的所述处理后的码。如果开启了混淆，Proguard 默认情况下会对所有代码，包括第三方包都进行混淆，可是有些代码或者第三方包是不能混淆的，这就需要我们手动编写混淆规则来保持不能被混淆的部分。

Android 中的“混淆”可以分为两部分，一部分是 Java 代码的优化与混淆，依靠 proguard 混淆器来实现；另一部分是资源压缩，将移除项目及依赖的库中未被使用的资源。

**代码混淆**

上面这几个功能都是默认打开的，要关闭他们只需配置对应的规则即可。混淆后默认会在工程目录 app/build/outputs/mapping/release 下生成一个 mapping.txt 文件，这就是混淆规则，我们可以根据这个文件把混淆后的代码反推回源本的代码。原则上，代码混淆后越乱越无规律越好，但有些地方我们是要避免混淆的，否则程序运行就会出错。

**资源压缩**

资源压缩将移除项目及依赖的库中未被使用的资源，这在减少 apk 包体积上会有不错的效果，一般建议开启。具体做法是在 build.grade 文件中，将 shrinkResources 属性设置为 true。需要注意的是，只有在用 minifyEnabled true 开启了代码压缩后，资源压缩才会生效。资源压缩包含了“合并资源”和“移除资源”两个流程。“合并资源”流程中，名称相同的资源被视为重复资源会被合并。需要注意的是，这一流程不受 shrinkResources 属性控制，也无法被禁止，gradle 必然会做这项工作，因为假如不同项目中存在相同名称的资源将导致错误。gradle 在四处地方寻找重复资源：

依赖->main->渠道->构建类型

举个例子，假如重复资源同时存在于 main 文件夹和不同渠道中，gradle 会选择保留渠道中的资源。同时，如果重复资源在同一层次出现，比如 src/main/res/ 和 src/main/res2/，则 gradle 无法完成资源合并，这时会报资源合并错误。“移除资源”流程则见名知意，需要注意的是，类似代码，混淆资源移除也可以定义哪些资源需要被保留，这点在下文给出。

### DexGuard 混淆(防二次打包)

DexGuard 与 Android 上主流的混淆工具 ProGuard 同属一家公司开发，但相比免费的 ProGuard 功能更多，混淆力度也更大。

### Apk 文件加壳

所谓 Apk 加壳，就是给目标 Apk 加一层保护程序，把重要数据信息隐藏起来。加壳程序可以有效阻止对程序的反编译和逆向分析。Apk 壳本质的功能就是实现类加载器。系统先执行壳代码，然后将加了密的 dex 进行解密操作，再加载到系统内存中运行。

#### 安卓 dex 加壳原理

加壳过程中主要有三个程序：

加壳过程：

脱壳过程：

#### 壳史

**第一代壳：DEX 加密(混淆技术)**

**第二代壳：Dex 抽取与 So 加固(加壳技术)**

类抽取常规的有隐藏 dex 文件和修改 dex 结构。隐藏 dex 文件是通过对目标 dex 文件进行整体加密或压缩方式把整个 dex 转换为另外一个文件存放在 assert 文件夹中或者其它地方，然后利用类加载器技术进行内存解密并加载运行。而修改 dex 结构则是抽取 DexCode 中的字节码指令后用零去填充，或者修改方法属性等操作，运行时在内存中做修正，修复等处理工作。

关于 Dex 动态加载，就要提到 dalvik 虚拟机了。它和 java 虚拟机一样，在运行程序时首先需要将对应的类加载到内存中。在标准虚拟机中，类加载可以从 class 文件中读取，也可以是其他二进制流，这样就可以在程序运行时手动加载 class，从而达到代码动态执行的目的。常用的有两个类：DexClassLoader 和 PathClassLoader,PathClassLoader 是安卓应用中的默认加载器。区别：

**第三代壳：Dex 动态解密与 So 混淆(指令抽离)**

**第四代壳：armvmp(指令转换)**

vmp:用 vmp 加固后的还原过程比较复杂和困难，需要用大量的时间作分析。

### 壳的识别

国内提供 apk 加固的第三方技术公司有：娜迦、爱加密、梆梆加固、360 加固保、百度加固、腾讯加固等。

经过加固后的 apk，通过 dex2jar 反编译：

腾讯乐固：

![](https://www.biaodianfu.com/wp-content/uploads/2021/10/tencent.png)

360 加固：

![](https://www.biaodianfu.com/wp-content/uploads/2021/10/360.png)

通过查看资料发现大多数加密后都会生成相应的特征 so 文件。这样就可以根据 so 来查壳。

<table><tbody><tr><td>特征 So 文件</td><td>所属加固公司</td></tr><tr><td>libchaosvmp.so</td><td>娜迦</td></tr><tr><td>libddog.so</td><td>娜迦</td></tr><tr><td>libfdog.so</td><td>娜迦</td></tr><tr><td>libedog.so</td><td>娜迦企业版</td></tr><tr><td>libexec.so</td><td>爱加密</td></tr><tr><td>libexecmain.so</td><td>爱加密</td></tr><tr><td>ijiami.dat</td><td>爱加密</td></tr><tr><td>ijiami.ajm</td><td>爱加密企业版</td></tr><tr><td>libsecexe.so</td><td>梆梆免费版</td></tr><tr><td>libsecmain.so</td><td>梆梆免费版</td></tr><tr><td>libSecShell.so</td><td>梆梆免费版</td></tr><tr><td>libDexHelper.so</td><td>梆梆企业版</td></tr><tr><td>libDexHelper-x86.so</td><td>梆梆企业版</td></tr><tr><td>libprotectClass.so</td><td>360</td></tr><tr><td>libjiagu.so</td><td>360</td></tr><tr><td>libjiagu_art.so</td><td>360</td></tr><tr><td>libjiagu_x86.so</td><td>360</td></tr><tr><td>libegis.so</td><td>通付盾</td></tr><tr><td>libNSaferOnly.so</td><td>通付盾</td></tr><tr><td>libnqshield.so</td><td>网秦</td></tr><tr><td>libbaiduprotect.so</td><td>百度</td></tr><tr><td>aliprotect.dat</td><td>阿里聚安全</td></tr><tr><td>libsgmain.so</td><td>阿里聚安全</td></tr><tr><td>libsgsecuritybody.so</td><td>阿里聚安全</td></tr><tr><td>libmobisec.so</td><td>阿里聚安全</td></tr><tr><td>libtup.so</td><td>腾讯</td></tr><tr><td>libexec.so</td><td>腾讯</td></tr><tr><td>libshell.so</td><td>腾讯</td></tr><tr><td>mix.dex</td><td>腾讯</td></tr><tr><td>lib/armeabi/mix.dex</td><td>腾讯</td></tr><tr><td>lib/armeabi/mixz.dex</td><td>腾讯</td></tr><tr><td>libtosprotection.armeabi.so</td><td>腾讯御安全</td></tr><tr><td>libtosprotection.armeabi-v7a.so</td><td>腾讯御安全</td></tr><tr><td>libtosprotection.x86.so</td><td>腾讯御安全</td></tr><tr><td>libnesec.so</td><td>网易易盾</td></tr><tr><td>libAPKProtect.so</td><td>APKProtect</td></tr><tr><td>libkwscmm.so</td><td>几维安全</td></tr><tr><td>libkwscr.so</td><td>几维安全</td></tr><tr><td>libkwslinker.so</td><td>几维安全</td></tr><tr><td>libx3g.so</td><td>顶像科技</td></tr><tr><td>libapssec.so</td><td>盛大</td></tr><tr><td>librsprotect.so</td><td>瑞星</td></tr></tbody></table>

这个特点可以作为加壳厂商的特征。这样就可以得到厂商加壳的套路，脱壳也就有了破解之法。  
这里提一下网上的一款apk查壳工具：ApkScan-PKID.其工作原理就是根据apk加壳后生成的特征文件来匹配加壳厂商的。

### 常用脱壳软件

**VirtualXposed**

[VirtualXposed](https://vxposed.com/)：无需root手机即可使用Xposed框架。

**ZjDroid**

[ZjDroid](https://github.com/halfkiss/ZjDroid)是基于Xposed Framewrok的动态逆向分析模块，逆向分析者可以通过ZjDroid完成以下工作：1、DEX文件的内存dump 2、基于Dalvik关键指针的内存BackSmali，有效破解主流加固方案 3、敏感API的动态监控 4、指定内存区域数据dump 5、获取应用加载DEX信息。6、获取指定DEX文件加载类信息。7、dump Dalvik java堆信息。8、在目标进程动态运行lua脚本。

**FDex2**

FDex2，可以从安卓app中dump导出有用的dex文件，供后续再从dex导出jar包，jar包导出java源码，功能非常的强大。

**frida**
- https://github.com/frida/frida
- https://github.com/iddoeldor/frida-snippets
[frida](https://frida.re/)的原理是，通过在PC上安装Frida，手机上运行frida-server，实现PC对手机的控制，同时通过js注入的方式，将dex从“壳”里“钩”出来。它是一款基于Python的hook（钩子）工具，因此在安装它之前我们需要先配置Py环境，现在的frida仅支持3.7以下的环境，3.8以上的暂不支持。[FRIDA-DEXDump](https://github.com/hluwa/FRIDA-DEXDump)

**dumpDex**

[dumpDex](https://github.com/WrBug/dumpDex)是一款Android脱壳工具，需要xposed支持**drizzleDumper**

[drizzleDumper](https://github.com/DrizzleRisk/drizzleDumper)是一款基于内存搜索的Android脱壳工具，可以从运行中的安卓app中，利用ptrace机制，导出dex文件
