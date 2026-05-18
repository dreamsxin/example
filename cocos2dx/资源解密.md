# 女神降临资源解密过程总结

## 结论

本样本的 Cocos 资源使用固定签名和循环 XOR 密钥保护：

- 签名：`sign_123`
- 密钥：`key_456`
- 算法：文件开头匹配 `sign_123` 后，跳过 8 字节签名，对剩余内容逐字节 XOR `key_456`。

伪代码：

```c
if (file.starts_with("sign_123")) {
    body = file + 8;
    for (i = 0; i < body_len; i++) {
        body[i] ^= "key_456"[i % 7];
    }
}
```

## 资源层初步判断

先从资源文件本身入手，而不是直接看代码。

资源目录：

```text
D:\work\openclaw-workspace\arpg\nvshenres\assets\resources
```

统计扩展名：

```powershell
Get-ChildItem -LiteralPath "D:\work\openclaw-workspace\arpg\nvshenres\assets\resources" -Recurse -File |
  Group-Object Extension |
  Sort-Object Count -Descending |
  Select-Object Count,Name
```

本次主要文件类型：

```text
18441 .json
5281  .png
1061  .mp3
505   .atlas
83    .jpg
```

抽样查看文件头：

```powershell
$files = Get-ChildItem -LiteralPath "D:\work\openclaw-workspace\arpg\nvshenres\assets\resources" -Recurse -File | Select-Object -First 10
foreach ($f in $files) {
  $bytes = [IO.File]::ReadAllBytes($f.FullName)
  $head = ($bytes[0..([Math]::Min(15, $bytes.Length - 1))] | ForEach-Object { $_.ToString("X2") }) -join " "
  "$($f.Extension) $($f.Length) $head $($f.FullName)"
}
```

发现大量文件都以同一段 ASCII 开头：

```text
73 69 67 6E 5F 31 32 33
s  i  g  n  _  1  2  3
```

因此初步判断：

- `sign_123` 是加密文件标记。
- 文件真实内容在 `sign_123` 之后。
- 需要继续找 native 或脚本里的解密逻辑。

## Java/JS 层排查

反编译目录：

```text
D:\work\openclaw-workspace\arpg\nvshenres\classes-source
```

搜索关键字：

```powershell
rg -n "sign_123|key_456|deEncryptPng|setDecriptKeyAndSign|xxtea|decrypt" `
  "D:\work\openclaw-workspace\arpg\nvshenres\classes-source"
```

结论：

- Java 层没有找到 `sign_123` 和 `key_456`。
- 没有找到 `deEncryptPng` 的 Java 调用。
- Java 里能看到 Cocos 初始化流程，例如 `Cocos2dxActivity`、`Cocos2dxHelper` 会把 APK 路径和 `AssetManager` 传给 native。
- Pangle/广告 SDK 里有一些 `decrypt` 相关函数，但和游戏资源无关。

继续排查 JS：

```powershell
rg -n "sign_123|key_456|deEncryptPng|setDecriptKeyAndSign|jsb.fileUtils" `
  "D:\work\openclaw-workspace\arpg\nvshenres\assets"
```

结论：

- JS 里没有设置 sign/key 的逻辑。
- 只看到正常的 `jsb.fileUtils`、`setSearchPaths`、启动脚本等。
- 因此解密更可能被接入 Cocos native 文件读取层。

## 分析 libcocos2djs.so

native so 路径：

```text
D:\work\openclaw-workspace\arpg\nvshenres\lib\arm64-v8a\libcocos2djs.so
```

### 使用工具

本次主要使用：

```text
nm -D                 查看动态符号
strings / rg          搜索明文字符串
Python pyelftools     解析 ELF、读取 section 和符号地址
Python capstone       反汇编 ARM64 指令
objdump               辅助查看 ELF 信息；Windows 版未必支持 ARM64 反汇编
```

安装 Python 库：

```powershell
pip install pyelftools capstone
```

### 查看动态符号

优先查动态符号，因为 Android so 即使被 strip，也可能保留导出符号：

```powershell
nm -D --defined-only "D:\work\openclaw-workspace\arpg\nvshenres\lib\arm64-v8a\libcocos2djs.so" |
  findstr /i "FileUtils decrypt xxtea"
```

