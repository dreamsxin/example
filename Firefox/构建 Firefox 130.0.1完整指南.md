# 用 155 Toolchain 构建 Firefox 130.0.1 完整指南

> **核心思路**：Firefox 130.0.1 的 `mach bootstrap` 在中文 Windows 11 上存在多个致命 bug，无法直接使用。解决方案是先用最新源码（155.0a1）的 bootstrap 下载完整 toolchain，再切换到 130.0.1 源码，手动修复 4 个兼容性问题后完成 configure 和 build。

`start-shell130.bat`
```cmd
@echo off
setlocal

set "FIREFOX130_SRC=E:\moz\firefox-130.0.1"
set "MOZBUILD_STATE_PATH=E:\moz\.mozbuild130"
set "MOZ_WINDOWS_RS_DIR=E:\moz\.mozbuild130\windows-rs"
set "RUSTUP_TOOLCHAIN=1.79.0-x86_64-pc-windows-msvc"
set "WINSYSROOT=E:\moz\.mozbuild130\vs-14.39.17.9"
set "MOZ_MSVC_TOOLS_VERSION=14.39.33519"
set "MOZ_WINDOWS_SDK_VERSION=10.0.22621.0"
set "VCToolsVersion=%MOZ_MSVC_TOOLS_VERSION%"
set "VCToolsInstallDir=%WINSYSROOT%\VC\Tools\MSVC\%MOZ_MSVC_TOOLS_VERSION%\"
rem Do not pass WindowsSdkDir/WINDOWSSDKDIR together with WINSYSROOT.
rem Firefox's configure derives the SDK location from WINSYSROOT, and treats
rem these environment names case-insensitively on Windows.
set "WINDOWSSDKDIR="
set "WINDOWSSDKVERSION="
set "UNIVERSALCRTSDKDIR="
set "UCRTVERSION="
set "MOZ_CLANG_DIR=E:\moz\.mozbuild130\clang"
set "MOZ_CLANG_VERSION=18.1.7"
set "CBINDGEN=E:\moz\.mozbuild130\cbindgen-0.26.0\bin\cbindgen.exe"
set "MAKENSISU=E:\moz\.mozbuild130\nsis\bin\makensis.exe"
set "MACH_HIDE_DEV_DRIVE_SUGGESTION=1"
set "CC=E:/moz/.mozbuild130/clang/bin/clang-cl.exe"
set "CXX=E:/moz/.mozbuild130/clang/bin/clang-cl.exe"
set "HOST_CC=E:/moz/.mozbuild130/clang/bin/clang-cl.exe"
set "HOST_CXX=E:/moz/.mozbuild130/clang/bin/clang-cl.exe"
set "LINKER=E:/moz/.mozbuild130/clang/bin/lld-link.exe"
set "HOST_LINKER=E:/moz/.mozbuild130/clang/bin/lld-link.exe"
set "AR=E:/moz/.mozbuild130/clang/bin/llvm-lib.exe"
set "HOST_AR=E:/moz/.mozbuild130/clang/bin/llvm-lib.exe"
set "RC=E:/moz/.mozbuild130/clang/bin/llvm-rc.exe"
set "PATH=E:\moz\.cargo\bin;%MOZ_CLANG_DIR%\bin;%VCToolsInstallDir%bin\Hostx64\x64;%WINSYSROOT%\Windows Kits\10\bin\%MOZ_WINDOWS_SDK_VERSION%\x64;%PATH%"

set "BUILD_MODE=%~1"
if "%BUILD_MODE%"=="" set "BUILD_MODE=dbg"

if /I "%BUILD_MODE%"=="dbg" (
  set "MOZCONFIG=E:\moz\firefox-130.0.1\mozconfig-dbg"
) else if /I "%BUILD_MODE%"=="rel" (
  set "MOZCONFIG=E:\moz\firefox-130.0.1\mozconfig-rel"
) else (
  echo Usage: %~nx0 [dbg^|rel]
  exit /b 2
)

if not exist "C:\mozilla-build\start-shell.bat" (
  echo MozillaBuild was not found at C:\mozilla-build.
  exit /b 3
)

if not exist "%MOZ_WINDOWS_RS_DIR%\Cargo.toml" (
  echo windows-rs was not found at %MOZ_WINDOWS_RS_DIR%.
  exit /b 4
)

if not exist "%CBINDGEN%" (
  echo cbindgen 0.26.0 was not found at %CBINDGEN%.
  exit /b 5
)

where rustup >nul 2>nul
if errorlevel 1 (
  echo rustup was not found. Expected E:\moz\.cargo\bin\rustup.exe or rustup on PATH.
  exit /b 6
)

if not exist "%WINSYSROOT%\VC\Tools\MSVC\%MOZ_MSVC_TOOLS_VERSION%\bin\Hostx64\x64\cl.exe" (
  echo MSVC %MOZ_MSVC_TOOLS_VERSION% was not found under %WINSYSROOT%.
  exit /b 7
)

if not exist "%WINSYSROOT%\Windows Kits\10\Include\%MOZ_WINDOWS_SDK_VERSION%\um\windows.h" (
  echo Windows SDK %MOZ_WINDOWS_SDK_VERSION% was not found under %WINSYSROOT%.
  exit /b 8
)

if not exist "%VCToolsInstallDir%include\yvals_core.h" (
  echo MSVC include files were not found at %VCToolsInstallDir%include.
  exit /b 9
)

if not exist "%VCToolsInstallDir%lib\x64\msvcrt.lib" (
  echo MSVC x64 libraries were not found at %VCToolsInstallDir%lib\x64.
  exit /b 10
)

if not exist "%MOZ_CLANG_DIR%\bin\clang-cl.exe" (
  echo Clang %MOZ_CLANG_VERSION% was not found at %MOZ_CLANG_DIR%\bin\clang-cl.exe.
  exit /b 11
)

if not exist "%MOZ_CLANG_DIR%\bin\libclang.dll" (
  echo libclang was not found at %MOZ_CLANG_DIR%\bin\libclang.dll.
  exit /b 12
)

if not exist "%MOZ_CLANG_DIR%\bin\lld-link.exe" (
  echo lld-link was not found at %MOZ_CLANG_DIR%\bin\lld-link.exe.
  exit /b 13
)

if not exist "%MOZ_CLANG_DIR%\bin\llvm-lib.exe" (
  echo llvm-lib was not found at %MOZ_CLANG_DIR%\bin\llvm-lib.exe.
  exit /b 14
)

if not exist "%MOZ_CLANG_DIR%\bin\llvm-rc.exe" (
  echo llvm-rc was not found at %MOZ_CLANG_DIR%\bin\llvm-rc.exe.
  exit /b 15
)

echo Firefox source: %FIREFOX130_SRC%
echo Build mode:      %BUILD_MODE%
echo MOZCONFIG:       %MOZCONFIG%
echo State path:      %MOZBUILD_STATE_PATH%
echo windows-rs:      %MOZ_WINDOWS_RS_DIR%
echo Rust toolchain:  %RUSTUP_TOOLCHAIN%
echo Windows sysroot: %WINSYSROOT%
echo MSVC tools:      %MOZ_MSVC_TOOLS_VERSION%
echo Windows SDK:     %MOZ_WINDOWS_SDK_VERSION%
echo Clang:           %MOZ_CLANG_DIR%
echo cbindgen:        %CBINDGEN%
echo makensis:        %MAKENSISU%

call "C:\mozilla-build\start-shell.bat" -where "%FIREFOX130_SRC%"
exit /b %ERRORLEVEL%
```

