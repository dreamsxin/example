## 

https://github.com/zen-browser/desktop

https://github.com/BrowserWorks/Waterfox

https://github.com/Floorp-Projects/Floorp

https://github.com/OtterBrowser/otter-browser

https://github.com/LadybirdBrowser/ladybird

## 指纹浏览器

**camoufox**
https://github.com/daijro/camoufox

## 编译

- https://firefox-source-docs.mozilla.org/setup/windows_build.html
- https://firefox-source-docs.mozilla.org/build/buildsystem/visualstudio.html

### 环境

**Mercurial**
https://tortoisehg.bitbucket.io/
https://www.mercurial-scm.org/downloads

**Python3.1**
https://www.python.org/downloads/release/python-3119/

**MozillaBuild**
https://ftp.mozilla.org/pub/mozilla/libraries/win32/MozillaBuildSetup-Latest.exe

### 编译
运行 mozilla-build
```shell
C:\mozilla-build\start-shell.bat
```
在 mozilla-build 提示窗口中输入
```shell
# Using the C:\mozilla-build\start-shell.bat shell from step 1:
cd c:/
mkdir mozilla-source
cd mozilla-source
wget https://hg.mozilla.org/mozilla-central/raw-file/default/python/mozboot/bin/bootstrap.py

# To use Git as your VCS
python3 bootstrap.py --vcs=git

# To use Mercurial as your VCS
python3 bootstrap.py
```

```shell
./mach configure
./mach build-backend -b VisualStudio
./mach build
```

## 目录说明

Mozilla 家族所有的项目代码（如 Firefox, Thunderbird, etc.）都被整合在一个单独的源码树中。其中既包括源代码，也包括在所支持的平台（ Linux, Windows, OS X 等）构建工程所依赖的代码。 

要想简单的对 Mozilla 源码进行大体的浏览， 你并不需要将它下载下来。你可以直接借助 MXR（对 HEAD 分支上的整个 mozilla 源码从 https://mxr.mozilla.org/mozilla-central/source/ 地址作为起始点 在浏览器上直接对代码进行浏览。

为了能够修改代码， 你必须通过下载一个源代码 snapshot 或  checking out the current sources from Mercurial 的方式获取它。

本文则描述了可以通过  client.mk pulled 到本地的目录结构。例如，  当前使用的文件夹只有会有一些 Mozilla 项目客户端产品。其他的文件夹在 Mozilla CVS 库中，例如 web 工具或 web 经典代码库。

See source code directories overview for a somewhat different (older) version of the same information. Also see the more detailed overview of the pieces of Gecko.

- accessible
Files for accessibility (i.e., MSAA (Microsoft Active Accessibility), ATK (Accessibility Toolkit, used by GTK+ 2) support files). See Accessibility.

- addon-sdk
The add-on SDK for creating Firefox add-ons using standard Web Technologies.

- b2g
在构建  FirefoxOS 时，需用到的 Boot2Gecko 项目的源代码。

- browser
包括了 Firefox 浏览器的前端代码 (in XUL, Javascript, XBL, 和 C++)。许多文件都是从作为 xpfe/ 文件的备份开始的。

- browser/devtools
包括了 DevTools 的前端代码(Scratchpad, Style Editor...). 可参考 toolkit/devtools。

- browser/extensions
包括了 PDF.js 和 Shumway 内嵌扩展

- browser/themes
包括了针对每一个 OS（linux， Mac， windows） 包装浏览器的图片和 CSS 文件。

- build
用于构建过程的各式各样的文件。请参照 config/.

- caps
Capability-based web page security management. Itcontains C++ interfaces and code for determining the capabilities of content based on the security settings or certificates (e.g., VeriSign). See Component Security.

- chrome
Chrome registry 与toolkit/一起使用。这些文件是rdf/chrome/目录文件的原始拷贝。

- config
在构建过程中，makefile 中的通用部分等使用的更多的文件。请参照  build/.

