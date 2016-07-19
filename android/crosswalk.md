# Crosswalk 使用

See: https://github.com/crosswalk-project/crosswalk-samples/

首先，從 Maven 取得 Crosswalk 函式庫。
`CrosswalkDemo/app/build.gradle` 這裡要先指定 Maven 的 Repository
```ini
repositories {
    maven {
        url 'https://download.01.org/crosswalk/releases/crosswalk/android/maven2'
    }
}
```

然後指定函式庫名稱及版本號

`compile 'org.xwalk:xwalk_core_library:15.44.384.12'`

在頁面 Layout處，加入以下區塊
```xml
<org.xwalk.core.XWalkView
    android:id="@+id/webView"
    android:layout_width="fill_parent"
    android:layout_height="match_parent"
    android:orientation="vertical" >
```
最後，簡單加上兩行程式碼，就能顯示 Crosswalk 的 WebView。
```java
XWalkView webView = (XWalkView) findViewById(R.id.webView);
webView.load("http://www.google.com", null);
```
使用 setUIClient，可以藉由複寫 onPageLoadStarted 和 onPageLoadStopped 兩個方法，來得知 WebView 開始載入頁面，和已完成頁面載入。
```java
webView.setUIClient(new XWalkUIClient(webView) {

    @Override
    public void onPageLoadStarted(XWalkView view, String url) {
        super.onPageLoadStarted(view, url);
        Log.d(TAG, "onPageLoadStarted");
    }

    @Override
    public void onPageLoadStopped(XWalkView view, String url, LoadStatus status) {
        super.onPageLoadStopped(view, url, status);
        Log.d(TAG, "onPageLoadStopped");
    }
});
```
若複寫 onProgresChanged 方法，則可得知網頁載入的進度。
最後，要在Activity結束後將WebView釋放，以避免記憶體泄漏的問題。
```java
webView.setResourceClient(new XWalkResourceClient(webView) {
    @Override
    public void onProgressChanged(XWalkView view, int progressInPercent) {
        super.onProgressChanged(view, progressInPercent);
        Log.d(TAG, "onProgressChanged = " + progressInPercent);
    }
});
```

最後，要在Activity結束後將WebView釋放，以避免記憶體泄漏的問題。
```java
    @Override
    protected void onPause() {
        super.onPause();
        if (webView != null) {
            webView.pauseTimers();
            webView.onHide();
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
        if (webView != null) {
            webView.resumeTimers();
            webView.onShow();
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (webView != null) {
            webView.onDestroy();
        }
```

# 下载Crosswalk源码

```shell
svn co https://src.chromium.org/svn/trunk/tools/depot_tools/
export PATH=`pwd`/depot_tools:"$PATH"
```

## 开始之前： Android

如果编译的是Android平台上的Crosswalk，你首先应该设置 XWALK_OS_ANDROID环境变量：

`export XWALK_OS_ANDROID=1`

你必须这样做，否则你下载的代码将不会包含某些对于Android环境下编译必要的组件。

如果你在下载源码时没有这个设置，你可以在之后设置并且再执行一次gclient sync，将其它的针对Android平台的组件包含进来。

## 获取源码

创建一个源码目录：

```shell
mkdir crosswalk-src
cd crosswalk-src
```
自动生成gclient的配置文件(.gclient)：

```shell
../depot_tools/gclient config --name=src/xwalk https://github.com/crosswalk-project/crosswalk.git
```

在包含`.gclient`文件的目录下，获取源码：

```shell
../depot_tools/gclient sync
```

追踪一个不同的Crosswalk分支

如果你想要追踪一个不是master的分支（例如beta或者一个稳定分支），你可以使用下面两种方式：

在初次checkout之前设定分支

如果你还没有Crosswalk的源代码，则将你需要checkout的分支的URL传递给gclient config调用。例如，为了追终crosswalk-2 branch:

`gclient config --name=src/xwalk git://github.com/crosswalk-project/crosswalk.git@origin/crosswalk-2`

为一个已经存在的checkout改变分支

如果你已经克隆了一个Crosswalk，并且想改变追踪的分支，你需要新建一个git分支，然后编辑你的`.gclient`文件。