## 配置过程
已为 Firefox 130 建立独立编译环境，未修改 Firefox 源码去兼容 `windows-rs 0.62.2`。

**目录结构**

```text
E:\moz\.mozbuild                    其他 Firefox 版本继续使用
E:\moz\.mozbuild130                 Firefox 130 专用
├─ windows-rs                       windows 0.52.0，实际使用
├─ windows-rs-0.62.2                原 0.62.2 备份
├─ windows-0.52.0.crate             下载的官方 crate
├─ srcdirs                          Firefox 130 新虚拟环境
└─ srcdirs-copied-backup            复制过来的旧虚拟环境备份
```

**1. 复制编译工具链**

原目录约 `13.65 GB`、32,011 个文件：

```powershell
robocopy E:\moz\.mozbuild E:\moz\.mozbuild130 `
  /E /COPY:DAT /DCOPY:DAT /R:2 /W:2 /MT:16
```

复制后核对文件数量和容量一致。

**2. 下载 windows-rs 0.52.0**

Firefox 130 源码指定的下载信息位于：

```text
taskcluster\kinds\fetch\toolchains.yml
```

下载地址：

```text
https://static.crates.io/crates/windows/windows-0.52.0.crate
```

下载命令：

```powershell
curl.exe -L --fail --retry 3 `
  --user-agent "Mozilla/5.0" `
  --output E:\moz\.mozbuild130\windows-0.52.0.crate `
  https://static.crates.io/crates/windows/windows-0.52.0.crate
```

