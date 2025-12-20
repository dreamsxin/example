# Python虚拟环境（venv）详细用法指南

## 1. 基本命令解析

`python -m venv .venv; .\.venv\Scripts\Activate; pip install -r requirements.txt`

### 创建虚拟环境
```bash
python -m venv .venv
```
- `python -m venv`：使用Python内置的venv模块
- `.venv`：虚拟环境目录名称（常用名称有`.venv`、`venv`、`env`等）

### 激活虚拟环境（不同系统）

**Windows (CMD/PowerShell)**：
```bash
# CMD命令提示符
.\.venv\Scripts\activate

# PowerShell
.\.venv\Scripts\Activate.ps1
```

**Linux/macOS**：
```bash
# bash/zsh
source .venv/bin/activate
```

## 2. 完整工作流程示例

### 场景：新建项目并设置虚拟环境
```bash
# 1. 创建项目目录
mkdir myproject
cd myproject

# 2. 创建虚拟环境（推荐使用 .venv）
python -m venv .venv

# 3. 激活虚拟环境
# Windows (CMD):
.\.venv\Scripts\activate
# Windows (PowerShell):
.\.venv\Scripts\Activate.ps1
# Linux/macOS:
source .venv/bin/activate

# 激活后提示符会显示环境名：
# (.venv) C:\Users\YourName\myproject>

# 4. 安装项目依赖
pip install pandas numpy

# 5. 查看已安装包
pip list

# 6. 生成requirements.txt
pip freeze > requirements.txt

# 7. 退出虚拟环境
deactivate
```

## 3. 常用选项和参数

### 创建虚拟环境时的可选参数
```bash
# 1. 指定Python版本（系统有多个Python时）
python3.9 -m venv .venv
py -3.10 -m venv .venv  # Windows特有

# 2. 包含系统站点包（可访问系统已安装的包）
python -m venv .venv --system-site-packages

# 3. 使用符号链接而不是拷贝（节省空间）
python -m venv .venv --symlinks

# 4. 使用拷贝而不是符号链接
python -m venv .venv --copies

# 5. 升级pip、setuptools、wheel
python -m venv .venv --upgrade-deps

# 6. 清除并重新创建已存在的环境
python -m venv .venv --clear
```

## 4. 跨平台激活脚本

### 创建通用激活脚本
**`activate.ps1` (PowerShell自动加载脚本)**：
```powershell
# 放在项目根目录，PowerShell会自动检测并提示
# 内容可为空，仅作为标记文件
```

**`activate.bat` (Windows CMD脚本)**：
```batch
@echo off
echo Activating virtual environment...
call .venv\Scripts\activate.bat
```

**`activate.sh` (Linux/macOS脚本)**：
```bash
#!/bin/bash
echo "Activating virtual environment..."
source .venv/bin/activate
```

## 5. 虚拟环境管理技巧

### 检查是否在虚拟环境中
```bash
# 方法1：查看python路径
python -c "import sys; print(sys.prefix)"

# 方法2：检查环境变量
echo $VIRTUAL_ENV  # Linux/macOS
echo %VIRTUAL_ENV%  # Windows

# 方法3：安装env工具
pip install virtualenvwrapper-win  # Windows
pip install virtualenvwrapper      # Linux/macOS
```

### 快速切换和删除
```bash
# 1. 快速重新激活（修改环境后）
deactivate && source .venv/bin/activate  # Linux/macOS
deactivate && .\.venv\Scripts\activate   # Windows

# 2. 删除虚拟环境
# 直接删除目录即可
rm -rf .venv  # Linux/macOS
rmdir /s .venv  # Windows CMD
Remove-Item -Recurse -Force .venv  # PowerShell
```

## 6. 与pip结合的最佳实践

### 项目依赖管理
```bash
# 1. 创建requirements.txt
pip freeze > requirements.txt

# 2. 从requirements.txt安装
pip install -r requirements.txt

# 3. 分层requirements文件（大型项目）
# requirements/
#   ├── base.txt     # 基础依赖
#   ├── dev.txt      # 开发依赖
#   └── prod.txt     # 生产依赖
```

### 常用pip命令
```bash
# 升级pip自身
python -m pip install --upgrade pip

# 安装特定版本
pip install package==1.0.0

# 安装本地包
pip install ./mypackage

# 卸载包
pip uninstall package_name

# 显示包信息
pip show package_name
```

## 7. 常见问题和解决方案

### 问题1：权限错误（Windows）
```bash
# 错误：无法加载文件，因为在此系统上禁止运行脚本
# 解决方案：以管理员身份运行PowerShell，执行：
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser
```

### 问题2：激活脚本不存在
```bash
# 检查虚拟环境是否创建成功
ls .venv/Scripts/  # Windows
ls .venv/bin/      # Linux/macOS

# 重新创建虚拟环境
python -m venv .venv --clear
```

### 问题3：多Python版本冲突
```bash
# 明确指定Python版本
# Windows
py -3.11 -m venv .venv
# Linux/macOS
python3.11 -m venv .venv
```

## 8. IDE集成

### VS Code配置
在项目根目录创建`.vscode/settings.json`：
```json
{
    "python.defaultInterpreterPath": ".venv\\Scripts\\python.exe",
    "python.terminal.activateEnvironment": true
}
```

### PyCharm配置
1. File → Settings → Project → Python Interpreter
2. 点击齿轮 → Add → Existing environment
3. 选择`.venv/Scripts/python.exe`

## 9. 一键配置脚本

### Windows批处理文件 `setup.bat`：
```batch
@echo off
echo Setting up Python virtual environment...

REM 检查Python是否存在
python --version >nul 2>nul
if errorlevel 1 (
    echo Python is not installed or not in PATH
    pause
    exit /b 1
)

REM 创建虚拟环境
python -m venv .venv
if errorlevel 1 (
    echo Failed to create virtual environment
    pause
    exit /b 1
)

REM 激活并安装依赖
call .venv\Scripts\activate.bat
pip install --upgrade pip

if exist requirements.txt (
    echo Installing dependencies from requirements.txt...
    pip install -r requirements.txt
)

echo.
echo Virtual environment setup complete!
echo To activate manually, run: .\.venv\Scripts\activate
pause
```

## 10. 最佳实践总结

1. **始终使用虚拟环境**：每个项目单独的环境
2. **使用`.venv`作为目录名**：被.gitignore普遍支持
3. **添加虚拟环境目录到.gitignore**：
   ```
   .venv/
   venv/
   env/
   */
   ```
4. **及时更新requirements.txt**：记录所有依赖
5. **指定Python版本**：使用`pyproject.toml`或`requirements.txt`中的标记
6. **定期更新依赖**：`pip list --outdated`查看需要更新的包
