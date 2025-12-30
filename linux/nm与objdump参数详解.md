# nm与objdump参数详解

## 一、nm命令详解

### 1.1 nm命令概述
`nm`用于显示目标文件（可执行文件、静态库、共享库）中的符号信息。

### 1.2 基本语法
```bash
nm [选项] 目标文件...
```

### 1.3 常用选项详解

#### 1.3.1 符号类型显示控制
| 选项 | 说明 | 示例 |
|------|------|------|
| `-a` | 显示所有符号（包括调试符号） | `nm -a a.out` |
| `-g` | 仅显示外部（全局）符号 | `nm -g lib.so` |
| `-u` | 仅显示未定义的符号 | `nm -u a.out` |
| `--defined-only` | 仅显示已定义的符号 | `nm --defined-only a.out` |
| `--undefined-only` | 仅显示未定义的符号 | `nm --undefined-only a.out` |

#### 1.3.2 符号排序选项
| 选项 | 说明 | 示例 |
|------|------|------|
| `-n` | 按地址/数值排序（升序） | `nm -n a.out` |
| `-v` | 按地址/数值排序（升序） | `nm -v a.out` |
| `--numeric-sort` | 按地址/数值排序 | `nm --numeric-sort a.out` |
| `--no-sort` | 不排序 | `nm --no-sort a.out` |
| `-r` | 反向排序 | `nm -rn a.out` |
| `--reverse-sort` | 反向排序 | `nm --reverse-sort a.out` |

#### 1.3.3 格式化输出
| 选项 | 说明 | 示例 |
|------|------|------|
| `-t <format>` | 指定输出格式 | `nm -t x a.out` |
| `--format=<format>` | 指定输出格式 | `nm --format=bsd a.out` |
| `-B` | BSD格式（默认） | `nm -B a.out` |
| `--format=bsd` | BSD格式 | `nm --format=bsd a.out` |
| `--format=sysv` | System V格式 | `nm --format=sysv a.out` |
| `-P` | POSIX格式 | `nm -P a.out` |
| `--format=posix` | POSIX格式 | `nm --format=posix a.out` |
| `--portability` | 可移植格式 | `nm --portability a.out` |

#### 1.3.4 详细输出
| 选项 | 说明 | 示例 |
|------|------|------|
| `-l` | 显示符号所在的行号（需要调试信息） | `nm -l a.out` |
| `--line-numbers` | 显示行号 | `nm --line-numbers a.out` |
| `-C` | 解码C++符号名（demangle） | `nm -C a.out` |
| `--demangle` | 解码C++符号名 | `nm --demangle a.out` |
| `--no-demangle` | 不解码C++符号名 | `nm --no-demangle a.out` |
| `-S` | 显示符号大小 | `nm -S a.out` |
| `--print-size` | 显示符号大小 | `nm --print-size a.out` |
| `-A` | 显示文件名 | `nm -A *.o` |
| `--print-file-name` | 显示文件名 | `nm --print-file-name *.o` |

#### 1.3.5 特殊符号处理
| 选项 | 说明 | 示例 |
|------|------|------|
| `-D` | 显示动态符号 | `nm -D lib.so` |
| `--dynamic` | 显示动态符号 | `nm --dynamic lib.so` |
| `--with-symbol-versions` | 显示符号版本 | `nm --with-symbol-versions libc.so` |
| `--plugin <name>` | 使用插件 | `nm --plugin libfoo.so` |

### 1.4 符号类型说明

nm输出中的符号类型（第二列）：
| 符号 | 说明 | 示例 |
|------|------|------|
| `A` | 绝对符号，值在链接期间不会改变 | |
| `B` | BSS段中的未初始化数据 | |
| `C` | 公共段（未初始化的数据） | |
| `D` | 已初始化数据段中的符号 | |
| `G` | 已初始化数据段中的小对象 | |
| `I` | 间接引用另一个符号 | |
| `N` | 调试符号 | |
| `R` | 只读数据段 | |
| `S` | 未初始化数据段中的小对象 | |
| `T` | 文本（代码）段中的符号 | `main`, `printf` |
| `U` | 未定义的符号 | `malloc`, `free` |
| `V` | 弱对象符号 | |
| `W` | 弱符号，未标记为弱对象符号 | |
| `-` | stabs符号表中的调试符号 | |
| `?` | 未知类型 | |