Firefox 源码记录的 SHA-256：

```text
e48a53791691ab099e5e2ad123536d0fff50652600abaf43bbf952894110d0be
```

实际下载文件校验结果完全一致。

**3. 安装 0.52.0 并保留 0.62.2**

```powershell
Move-Item `
  E:\moz\.mozbuild130\windows-rs `
  E:\moz\.mozbuild130\windows-rs-0.62.2

New-Item -ItemType Directory `
  E:\moz\.mozbuild130\windows-0.52.0

tar.exe -xf E:\moz\.mozbuild130\windows-0.52.0.crate `
  -C E:\moz\.mozbuild130\windows-0.52.0

Move-Item `
  E:\moz\.mozbuild130\windows-0.52.0\windows-0.52.0 `
  E:\moz\.mozbuild130\windows-rs
```

最终版本：

```text
E:\moz\.mozbuild130\windows-rs             0.52.0
E:\moz\.mozbuild130\windows-rs-0.62.2      0.62.2
```

**安装cbindgen 0.26.0**
Firefox 130 对 cbindgen 的实际兼容版本是 `0.26.0`。之前 `E:\moz\.mozbuild130\cbindgen\cbindgen.exe` 是 `0.29.4`，会生成错误的 WebRender 头文件，出现：

```text
use of undeclared identifier 'COUNT'
BudgetType_VALUES[COUNT]
```

**安装位置**

cbindgen 0.26.0 安装到了：

```text
E:\moz\.mozbuild130\cbindgen-0.26.0\bin\cbindgen.exe
```

来源是 Cargo 从 crates.io 安装，不是手工下载 zip：

```powershell
E:\moz\.cargo\bin\cargo.exe install cbindgen --version 0.26.0 --root E:\moz\.mozbuild130\cbindgen-0.26.0 --locked
```

验证结果：

```text
cbindgen 0.26.0
```

WASI SDK 22.0
Clang/libc++/compiler-rt 18.x
Source: https://github.com/WebAssembly/wasi-sdk/releases/tag/wasi-sdk-22

**环境变量配置**

已写入 [start-shell130.bat](E:/moz/start-shell130.bat:7)：

```bat
set "CBINDGEN=E:\moz\.mozbuild130\cbindgen-0.26.0\bin\cbindgen.exe"
```

脚本里也加了存在性检查：

```bat
if not exist "%CBINDGEN%" (
  echo cbindgen 0.26.0 was not found at %CBINDGEN%.
  exit /b 5
)
```

**4. 重建 Mach 虚拟环境**

复制来的 `srcdirs` 虚拟环境不可直接复用，因此保留备份并让 Mach 重建：

```powershell
Move-Item `
  E:\moz\.mozbuild130\srcdirs `
  E:\moz\.mozbuild130\srcdirs-copied-backup

New-Item -ItemType Directory `
  E:\moz\.mozbuild130\srcdirs
