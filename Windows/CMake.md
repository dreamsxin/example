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

CMake GUI 是“生成工程文件”的工具，不是“创建项目模板”的工具。