- content
表示 web 页面（HTML，SVG，XML 文档，元素，文本节点，等）的数据结构。这些对象包含了许多 DOM 接口的实现，同时也实现了和这些对象相关的行为，例如链接处理，表单控制行为以及表单提交。

这个目录同样包含了  XUL, XBL, XTF, <canvas> 的代码，以及实现 XSLT 及事件处理的代码。

- db
数据库访问模型的容器

- db/sqlite3
SQLite 数据库，在 storage 中使用。

- docshell
Implementation of the docshell, the main object managing things related to a document window. Each frame has its own docshell. It contains methods for loading URIs, managing URI content listeners, etc. It is the outermost layer of the embedding API used to embed a Gecko browser into an application. See also webshell/.

- dom
由 DOM 规范所指定的接口的 IDL 定义 以及对这些接口的 Mozilla 扩展（主要是在 content/ 文件夹下对这些接口的实现，但不完全是 )。
在 JavaScript 与 DOM 实现间的连接部分是对 JavaScript 和 DOM 特有的。 (The parts that are not DOM-specific, i.e., the generic binding between XPCOM and JavaScript, live in js/src/xpconnect/.)
对一些核心的核心  "DOM Level 0" 对象的实现，如window, window.navigator, window.location, 等.
- editor
The editor directory contains C++ interfaces, C++ code, and XUL/Javascript for the embeddable editor component, which is used for the HTML Editor("Composer"), for plain and HTML mail composition, and for text fields and text areas throughout the product. The editor is designed like a "browser window with editing features": it adds some special classes for editing text and managing transaction undo/redo, but reuses browser code for nearly everything else.

- Contains:

The backend for HTML and text editing. This is not only used for the mail composer and the page editor composer, but also for rich text editing inside webpages. Textarea and input fields are using such an editor as well (in the plaintext variant). See Mozilla Editor.
The frontend for the HTML editor that is part of SeaMonkey.
- embedding
XXX this needs a description. See Embedding.

- extensions
Contains several extensions to mozilla, which can be enabled at compile-time using the --enable-extensions configure argument.

Note that some of these are now built specially and not using the --enable-extensions option. For example, disabling xmlextras is done using --disable-xmlextras.

- extensions/auth
Implementation of the negotiate auth method for HTTP and other protocols. Has code for SSPI, GSSAPI, etc. See Integrated Authentication.

- extensions/content-packs
Content- and locale-pack switching user interface.

- extensions/cookie
Permissions backend for cookies, images, etc., as well as the user interface to these permissions and other cookie features.

- extensions/cview
Component viewer, which allows to view the currently registered components and interfaces.

- extensions/datetime
datetime协议的支持

- extensions/finger
finger协议的支持

- extensions/gnomevfs
gnome-vfs接口，被允许使用Mozilla内部gnome-vfs支持的所有协议。

- extensions/help
Help viewer and help content. See Mozilla Help Viewer Project.

- extensions/irc
ChatZilla，IRC(Internet Relay Chat)的组件。请参考ChatZilla.

- extensions/java
XPCOM<->Java bridge ("JavaXPCOM", formerly known as Javaconnect). Unrelated to the code in java/.

- extensions/layout-debug
布局调试器。viewer的替代者。被用于执行布局回归测试和支持其它布局调试特性。

- extensions/lightning
The Lightning project, "an extension to tightly integrate calendar functionality (scheduling, tasks, etc.) into Thunderbird." See Lightning.

- extensions/mono
一座在CLR/.NET/Mono/C#/etc.世界和XPCOM之间的双向桥。

- extensions/p3p
W3C平台Privacy Preferences standard的实现.请见 Platform for Privacy Preferences (P3P).

- extensions/pref
Preference相关的扩展

- extensions/python
支持使用python实现XPCOM组件。请见PyXPCOM.

- extensions/reporter
上报Web页面问题的一个扩展.客户端部分.请见Mozilla Reporter.

- extension/schema-validation
Allows to validate XML trees according to specified XML Schemas.

- extension/spatialnavigation
Spatial navigation, navigating between links of a document according to their on-screen position. See Spatial Navigation.

- extensions/spellcheck
mailnews和composer的拼写检查程序.