```

**5. 独立启动脚本**

已创建 [start-shell130.bat](E:/moz/start-shell130.bat)，主要环境变量如下：

```bat
set "FIREFOX130_SRC=E:\moz\firefox-130.0.1"
set "MOZBUILD_STATE_PATH=E:\moz\.mozbuild130"
set "MOZ_WINDOWS_RS_DIR=E:\moz\.mozbuild130\windows-rs"
set "MAKENSISU=E:\moz\.mozbuild130\nsis\bin\makensis.exe"
set "MACH_HIDE_DEV_DRIVE_SUGGESTION=1"
```

其中 `MAKENSISU` 必须明确指向 `.exe`。NSIS 目录同时存在 Linux ELF 格式的 `makensis` 和 Windows 的 `makensis.exe`，不指定时 Firefox 130 会错误选中 ELF 文件。

使用方式：

```bat
E:\moz\start-shell130.bat dbg
```

或发布构建：

```bat
E:\moz\start-shell130.bat rel
```

不传参数时默认使用 `mozconfig-dbg`。

进入 Shell 后：

```bash
./mach configure
./mach build
```

`./mach configure` 已验证成功，日志确认：

```text
checking for the windows rust crate source...
E:\moz\.mozbuild130\windows-rs
```

`./mach build pre-export` 后续验证被中断，尚未确认完成。

---

下面是 Firefox 130.0.1 在 Windows 上最终可用的完整编译环境配置，已包含最新修正的 WASI sysroot。

**目录规划**

```text
源码：
E:\moz\firefox-130.0.1

Firefox 130 专用工具链：
E:\moz\.mozbuild130

启动脚本：
E:\moz\start-shell130.bat

Debug 输出：
E:\moz\firefox-130.0.1\obj-ff-dbg

Release 输出：
E:\moz\firefox-130.0.1\obj-ff-rel
```

`.mozbuild130` 可以先从已有环境复制，但复制后必须替换版本不匹配的工具：

```powershell
robocopy E:\moz\.mozbuild E:\moz\.mozbuild130 /E /COPY:DAT /R:2 /W:1
```

**最终工具版本**

| 工具 | 最终版本 |
|---|---|
| MozillaBuild | 已安装于 `C:\mozilla-build` |
| Rust | `1.79.0-x86_64-pc-windows-msvc` |
| Cargo | `1.79.0` |
| Clang/LLVM | `18.1.7` |
| MSVC | `14.39.33519` |
| Windows SDK | `10.0.22621.0` |
| cbindgen | `0.26.0` |
| windows-rs | `0.52.0` |
| WASI SDK | `22.0` |
| WASI libc++ | `18.1.0` |
| NSIS | `3.07` |
| NASM | `3.02` |
| GNU Make/mozmake | `4.4.1` |

**下载地址**

- [MozillaBuild 官方说明](https://wiki.mozilla.org/MozillaBuild)
- [Visual Studio Build Tools](https://aka.ms/vs/17/release/vs_BuildTools.exe)
- [Rustup Windows x64](https://static.rust-lang.org/rustup/dist/x86_64-pc-windows-msvc/rustup-init.exe)
- [LLVM 18.1.7 发布页](https://github.com/llvm/llvm-project/releases/tag/llvmorg-18.1.7)
- [LLVM 18.1.7 Windows 安装包](https://github.com/llvm/llvm-project/releases/download/llvmorg-18.1.7/LLVM-18.1.7-win64.exe)
- [cbindgen 0.26.0](https://crates.io/crates/cbindgen/0.26.0)
- [windows-rs 0.52.0](https://crates.io/crates/windows/0.52.0)
- [windows-rs 0.52.0 源码包](https://static.crates.io/crates/windows/windows-0.52.0.crate)
- [WASI SDK 22 发布页](https://github.com/WebAssembly/wasi-sdk/releases/tag/wasi-sdk-22)
- [WASI sysroot 22.0](https://github.com/WebAssembly/wasi-sdk/releases/download/wasi-sdk-22/wasi-sysroot-22.0.tar.gz)
- [WASI compiler-rt 22.0](https://github.com/WebAssembly/wasi-sdk/releases/download/wasi-sdk-22/libclang_rt.builtins-wasm32-wasi-22.0.tar.gz)
- [NASM 3.02](https://www.nasm.us/pub/nasm/releasebuilds/3.02/win64/nasm-3.02-win64.zip)
- [NSIS 下载页](https://nsis.sourceforge.io/Download)

**Rust 配置**

```bash
rustup toolchain install 1.79.0-x86_64-pc-windows-msvc
rustup component add rust-src --toolchain 1.79.0-x86_64-pc-windows-msvc

