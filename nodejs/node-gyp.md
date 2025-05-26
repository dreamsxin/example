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
