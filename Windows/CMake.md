# CMake

使用 CMake GUI

- 读取 CMakeLists.txt
- 配置 CMake cache
- 选择编译器/生成器
- 生成 Visual Studio .sln / Makefile / Ninja 工程

**最小 C++ 项目结构**

```text
mycpp/
  CMakeLists.txt
  main.cpp
```

`main.cpp`：

```cpp
#include <iostream>

int main() {
    std::cout << "hello cmake\n";
    return 0;
}
```

`CMakeLists.txt`：

```cmake
cmake_minimum_required(VERSION 3.20)

project(MyCppProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(MyCppProject main.cpp)
```

**用 CMake GUI 生成 Visual Studio 工程**

1. 打开 `cmake-gui`
2. `Where is the source code` 选择：
   ```text
   D:/path/to/mycpp
   ```
3. `Where to build the binaries` 选择：
   ```text
   D:/path/to/mycpp/out
   ```
4. 点 `Configure`
5. 选择生成器，例如：
   ```text
   Visual Studio 17 2022
   ```
6. 平台选择：
   ```text
   x64
   ```
7. 再点一次 `Configure`
8. 点 `Generate`

生成后会得到：

```text
mycpp/out/MyCppProject.sln
```

然后用 Visual Studio 打开 `.sln` 即可编译。

**带库项目示例**

如果你要生成 DLL：

```cmake
cmake_minimum_required(VERSION 3.20)

project(MySdk LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

add_library(MySdk SHARED
    sdk.cpp
    sdk.h
)
```

如果要生成静态库：

```cmake
add_library(MySdk STATIC
    sdk.cpp
    sdk.h
)
```

如果要多个子目录：

```text
mycpp/
  CMakeLists.txt
  src/
    CMakeLists.txt
    main.cpp
```

根 `CMakeLists.txt`：

```cmake
cmake_minimum_required(VERSION 3.20)
project(MyCppProject LANGUAGES CXX)

add_subdirectory(src)
```

`src/CMakeLists.txt`：

```cmake
add_executable(MyCppProject main.cpp)
```

所以流程是：

```text
先写 CMakeLists.txt
再用 CMake GUI 配置
最后 Generate 生成 .sln
```

# 下面是从零创建一个最小 C++ `.sln` 项目的教程，使用 **CMake + CMake GUI + Visual Studio 2022**。

**1. 创建目录**

例如创建：

```text
D:\demo\hello-cmake
```

目录结构：

```text
hello-cmake/
  CMakeLists.txt
  main.cpp
```

**2. 创建 main.cpp**

文件：[main.cpp](D:/demo/hello-cmake/main.cpp)

```cpp
#include <iostream>

int main() {
    std::cout << "Hello CMake SLN!" << std::endl;
    return 0;
}
```

**3. 创建 CMakeLists.txt**

文件：[CMakeLists.txt](D:/demo/hello-cmake/CMakeLists.txt)

```cmake
cmake_minimum_required(VERSION 3.20)

project(HelloCMake LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(HelloCMake main.cpp)
```

含义：

```cmake
cmake_minimum_required(VERSION 3.20)
```

要求 CMake 最低版本。

```cmake
project(HelloCMake LANGUAGES CXX)
```

定义项目名，生成的 `.sln` 默认叫 `HelloCMake.sln`。

```cmake
set(CMAKE_CXX_STANDARD 20)
```

使用 C++20。

```cmake
add_executable(HelloCMake main.cpp)
```

生成一个可执行程序 `HelloCMake.exe`。

**4. 用 CMake GUI 生成 sln**

打开 `cmake-gui`。

设置：

```text
Where is the source code:
D:/demo/hello-cmake

Where to build the binaries:
D:/demo/hello-cmake/out
```

点击 `Configure`。

第一次会弹出生成器选择窗口，选择：

```text
Visual Studio 17 2022
```

平台选择：

```text
x64
```

然后点 `Finish`。

如果配置成功，再点一次：

```text
Configure
```

最后点击：

```text
Generate
```

**5. 打开 sln**

生成后会出现：