- extensions/sql
支持XUL程序访问SQL数据库.请见SQL Support in Mozilla.

- extensions/sroaming
Session roaming,例如,支持存储一个profile到远端服务器.
- extensions/transformiix
XSLT支持. XSL Transformations是一种用于将XML文档转换为其它XML文档的语言.请见XSL Transformations.

- extensions/tridentprofile
针对微软IE浏览器的一些重要的profiles的支持.微软IE浏览器的内核为Trident.

- extensions/typeaheadfind
Find As You Type allows quick web page navigation when you type a succession of characters in the body of the displayed page. See Find As You Type.

- extensions/universalchardet
统一字符集侦测器

- extensions/venkman
JavaScript调试器(JavaScript Debugger)

- extensions/wallet
密码和Form管理器.

- extensions/webdav
WebDAV code; exposes special APIs for accessing WebDAV servers. Used by the Calendar project.

- extensions/webservices
Webservices的支持. 请见Web Services.

- extensions/xforms
XForms扩展代码.请见XForms.

- extensions/xmlextras
一些XML相关的扩展.请见XML Extras.

- extensions/xml-rpc
XML远程过程调用. 目前未被承认. 请见XML-RPC in Mozilla.

- extensions/xmlterm
使用Mozilla技术实现的一种终端(terminal)。只有在GTK版本上可用。

- gfx
Contains interfaces that abstract the capabilities of platform specific graphics toolkits, along with implementations on various platforms. These interfaces provide methods for things like drawing images, text, and basic shapes. It also contains basic data structures such as points and rectangles used here and in other parts of Mozilla.

It is also the home of the new graphics architecture based on cairo (via a C++ wrapper called thebes). See NewGFXAPIs and GFXEvolution.

- hal
包括平台特定的功能，例如获取 Windows 中的电池状态等。

- image
图片渲染库。包括 mozilla 所支持的图片格式的解码器。 

- intl
国际化和本地化支持，请参考  Internationalisation Projects.

- intl/chardet
用于嗅探Web页面字符编码的代码。

- intl/ctl
用于处理复杂文本布局，与南亚语言相关的字形(not built by default, needs --enable-ctl)。

- intl/locale
Code related to determination of locale information from the operating environment.

- intl/lwbrk
Code related to line breaking and word breaking.

- intl/strres
Code related to string resources used for localization.

- intl/uconv
Code that converts (both ways: encoders and decoders) between UTF-16 and many other character encodings.

- intl/unicharutil
Code related to implementation of various algorithms for Unicode text, such as case conversion.

- ipc
实现IPC(进程间通信)的容器。

- js
- js/src
JavaScript 引擎，也称为 SpiderMonkey. 请参照 JavaScript.

- js/jsd
JavaScript 调试库。请参考  JavaScript Debugging.

- js/xpconnect
xpcponnect是支持C++代码中调用JavaScript代码及从JavaScript代码中调用C++代码的支持代码，其使用XPCOM接口。请参考XPConnect.

- layout
Code that implements a tree of rendering objects that describe the types and locations of the objects that are displayed on the screen (such as CSS boxes, tables, form controls, XUL boxes, etc.), and code that manages operations over that rendering tree (such as creating and destroying it, doing layout, painting, and event handling). See documentation and other information.

- layout/base
处理渲染树(rendering tree)的代码。

layout/generic
The basic rendering object interface and the rendering tree objects for basic CSS boxes.

- layout/forms
渲染树对象：HTML form控件。

- layout/tables
渲染树对象：CSS/HTML表单。

- layout/mathml
渲染树对象：MathML(数学标记语言).

- layout/svg
渲染树对象：SVG.

- layout/xul
附加的渲染对象接口XUL和渲染树对象XUL boxes.

- media
包含使用media库的源码(例如: libpng)

- memory
针对memalloc函数等的跨平台封装。

- mfbt
实现类似WeakPtr的类。跨平台断言等。

- modules
modules/libjar
用于读取mozilla前端.jar文件的库。

- modules/libpref
用于读/写preferences的库