### 1.5 实用示例

```bash
# 1. 按地址顺序显示所有符号
nm -n a.out

# 2. 只显示代码段中的函数
nm -n a.out | grep ' T '

# 3. 显示未定义的符号（需要链接的库函数）
nm -u a.out

# 4. 显示符号及其大小
nm -S a.out

# 5. 显示动态符号（用于共享库）
nm -D libfoo.so

# 6. 解码C++符号
nm -C cpp_program

# 7. 查找特定函数的地址
nm a.out | grep -E ' main$'
nm a.out | grep 'T main'

# 8. 显示所有文件中的符号（用于多个目标文件）
nm -A *.o

# 9. 生成符号表映射文件
nm -n a.out > symbols.txt

# 10. 查找所有全局函数
nm -g a.out | grep ' T '
```

## 二、objdump命令详解

### 2.1 objdump命令概述
`objdump`用于显示目标文件的详细信息，包括反汇编、节头、符号表等。

### 2.2 基本语法
```bash
objdump [选项] 目标文件...
```

### 2.3 常用选项详解

#### 2.3.1 反汇编相关
| 选项 | 说明 | 示例 |
|------|------|------|
| `-d` | 反汇编可执行代码的节 | `objdump -d a.out` |
| `-D` | 反汇编所有节（包括数据节） | `objdump -D a.out` |
| `-S` | 显示源代码和汇编代码混合（需要-g） | `objdump -S a.out` |
| `--source` | 显示源代码（需要调试信息） | `objdump --source a.out` |
| `-l` | 显示行号信息 | `objdump -l a.out` |
| `--line-numbers` | 显示行号信息 | `objdump --line-numbers a.out` |
| `--start-address=<addr>` | 从指定地址开始反汇编 | `objdump -d --start-address=0x400000 a.out` |
| `--stop-address=<addr>` | 在指定地址停止反汇编 | `objdump -d --stop-address=0x401000 a.out` |
| `--disassemble=<symbol>` | 仅反汇编指定符号 | `objdump -d --disassemble=main a.out` |
| `--disassemble-all` | 反汇编所有节（同-D） | `objdump --disassemble-all a.out` |
| `-M <options>` | 指定反汇编选项 | `objdump -M intel a.out` |

#### 2.3.2 节（Section）信息
| 选项 | 说明 | 示例 |
|------|------|------|
| `-h` | 显示节头信息 | `objdump -h a.out` |
| `--section-headers` | 显示节头信息 | `objdump --section-headers a.out` |
| `-t` | 显示符号表 | `objdump -t a.out` |
| `-T` | 显示动态符号表 | `objdump -T lib.so` |
| `-r` | 显示重定位信息 | `objdump -r a.out` |
| `-R` | 显示动态重定位信息 | `objdump -R lib.so` |
| `-s` | 显示所有节的完整内容 | `objdump -s a.out` |
| `--full-contents` | 显示所有节的完整内容 | `objdump --full-contents a.out` |
| `-j <section>` | 仅处理指定节 | `objdump -j .text a.out` |
| `--section=<section>` | 仅处理指定节 | `objdump --section=.text a.out` |

#### 2.3.3 文件头信息
| 选项 | 说明 | 示例 |
|------|------|------|
| `-f` | 显示文件头信息 | `objdump -f a.out` |
| `--file-headers` | 显示文件头信息 | `objdump --file-headers a.out` |
| `-p` | 显示程序头信息 | `objdump -p a.out` |
| `--private-headers` | 显示私有头信息 | `objdump --private-headers a.out` |
| `-x` | 显示所有头信息 | `objdump -x a.out` |
| `--all-headers` | 显示所有头信息 | `objdump --all-headers a.out` |