关键发现：

```text
cocos2d::FileUtils::getDataFromFile
cocos2d::FileUtils::getStringFromFile
cocos2d::FileUtils::setDecriptKeyAndSign
xxtea_decrypt
```

`setDecriptKeyAndSign` 是强信号，说明 Cocos 的 `FileUtils` 被加了资源解密相关逻辑。

### 搜索字符串

可以先用字符串搜索：

```powershell
strings "D:\work\openclaw-workspace\arpg\nvshenres\lib\arm64-v8a\libcocos2djs.so" |
  findstr /i "sign key decrypt png xxtea"
```

本样本没有直接搜到完整的 `sign_123` 和 `key_456`。原因是这两个值不是以完整字符串常量放在 `.rodata`，而是在 ARM64 指令里通过 `mov/movk/str` 拼到栈上，再赋值给 `std::string`。

### 解析符号地址

用 `pyelftools` 解析 `.dynsym` 可以拿到函数地址和大小。重点关注：

```text
cocos2d::FileUtils::init                       0xADABB8
cocos2d::FileUtils::setDecriptKeyAndSign       0xADAD58
cocos2d::FileUtils::getStringFromFile          0xADAE1C
cocos2d::FileUtils::getDataFromFile            0xADB038
```

示例脚本：

```python
from elftools.elf.elffile import ELFFile

so = r"D:\work\openclaw-workspace\arpg\nvshenres\lib\arm64-v8a\libcocos2djs.so"

with open(so, "rb") as f:
    elf = ELFFile(f)
    dynsym = elf.get_section_by_name(".dynsym")
    for sym in dynsym.iter_symbols():
        name = sym.name
        if "FileUtils" in name and any(x in name for x in ["init", "getDataFromFile", "getStringFromFile", "setDecriptKeyAndSign"]):
            print(hex(sym["st_value"]), sym["st_size"], name)
```

### 反汇编 ARM64 函数

Windows 上常见 `objdump` 不一定能反汇编 ARM64 so，所以这里用 `capstone`。

示例脚本：

```python
from elftools.elf.elffile import ELFFile
from capstone import Cs, CS_ARCH_ARM64, CS_MODE_ARM

so = r"D:\work\openclaw-workspace\arpg\nvshenres\lib\arm64-v8a\libcocos2djs.so"

with open(so, "rb") as f:
    elf = ELFFile(f)
    text = elf.get_section_by_name(".text")
    text_addr = text["sh_addr"]
    text_data = text.data()

    start = 0xADABB8
    size = 0x200
    offset = start - text_addr
    code = text_data[offset:offset + size]

md = Cs(CS_ARCH_ARM64, CS_MODE_ARM)
for insn in md.disasm(code, start):
    print(hex(insn.address), insn.mnemonic, insn.op_str)
```

## 确认 sign/key 写死位置

在 `cocos2d::FileUtils::init`，地址 `0xADABB8` 附近，能看到构造 `sign_123` 的指令：

```asm
mov  x9, #0x6973
movk x9, #0x6e67, lsl #16
movk x9, #0x315f, lsl #32
movk x9, #0x3332, lsl #48
stur x9, [sp, #0x19]
```

ARM64 是小端序，还原字节：

```text
0x6973 -> 73 69 -> s i
0x6e67 -> 67 6e -> g n
0x315f -> 5f 31 -> _ 1
0x3332 -> 32 33 -> 2 3
```

组合后：

```text
73 69 67 6e 5f 31 32 33
sign_123
```

同一段还构造了 `key_456`：

```asm
mov  w11, #0x656b
movk w11, #0x5f79, lsl #16
mov  w8, #0x345f
movk w8, #0x3635, lsl #16
stur w11, [sp, #1]
str  w8, [sp, #4]
```

小端还原：

```text
6b 65 79 5f 34 35 36
key_456
```

随后函数调用 `std::string::assign`，把两个值写入 `FileUtils` 对象：

```text
this + 0xD0 = sign_123
this + 0xE8 = key_456
```

另一个函数 `cocos2d::FileUtils::setDecriptKeyAndSign` 位于 `0xADAD58`，作用也是给这两个字段赋值：

```text
this + 0xD0 = sign
this + 0xE8 = key
```