rustc +1.79.0-x86_64-pc-windows-msvc --version
cargo +1.79.0-x86_64-pc-windows-msvc --version
```

启动脚本中固定：

```bat
set RUSTUP_TOOLCHAIN=1.79.0-x86_64-pc-windows-msvc
```

**Clang 18.1.7**

安装到：

```text
E:\moz\.mozbuild130\clang-18.1.7
```

建立稳定目录别名：

```cmd
mklink /J E:\moz\.mozbuild130\clang E:\moz\.mozbuild130\clang-18.1.7
```

不要求真实目录必须叫 `clang`，但使用 junction 后 Firefox 和脚本始终引用：

```text
E:\moz\.mozbuild130\clang\bin\clang-cl.exe
```

**Visual Studio sysroot**

最终使用：

```text
E:\moz\.mozbuild130\vs-14.39.17.9
```

关键版本：

```text
MSVC：14.39.33519
Windows SDK：10.0.22621.0
```

使用 Firefox 自带配置固定下载：

```bash
cd /e/moz/firefox-130.0.1

./mach python taskcluster/scripts/misc/get_vs.py \
  build/vs/vs2022.yaml \
  /e/moz/.mozbuild130/vs-14.39.17.9
```

这比直接使用系统中可能更新到 `14.51` 的 Visual Studio 工具链更稳定。

**cbindgen 0.26.0**

```cmd
cargo +1.79.0 install --locked --version 0.26.0 cbindgen ^
  --root E:\moz\.mozbuild130\cbindgen-0.26.0
```

最终路径：

```text
E:\moz\.mozbuild130\cbindgen-0.26.0\bin\cbindgen.exe
```

这解决了生成的 `webrender_ffi_generated.h` 中：

```text
BudgetType_VALUES[COUNT]
use of undeclared identifier 'COUNT'
```

**windows-rs 0.52.0**

Firefox 130 要求 `0.52.0`，不能使用 `.mozbuild` 中较新的 `0.62.2`。

最终目录：

```text
E:\moz\.mozbuild130\windows-rs
```

对应环境变量：

```bat
set MOZ_WINDOWS_RS_DIR=E:\moz\.mozbuild130\windows-rs
```

确认版本：

```powershell
Select-String `
  E:\moz\.mozbuild130\windows-rs\Cargo.toml `
  -Pattern '^version'
```

**WASI 工具链**

之前的目录包含 LLVM 21 libc++：

```text
_LIBCPP_VERSION 210108
```

但编译器是 Clang 18.1.7，因此出现：

```text
use of undeclared identifier '__builtin_ctzg'
```

最终改成 WASI SDK 22：

```text
Clang：18.1.7
libc++：18.1.0
compiler-rt：SDK 22 配套版本
```

最终 sysroot：

```text
E:\moz\.mozbuild130\sysroot-wasm32-wasi
```

旧目录备份：

```text
E:\moz\.mozbuild130\sysroot-wasm32-wasi-llvm21.backup
```

WASI builtins 放到：

```text
E:\moz\.mozbuild130\clang\lib\clang\18\lib\wasi\
libclang_rt.builtins-wasm32.a
```

验证结果：

```text
_LIBCPP_VERSION 180100
clang version 18.1.7
WASI C++ 编译、链接测试通过
```

不要修改 libc++ 的 `countr.h` 来绕过错误。

**Rust 原生依赖修正**

Cargo 内部的 `cc-rs` 可能绕过主构建参数，误用系统最新 MSVC STL。为此在：

```text
E:\moz\firefox-130.0.1\config\makefiles\rust.mk
```

给 Rust 调用的 clang-cl 参数补上：

```text
/winsysroot E:\moz\.mozbuild130\vs-14.39.17.9
/vctoolsversion 14.39.33519
/winsdkversion 10.0.22621.0
```

这解决了：

```text
cannot find type `_Val_types`
STL1000: Unexpected compiler version
```

**start-shell130.bat 核心配置**

```bat
@echo off