#### 2.3.4 其他选项
| 选项 | 说明 | 示例 |
|------|------|------|
| `-C` | 解码C++符号名 | `objdump -C a.out` |
| `--demangle` | 解码C++符号名 | `objdump --demangle a.out` |
| `-w` | 宽行输出 | `objdump -w a.out` |
| `--wide` | 宽行输出 | `objdump --wide a.out` |
| `-z` | 显示零字节 | `objdump -z a.out` |
| `--disassemble-zeroes` | 显示零字节 | `objdump --disassemble-zeroes a.out` |
| `-EB` | 大端字节序 | `objdump -EB a.out` |
| `-EL` | 小端字节序 | `objdump -EL a.out` |
| `--endian={big|little}` | 指定字节序 | `objdump --endian=little a.out` |

### 2.4 实用示例

```bash
# 1. 反汇编可执行文件
objdump -d a.out

# 2. 反汇编特定函数
objdump -d a.out --disassemble=main

# 3. 显示源代码和汇编代码混合（需要-g编译）
objdump -S a.out

# 4. 显示节头信息
objdump -h a.out

# 5. 显示所有头信息
objdump -x a.out

# 6. 显示符号表
objdump -t a.out

# 7. 显示动态符号表
objdump -T libfoo.so

# 8. 显示重定位信息
objdump -r a.out

# 9. 显示指定节的内容
objdump -s -j .rodata a.out

# 10. 从指定地址开始反汇编
objdump -d --start-address=0x400500 --stop-address=0x400600 a.out

# 11. 使用Intel语法反汇编
objdump -d -M intel a.out

# 12. 反汇编所有节（包括数据）
objdump -D a.out

# 13. 显示行号信息
objdump -l a.out

# 14. 查找特定字符串
objdump -s a.out | grep -A5 -B5 "Hello"

# 15. 显示程序头信息（加载信息）
objdump -p a.out
```

### 2.5 高级用法示例

```bash
# 1. 提取.text节到文件
objdump -s -j .text a.out | grep -A 10000 "Contents of section .text:" | \
  tail -n +2 | xxd -r > text.bin

# 2. 生成调用图（需要安装graphviz）
objdump -d a.out | grep 'call.*<.*>' | sed 's/.*call.*<\(.*\)>.*/\1/' | \
  sort | uniq -c | sort -rn > calls.txt

# 3. 分析函数大小
objdump -t a.out | awk '/\.text/ && !/\.text\./ {print $5, $6}' | \
  sort -k2 | grep 'F' > func_sizes.txt

# 4. 查找所有系统调用
objdump -d a.out | grep -E 'syscall|int.*0x80' | head -20

# 5. 分析GOT/PLT表
objdump -d -j .plt a.out
objdump -s -j .got.plt a.out

# 6. 显示特定架构的反汇编
objdump -d -m i386 a.out  # 32位x86
objdump -d -m i386:x86-64 a.out  # x86-64
objdump -d -m arm a.out  # ARM
objdump -d -m aarch64 a.out  # ARM64
```

## 三、结合nm和objdump进行二进制分析

### 3.1 自动化分析脚本

