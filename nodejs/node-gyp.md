- https://www.npmjs.com/package/@electron/node-gyp

You can install node-gyp using npm:
```shell
npm install -g node-gyp
```
Depending on your operating system, you will need to install:

## On Unix

A supported version of Python
make
A proper C/C++ compiler toolchain, like GCC

## On macOS

A supported version of Python
Xcode Command Line Tools which will install clang, clang++, and make.
Install the Xcode Command Line Tools standalone by running xcode-select --install. -- OR --
Alternatively, if you already have the full Xcode installed, you can install the Command Line Tools under the menu Xcode -> Open Developer Tool -> More Developer Tools....

## On Windows

Install tools with Chocolatey:
```shell
choco install python visualstudio2022-workload-vctools -y
```

## 安装C/C++编译工具。

```shell
powershell -ExecutionPolicy Unrestricted -Command "Add-Type -Path 'D:\nodejs\node_modules\npm\node_modules\node-gyp\lib\Find-VisualStudio.cs'; [VisualStudioConfiguration.Main]::PrintJson()"
```

1.如果你是.Net开发者，或者说你是用Visual Studio开发的开发人员。按照以下步骤进行设置

```shell
npm config set msvs_version 2019
```

2.如果你是VS2017及以下版本，此步骤可省略

因为VS2019的MSBuild.exe文件和之前版本的所在位置不同，因此，我们需要亲自指定【注意你的VS版本，可能与我的路径并不相同】

```shell
npm config set msbuild_path "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\MSBuild\Current\Bin\M
```