例如，假设你想要追踪crosswalk-2分支。 首先，在你的Crosswalk仓库中新建一个分支：

```shell
cd /path/to/src/xwalk
git checkout -b crosswalk-2 origin/crosswalk-2
```

然后，编辑你的.gclient文件（上面产生的）并且改变入口url。它看上去应该像这样：

"url": "git://github.com/crosswalk-project/crosswalk.git@origin/crosswalk-2",然后，再次同步你的代码：

```shell
gclient sync
```

# 编译Android版Crosswalk

如上面所提到的，Android平台的Crosswalk的编译过程主要基于Chrome的过程，所以需要确定你 对其很熟悉。

为Android平台的Crosswalk安装依赖关系：

```shell
./build/install-build-deps-android.sh
```

配置 gyp。

gyp是在Chromium中使用的编译系统生成器，用于为Ninja和其他系统生成实际的编译文件。为了生成Android环境的编译文件还需要一些变量。

在包含你的.gclient文件的目录下（在src/之上）生成一个名为chromium.gyp_env的文件。

```shell
echo "{ 'GYP_DEFINES': 'OS=android target_arch=ia32', }" > chromium.gyp_env
```

如果你需要编译ARM版，使用target_arch=arm代替上述的 target_arch=ia32。

配置setup来生成Crosswalk项目。

```shell
export GYP_GENERATORS='ninja'
python xwalk/gyp_xwalk
```

通过工具和嵌入式库，编译Android版Crosswalk的主要部分，你可以运行：

`ninja -C out/Release xwalk_core_library`

这样将会在out/Release下创建一个名为xwalk_core_library的目录，目录中包含有特定平台（例如X86或者ARM版）的Crosswalk库，这些库可以将Crosswalk嵌入到项目中。

编译Crosswalk的运行时库（在Crosswalk共享模式下，一个可以作为应用的runtime的APK），运行：

`ninja -C out/Release xwalk_runtime_lib_apk`

这将会在out/Release/apks下生成一个名称为XWalkRuntimeLib.apk的APK。

为了构造一个简单的web应用APK（为了快速安装／目标测试），只要执行：

`ninja -C out/Release xwalk_app_template_apk`

这将会在out/Release/apks下生成一个名称为XWalkAppTemplate.apk的APK。

最终，你也可以通过使用Gradle和Maven构建AAR文件。下列命令将会生成特定平台下的AAR文件，AAR文件包含了Crosswalk库。

`ninja -C out/Release xwalk_core_library_aar xwalk_shared_library_aar`

ARR文件都将位于out/Release目录下。


# 构建桌面版Crosswalk

下面这些步骤涵盖了编译Crosswalk的方方面面。编译的Crosswalk是运行在桌面环境(Windows, Linux,或Mac OS)下的。

gyp是被用于生成Crosswalk项目的工具。这些项目随后会被用作实际代码编译的基础。

为了编译项目，进入src目录并运行

在Linux, Mac操作系统下：

```shell
export GYP_GENERATORS='ninja'
python xwalk/gyp_xwalk
```
在Windows下：
```shell
set GYP_GENERATORS=ninja
python xwalk\gyp_xwalk
```

如果你想创建一个Visual Studio项目，以便使用Visual Studio下编译／编辑Crosswalk，你应该按如下方式设置GYP_GENERATORS：

```shell
set GYP_GENERATORS=ninja,ninja-msvs
```

注意：在Windows下，set在当前的cmd窗口下有效。setx使得在未来的cmd窗口下永久有效，类似于在环境变量对话框内设置变量。

现在你已经使用gyp构建了项目，并且已经准备实际编译。编译Crosswalk启动器（可以运行一个web应用）:
```shell
ninja -C out/Release xwalk
```

测试桌面版Crosswalk

启动一个Crosswalk应用最简单的方式是使用一个应用的manifest作为参数的xwalk命令：

`xwalk /path/to/manifest.json`

Crosswalk将会解析manifest，并且从已经在start_url中定义好的入口处启动应用。

如果你没有任何的HTML应用来测试， Crosswalk样例中包含一些你可以试试。