```text
D:\demo\hello-cmake\out\HelloCMake.sln
```

用 Visual Studio 打开它。

**6. 编译运行**

在 Visual Studio 中：

1. 选择配置：`Debug`
2. 选择平台：`x64`
3. 右键 `HelloCMake`
4. 点击 `设为启动项目`
5. 按 `Ctrl + F5` 运行

也可以用命令行编译：

```powershell
cd D:\demo\hello-cmake

cmake -S . -B out -G "Visual Studio 17 2022" -A x64
cmake --build out --config Debug
```

生成的 exe 通常在：

```text
D:\demo\hello-cmake\out\Debug\HelloCMake.exe
```

最小流程就是：

```text
写 main.cpp
写 CMakeLists.txt
CMake GUI Configure
CMake GUI Generate
打开 .sln
编译运行
```

# **C++ DLL + Visual Studio `.sln`** 项目

**1. 创建目录**
```powershell
mkdir D:\demo\hello_dll
cd D:\demo\hello_dll
```

**2. 创建 `hello.h`**
```cpp
#pragma once

#ifdef HELLO_DLL_EXPORTS
#define HELLO_API __declspec(dllexport)
#else
#define HELLO_API __declspec(dllimport)
#endif

extern "C" HELLO_API int add(int a, int b);
```

**3. 创建 `hello.cpp`**
```cpp
#include "hello.h"

int add(int a, int b) {
    return a + b;
}
```

**4. 创建 `CMakeLists.txt`**
```cmake
cmake_minimum_required(VERSION 3.20)

project(HelloDll LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(hello SHARED
    hello.cpp
    hello.h
)

target_compile_definitions(hello PRIVATE HELLO_DLL_EXPORTS)
```

说明：

```cmake
add_library(hello SHARED ...)
```

表示生成动态库，也就是 Windows 下的 `hello.dll`。

```cmake
target_compile_definitions(hello PRIVATE HELLO_DLL_EXPORTS)
```

表示编译 DLL 本身时导出函数。

**5. 用 CMake 生成 `.sln`**

确保已安装：

- Visual Studio 2022
- Desktop development with C++
- CMake

执行：

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
```

成功后会生成：

```text
D:\demo\hello_dll\build\HelloDll.sln
```

**6. 编译 DLL**

Debug：

```powershell
cmake --build build --config Debug
```

Release：

```powershell
cmake --build build --config Release
```

Debug 产物通常在：

```text
D:\demo\hello_dll\build\Debug\hello.dll
D:\demo\hello_dll\build\Debug\hello.lib
```

Release 产物通常在：

```text
D:\demo\hello_dll\build\Release\hello.dll
D:\demo\hello_dll\build\Release\hello.lib
```

**7. 用 Visual Studio 打开**

打开：

```text
D:\demo\hello_dll\build\HelloDll.sln
```

在 Visual Studio 里选择：

```text
Debug / x64
```

然后右键 `hello` 项目，选择：

```text
Build
```

**8. 可选：增加测试 exe**

如果想验证 DLL 能不能被调用，可以扩展成这样：

```text
hello_dll/
  CMakeLists.txt
  hello.h
  hello.cpp
  main.cpp
```

新增 `main.cpp`：

```cpp
#include <iostream>
#include "hello.h"

int main() {
    std::cout << add(2, 3) << std::endl;
    return 0;
}
```

修改 `CMakeLists.txt`：

```cmake
cmake_minimum_required(VERSION 3.20)

project(HelloDll LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(hello SHARED
    hello.cpp
    hello.h
)

target_compile_definitions(hello PRIVATE HELLO_DLL_EXPORTS)

add_executable(test_hello main.cpp)

target_link_libraries(test_hello PRIVATE hello)
```

重新生成：

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
```

编译：

```powershell
cmake --build build --config Debug
```

运行：

```powershell
.\build\Debug\test_hello.exe
```

输出：

```text
5
```

最终这个最小项目会生成：

```text
hello.dll        动态库
hello.lib        DLL 导入库
test_hello.exe   测试程序，可选
HelloDll.sln     Visual Studio 解决方案
```