```bash
#!/bin/bash
# analyze_binary.sh - 综合使用nm和objdump分析二进制文件

BINARY=$1

if [ -z "$BINARY" ]; then
    echo "Usage: $0 <binary>"
    exit 1
fi

if [ ! -f "$BINARY" ]; then
    echo "Error: $BINARY not found"
    exit 1
fi

echo "=== Binary Analysis Report for $BINARY ==="
echo

echo "=== 1. Basic Information ==="
file "$BINARY"
echo

echo "=== 2. Symbol Table (nm -n) ==="
nm -n "$BINARY" | head -20
echo "..."
echo

echo "=== 3. Function Symbols (T type) ==="
nm -n "$BINARY" | grep ' T ' | head -20
echo

echo "=== 4. Undefined Symbols ==="
nm -u "$BINARY" | head -10
echo

echo "=== 5. Section Headers ==="
objdump -h "$BINARY" | head -30
echo

echo "=== 6. Main Function Disassembly ==="
# 先找到main函数的地址
MAIN_ADDR=$(nm "$BINARY" | grep -E '\s+T\s+main$' | awk '{print "0x"$1}')
if [ -n "$MAIN_ADDR" ]; then
    echo "Main function at $MAIN_ADDR"
    objdump -d --start-address="$MAIN_ADDR" --stop-address=$(printf "0x%x" $((MAIN_ADDR + 0x100))) "$BINARY"
else
    echo "Main function not found"
fi
echo

echo "=== 7. Program Headers (Load Info) ==="
objdump -p "$BINARY" | head -30
echo

echo "=== 8. String Constants ==="
strings "$BINARY" | grep -E 'Hello|Error|Warning|Info|Debug' | head -10
echo

echo "=== 9. Dynamic Symbols ==="
nm -D "$BINARY" 2>/dev/null | head -10 || echo "No dynamic symbols"
echo

echo "=== 10. Relocation Entries ==="
objdump -r "$BINARY" | head -10
```

### 3.2 查找特定信息

```bash
#!/bin/bash
# find_symbols.sh - 查找特定类型的符号

BINARY=$1
PATTERN=$2

if [ $# -lt 2 ]; then
    echo "Usage: $0 <binary> <symbol_pattern>"
    echo "Example: $0 a.out 'print.*'"
    exit 1
fi

echo "Searching for symbols matching '$PATTERN' in $BINARY"
echo

echo "1. Text symbols (functions):"
nm "$BINARY" | grep ' T ' | grep -i "$PATTERN"
echo

echo "2. Data symbols:"
nm "$BINARY" | grep ' D ' | grep -i "$PATTERN"
echo

echo "3. BSS symbols:"
nm "$BINARY" | grep ' B ' | grep -i "$PATTERN"
echo

echo "4. Undefined symbols:"
nm -u "$BINARY" | grep -i "$PATTERN"
echo

echo "5. Disassembly of matching functions:"
nm "$BINARY" | grep ' T ' | grep -i "$PATTERN" | while read line; do
    ADDR=$(echo $line | awk '{print $1}')
    NAME=$(echo $line | awk '{print $3}')
    echo "=== $NAME (0x$ADDR) ==="
    objdump -d --disassemble="$NAME" "$BINARY" 2>/dev/null | head -20
    echo
done
```

### 3.3 分析函数调用关系

```bash
#!/bin/bash
# call_analysis.sh - 分析函数调用关系

BINARY=$1

if [ -z "$BINARY" ]; then
    echo "Usage: $0 <binary>"
    exit 1
fi

echo "=== Function Call Analysis for $BINARY ==="
echo

# 提取所有函数名
echo "1. List of functions:"
nm "$BINARY" | grep ' T ' | awk '{printf "0x%s %s\n", $1, $3}' > /tmp/functions.txt
cat /tmp/functions.txt
echo

# 分析每个函数的调用
echo "2. Function calls:"
while read line; do
    ADDR=$(echo $line | awk '{print $1}')
    NAME=$(echo $line | awk '{print $2}')
    
    echo "--- $NAME ($ADDR) ---"
    
    # 反汇编函数并查找call指令
    objdump -d --start-address="$ADDR" "$BINARY" 2>/dev/null | \
        grep -E 'call.*<.*>' | \
        sed 's/.*call.*[[:space:]]*//' | \
        sed 's/^<//;s/>.*$//' | \
        sort | uniq
    
    echo
done < /tmp/functions.txt | head -100

# 清理
rm -f /tmp/functions.txt
```

## 四、实际应用示例

### 4.1 调试信息分析