但本样本里没有找到它的直接调用点。实际默认值已经在 `FileUtils::init` 里写死。

## 确认解密算法

重点看两个读取函数：

```text
cocos2d::FileUtils::getDataFromFile    0xADB038
cocos2d::FileUtils::getStringFromFile  0xADAE1C
```

用途：

- `getDataFromFile`：读取二进制资源，PNG/JPG 等走这里。
- `getStringFromFile`：读取文本资源，JSON/JS/atlas/plist 等走这里。

在 `getDataFromFile` 的反汇编里能看到：

1. 先读取文件内容。
2. 判断内容是否以 `this + 0xD0` 的签名开头。
3. 如果匹配，跳过签名长度。
4. 用 `this + 0xE8` 的 key 循环 XOR 剩余内容。

核心 XOR 循环类似：

```asm
ldrb w14, [x12]             ; 读取加密字节
ldrb w13, [x10, w8, sxtw]   ; 读取 key[i % key_len]
eor  w13, w14, w13          ; XOR
strb w13, [x12], #1         ; 写回解密结果
```

还原成 C 伪代码：

```c
if (data.starts_with(sign)) {
    output = data + sign.length;
    for (i = 0; i < output.length; i++) {
        output[i] ^= key[i % key.length];
    }
    return output;
}
```

`getStringFromFile` 中也存在同类循环，只是处理的是字符串缓冲区。

## 为什么没有 deEncryptPng

本样本没有找到 `deEncryptPng` 这个符号、字符串或 JS 调用。

原因是实际解密不是作为单独的 PNG 解密函数暴露，而是接入了 Cocos 文件读取层：

```text
FileUtils::getDataFromFile
FileUtils::getStringFromFile
```

游戏层读取资源时仍然是普通 Cocos 调用，例如 `cc.loader`、`jsb.fileUtils`。底层 native `FileUtils` 已经自动完成解密，因此 Java/JS 层看不到显式的 `deEncryptPng`。

## 通用脚本

脚本：

```text
tools/cocos_xor_resource_tool.py
```

### 分析资源目录

```powershell
python tools\cocos_xor_resource_tool.py analyze `
  "D:\work\openclaw-workspace\arpg\nvshenres\assets\resources" `
  --report-json "D:\work\openclaw-workspace\arpg\nvshenres_analysis.json" `
  --report-csv "D:\work\openclaw-workspace\arpg\nvshenres_analysis.csv"
```

### 解密资源目录

```powershell
python tools\cocos_xor_resource_tool.py decrypt `
  "D:\work\openclaw-workspace\arpg\nvshenres\assets\resources" `
  "D:\work\openclaw-workspace\arpg\nvshenres_decrypted\assets\resources" `
  --overwrite `
  --report-json "D:\work\openclaw-workspace\arpg\nvshenres_decrypted\decrypt_report.json" `
  --report-csv "D:\work\openclaw-workspace\arpg\nvshenres_decrypted\decrypt_report.csv"
```

### 验证解密结果

```powershell
python tools\cocos_xor_resource_tool.py verify `
  "D:\work\openclaw-workspace\arpg\nvshenres_decrypted\assets\resources" `
  --report-json "D:\work\openclaw-workspace\arpg\nvshenres_decrypted\verify_report.json" `
  --report-csv "D:\work\openclaw-workspace\arpg\nvshenres_decrypted\verify_report.csv"
```

### 换其他 sign/key

如果以后遇到同类方案但参数不同，可以直接传参：

```powershell
python tools\cocos_xor_resource_tool.py decrypt "源目录" "输出目录" --sign "sign_123" --key "key_456" --overwrite
```

也支持十六进制输入：

```powershell
python tools\cocos_xor_resource_tool.py decrypt "源目录" "输出目录" --sign "hex:73 69 67 6e 5f 31 32 33" --key "hex:6b 65 79 5f 34 35 36"
```

## 本次实际结果

```text
total: 25384
decrypted: 24321
copied_plain: 1063
json: 18441
png: 5266
jpg: 98
mp3: 1061
text/atlas/plist: 511
binary/font: 6
extension_mismatch: 17
```

验证结果：

```text
18441 个 JSON 全部可解析
输出目录中 remaining_sign = 0
```