- modules/zlib
zlib源码，至少在网络库中用于压缩传输。

- mozglue
XXX this needs a description.

- netwerk
Networking library, also known as Necko. Responsible for doing actual transfers from and to servers, as well as for URI handling and related stuff. See also Network library documentation.

- nsprpub
Netscape Portable Runtime. Used as an abstraction layer to things like threads, file I/O, and socket I/O. See Netscape Portable Runtime.

- nsprpub/lib
通常不使用；也许可能用于Mac？

- other-licenses
包含MPL没有覆盖到的，但被用于其它一些Mozilla代码的库。

- parser
一组用于解析基于XML/HTML文件的数据结构和函数。

parser/expat
Mozilla使用的XML解析器expat源码的拷贝。

- parser/html
HTML解析器(除about:blank)。

- parser/xml
集成expat(parser/expat)到Gecko的代码。

- parser/htmlparser
废弃的HTML解析器，仍用于about:blank。在XML解析时，被用于管理网络字节流到Unicode间的转换。

- probes
XXX this needs a description.

- profile
profile处理，profile manager后端和前端代码。

- python
- python/mach
Mach工具代码

- rdf
RDF handling APIs. See RDF and RDF. Also contains the chrome registry code used by SeaMonkey, although toolkit apps (such as Firefox and Thunderbird) use the copy forked into chrome.

- security
包含NSS和PSM，支持加密功能(如S/MIME，SSL等)。请参阅Network Security Services (NSS) 和 Personal Security Manager (PSM).

- services
XXX this needs a description.

- startupcache
XXX this needs a description.

- storage
Storage: sqlite的XPCOM封装.统一存储所有profile相关的数据.取代mork.请参阅Unified Storage.

- testing
Mozilla代码库项目的通用测试工具(自动化测试套件、有趣的程序)

- toolkit
The "new toolkit" used by Thunderbird, Firefox, etc. This contains numerous front-end components shared between applications as well as most of the XBL-implemented parts of the XUL language (most of which was originally forked from versions in xpfe/).

- toolkit/mozapps/extensions/test/xpinstall
The installer, which contains code for installing Mozilla and for installing XPIs/extensions. This directory also contains code needed to build installer packages. See XPInstall and the XPInstall project page.

- tools
Mozilla编译过程中可选的一些工具，主要用于调试。

- uriloader
XXX this needs a description.

- uriloader/base
Content dispatch in Mozilla. Used to load uris and find an appropriate content listener for the data. Also manages web progress notifications. See Document Loading: From Load Start to Finding a Handler and The Life Of An HTML HTTP Request.

- uriloader/exthandler
Used to handle content that Mozilla can't handle itself. Responsible for showing the helper app dialog, and generally for finding information about helper applications.

- uriloader/prefetch
Service to prefetch documents in order to have them cached for faster loading.

- view
View管理器。包含用于绘图、滚动、事件处理、窗口z-order堆叠次序和透明度的跨平台代码。以后会逐步废弃。

- webapprt
XXX this needs a description.

- widget
针对每个平台实现的跨平台 API， 用来处理操作系统/环境组件，例如创建和处理窗口，弹出框以及其他本地组件相关的代码，以及将与绘制和事件相关的系统消息转化为用于 Mozilla 其他地方的消息（例如， view/ 和 content/，后者会转化许多消息到另外的 API，DOM 事件 API）。

- xpcom
跨平台组件对象模型 。也包含被其他 Mozilla 代码使用的数据结构。请参考 XPCOM Project.

- xpfe
XPFE (跨平台前端) 是指SeaMonkey的前端. 它包含XUL文件(这些XUL文件被其它Mozilla套件用于浏览器接口、通用文件目的)和XBL文件(XUL语言的部分被实现在XBL内)。这些代码的大部分被拷贝到browser/和 toolkit/下，用于Firefox、Thunderbird等。

- xpfe/components
用于Mozilla前端的组件，就像Mozilla期望的其它部分接口的实现。

- xulrunner
XULRunner 代码，请参照 XULRunner.