```bash
# 编译带调试信息的程序
gcc -g -o test test.c

# 使用nm查看调试符号
nm -a test | grep -E ' N '

# 使用objdump查看带源码的反汇编
objdump -S test | head -100

# 查看特定函数的源码级反汇编
objdump -S --disassemble=main test
```

### 4.2 共享库分析

```bash
# 查看共享库的导出符号
nm -D libfoo.so

# 查看共享库的动态符号表
objdump -T libfoo.so

# 查看共享库的重定位信息
objdump -R libfoo.so

# 查看共享库的依赖
objdump -p libfoo.so | grep NEEDED
```

### 4.3 安全分析

```bash
# 查找危险函数调用
objdump -d target | grep -E 'system|exec|popen|gets|strcpy|sprintf'

# 检查可写可执行段
objdump -h target | grep -E 'W.*E|E.*W'

# 查找字符串引用
strings -t x target | grep -E '/bin/sh|password|secret'

# 检查PLT/GOT完整性
objdump -d -j .plt target
objdump -s -j .got target
```

## 五、性能分析应用

### 5.1 函数大小统计

```bash
#!/bin/bash
# func_stats.sh - 统计函数大小

BINARY=$1

if [ -z "$BINARY" ]; then
    echo "Usage: $0 <binary>"
    exit 1
fi

echo "=== Function Size Statistics for $BINARY ==="
echo

# 使用nm获取函数大小
nm -S --size-sort "$BINARY" | grep ' T ' | \
    awk '{printf "0x%04x %s\n", strtonum("0x"$2), $4}' | \
    sort -rn | head -20

echo
echo "=== Top 10 Largest Functions ==="

nm -S "$BINARY" | grep ' T ' | \
    awk '{printf "%d %s\n", strtonum("0x"$2), $4}' | \
    sort -rn | head -10 | \
    while read size name; do
        printf "%-40s %10d bytes\n" "$name" $size
    done
```

### 5.2 交叉引用分析

```bash
#!/bin/bash
# xref.sh - 生成交叉引用列表

BINARY=$1

if [ -z "$BINARY" ]; then
    echo "Usage: $0 <binary>"
    exit 1
fi

# 生成符号地址映射
nm -n "$BINARY" | grep ' T \| D \| B ' > /tmp/symbols.txt

# 生成反汇编并分析引用
objdump -d "$BINARY" | \
    awk '/^[0-9a-f]+ <.*>:/ {func=$2} /call.*<.*>/ {print func " -> " $NF}' | \
    sed 's/[<>]//g' | \
    sort | uniq -c | sort -rn
```

## 六、总结

### 6.1 nm与objdump对比

| 特性 | nm | objdump |
|------|-----|---------|
| **主要用途** | 显示符号表 | 显示目标文件详细信息 |
| **符号信息** | 详细符号类型和地址 | 基本符号信息 |
| **反汇编** | 不支持 | 支持完整的反汇编 |
| **节信息** | 不支持 | 支持完整的节信息 |
| **重定位** | 不支持 | 支持重定位信息 |
| **文件头** | 不支持 | 支持所有头信息 |
| **调试信息** | 有限支持 | 支持源码级反汇编 |

### 6.2 选择建议

1. **需要快速查看符号表** → 使用 `nm`
2. **需要反汇编代码** → 使用 `objdump -d`
3. **需要查看节信息** → 使用 `objdump -h`
4. **需要查看文件格式** → 使用 `objdump -f` 或 `objdump -x`
5. **需要源码级调试** → 使用 `objdump -S`（需要-g编译）
6. **需要分析共享库** → 使用 `nm -D` 和 `objdump -T`

### 6.3 常用组合

```bash
# 完整的二进制分析流程
1. file binary          # 查看文件类型
2. nm -n binary         # 查看符号表
3. objdump -h binary    # 查看节信息
4. objdump -d binary    # 反汇编
5. objdump -t binary    # 查看符号表（objdump格式）
6. strings binary       # 查看字符串
```

通过熟练掌握nm和objdump的参数，可以高效地进行二进制分析、调试和逆向工程工作。