set FIREFOX130_SRC=E:\moz\firefox-130.0.1
set MOZBUILD_STATE_PATH=E:\moz\.mozbuild130
set MOZ_WINDOWS_RS_DIR=E:\moz\.mozbuild130\windows-rs

set RUSTUP_TOOLCHAIN=1.79.0-x86_64-pc-windows-msvc

set WINSYSROOT=E:\moz\.mozbuild130\vs-14.39.17.9
set MOZ_MSVC_TOOLS_VERSION=14.39.33519
set MOZ_WINDOWS_SDK_VERSION=10.0.22621.0

set VCToolsVersion=14.39.33519
set VCToolsInstallDir=%WINSYSROOT%\VC\Tools\MSVC\14.39.33519\

set MOZ_CLANG_DIR=E:\moz\.mozbuild130\clang
set MOZ_CLANG_VERSION=18.1.7

set CBINDGEN=E:\moz\.mozbuild130\cbindgen-0.26.0\bin\cbindgen.exe
set MAKENSISU=E:\moz\.mozbuild130\nsis\bin\makensis.exe

set CC=E:/moz/.mozbuild130/clang/bin/clang-cl.exe
set CXX=E:/moz/.mozbuild130/clang/bin/clang-cl.exe
set HOST_CC=E:/moz/.mozbuild130/clang/bin/clang-cl.exe
set HOST_CXX=E:/moz/.mozbuild130/clang/bin/clang-cl.exe

set LINKER=E:/moz/.mozbuild130/clang/bin/lld-link.exe
set HOST_LINKER=E:/moz/.mozbuild130/clang/bin/lld-link.exe
set AR=E:/moz/.mozbuild130/clang/bin/llvm-lib.exe
set HOST_AR=E:/moz/.mozbuild130/clang/bin/llvm-lib.exe
set RC=E:/moz/.mozbuild130/clang/bin/llvm-rc.exe

set WINDOWSSDKDIR=
set WINDOWSSDKVERSION=
set UNIVERSALCRTSDKDIR=
set UCRTVERSION=

set PATH=E:\moz\.cargo\bin;%MOZ_CLANG_DIR%\bin;%VCToolsInstallDir%bin\Hostx64\x64;%WINSYSROOT%\Windows Kits\10\bin\10.0.22621.0\x64;%PATH%

if /I "%1"=="rel" (
  set MOZCONFIG=%FIREFOX130_SRC%\mozconfig-rel
) else (
  set MOZCONFIG=%FIREFOX130_SRC%\mozconfig-dbg
)

call C:\mozilla-build\start-shell.bat
```

**编译命令**

Debug：

```cmd
E:\moz\start-shell130.bat dbg
```

```bash
cd /e/moz/firefox-130.0.1
./mach build
```

Release：

```cmd
E:\moz\start-shell130.bat rel
```

```bash
cd /e/moz/firefox-130.0.1
./mach build
```

当前最终组合是：

```text
Firefox 130.0.1
+ Rust 1.79.0
+ Clang 18.1.7
+ MSVC 14.39
+ Windows SDK 22621
+ cbindgen 0.26.0
+ windows-rs 0.52.0
+ WASI libc++ 18.1.0
```
