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

https://firefox-source-docs.mozilla.org/setup/windows_build.html

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
