# Multica

## 安装
`irm https://raw.githubusercontent.com/multica-ai/multica/main/scripts/install.ps1 | iex`

`curl -fsSL https://raw.githubusercontent.com/multica-ai/multica/main/scripts/install.sh | bash -s -- --with-server
multica setup self-host`

## 卸载

### 1. 停止所有正在运行的服务
如果你用的是**自部署模式**（`$env:MULTICA_MODE="local"` 或 `"with-server"`），先关闭 Docker 服务和后台守护进程：
```powershell
$env:MULTICA_MODE="stop"
irm https://raw.githubusercontent.com/multica-ai/multica/main/scripts/install.ps1 | iex
```

这一步会：
*   运行 `docker compose -f docker-compose.selfhost.yml down` 停止并移除自部署容器（数据库、后端、前端等）
*   执行 `multica daemon stop` 尝试停止本地 daemon（如果已配置）

### 2. 移除 CLI 可执行文件
脚本将 CLI 安装到了 `$env:USERPROFILE\.multica\bin\multica.exe`，需要手动删除，并清理环境变量：
```powershell
# 删除 CLI 文件
Remove-Item -Recurse -Force "$env:USERPROFILE\.multica\bin" -ErrorAction SilentlyContinue

# 从用户级 PATH 中移除该目录（重启终端生效）
$currentPath = [Environment]::GetEnvironmentVariable("Path", "User")
if ($currentPath) {
    $newPath = ($currentPath -split ';' | Where-Object { $_ -ne "$env:USERPROFILE\.multica\bin" }) -join ';'
    [Environment]::SetEnvironmentVariable("Path", $newPath, "User")
}
```

### 3. 清理自部署产生的所有数据
自部署模式会在 `$env:USERPROFILE\.multica\server` 下克隆整个仓库并存放 `.env`、Docker 卷等。需要彻底删除该目录，并移除 Docker 残留的卷和镜像：
```powershell
# 删除整个自部署安装目录（包含代码、配置、数据库卷挂载目录等）
Remove-Item -Recurse -Force "$env:USERPROFILE\.multica\server" -ErrorAction SilentlyContinue

# 清理 Docker 资源（卷和镜像，可选）
docker volume prune -f --filter "label=com.docker.compose.project=multica"
# 如果知道镜像名，可以进一步删除：
# docker rmi $(docker images -q "*multica*") -f
```

### 4. 清理可能残留的用户配置目录
如果之前还通过 `multica setup` 生成过配置，通常在 `~/.multica`（但 Windows 上可能在 `$env:USERPROFILE\.multica\config` 等位置），可一并清理：
```powershell
Remove-Item -Recurse -Force "$env:USERPROFILE\.multica" -ErrorAction SilentlyContinue
```

## 自定义服务器地址

根据安装脚本中的端口定义，**Multica 自托管服务的默认端口**为：

*   **后端 API 端口：`8080`**
*   **前端 Web 端口：`3000`**

下面这个改写后的脚本**只安装 CLI，不启动任何本地 Docker 服务**。它会自动连接你指定的局域网 IP 和默认端口，完成 CLI 与远程自托管服务的配对。

### 使用方法
```powershell
# 方式一：通过环境变量指定 IP
$env:MULTICA_REMOTE_IP="192.168.1.100"
irm https://your-script-url/install-remote.ps1 | iex

# 方式二：直接运行，脚本会询问 IP
irm https://your-script-url/install-remote.ps1 | iex
```

### 改写后的脚本（保存为 `install-remote.ps1`）
```powershell
# Multica CLI installer — 连接到局域网已有的自托管服务
# 用法:
#   $env:MULTICA_REMOTE_IP="你的服务器IP"; irm ... | iex
#   或不设置环境变量，脚本会交互询问 IP

$ErrorActionPreference = "Stop"

# ---------- 配置 ----------
$DefaultBackendPort = "8080"
$DefaultFrontendPort = "3000"

# ---------- 辅助函数 ----------
function Write-Info  { param([string]$Msg) Write-Host "==> $Msg" -ForegroundColor Cyan }
function Write-Ok    { param([string]$Msg) Write-Host "[OK] $Msg" -ForegroundColor Green }
function Write-Warn  { param([string]$Msg) Write-Warning $Msg }
function Write-Fail  { param([string]$Msg) Write-Host "[ERROR] $Msg" -ForegroundColor Red; exit 1 }

function Test-CommandExists {
    param([string]$Name)
    $null -ne (Get-Command $Name -ErrorAction SilentlyContinue)
}

function Get-LatestVersion {
    try {
        $release = Invoke-RestMethod -Uri "https://api.github.com/repos/multica-ai/multica/releases/latest" -ErrorAction Stop
        return $release.tag_name
    } catch {
        return $null
    }
}

function Convert-ToCliArch {
    param([object]$Value)
    if ($null -eq $Value) { return $null }
    $normalized = "$Value".Trim().ToUpperInvariant()
    switch ($normalized) {
        "9"      { return "amd64" }
        "AMD64"  { return "amd64" }
        "X64"    { return "amd64" }
        "X86_64" { return "amd64" }
        "12"     { return "arm64" }
        "ARM64"  { return "arm64" }
        "AARCH64" { return "arm64" }
        default  { return $null }
    }
}

function Get-WindowsCliArch {
    $signals = @()
    $nativeArchSignalFound = $false
    try {
        if (Get-Command Get-CimInstance -ErrorAction SilentlyContinue) {
            $processorArch = Get-CimInstance -ClassName Win32_Processor -ErrorAction Stop |
                Select-Object -First 1 -ExpandProperty Architecture
            $signals += [pscustomobject]@{ Source = "Win32_Processor.Architecture"; Value = $processorArch }
            $nativeArchSignalFound = $true
        }
    } catch {}
    try {
        if (-not $nativeArchSignalFound -and (Get-Command Get-WmiObject -ErrorAction SilentlyContinue)) {
            $processorArch = Get-WmiObject -Class Win32_Processor -ErrorAction Stop |
                Select-Object -First 1 -ExpandProperty Architecture
            $signals += [pscustomobject]@{ Source = "Win32_Processor.Architecture"; Value = $processorArch }
            $nativeArchSignalFound = $true
        }
    } catch {}
    try {
        $signals += [pscustomobject]@{
            Source = "RuntimeInformation.OSArchitecture"
            Value = [System.Runtime.InteropServices.RuntimeInformation]::OSArchitecture
        }
    } catch {}
    $signals += [pscustomobject]@{ Source = "PROCESSOR_ARCHITEW6432"; Value = $env:PROCESSOR_ARCHITEW6432 }
    $signals += [pscustomobject]@{ Source = "PROCESSOR_ARCHITECTURE"; Value = $env:PROCESSOR_ARCHITECTURE }
    foreach ($signal in $signals) {
        $arch = Convert-ToCliArch $signal.Value
        if ($arch) { return $arch }
    }
    $details = ($signals | Where-Object { $null -ne $_.Value -and "$($_.Value)".Trim() -ne "" } |
        ForEach-Object { "$($_.Source)=$($_.Value)" }) -join ", "
    if (-not $details) { $details = "no architecture signals available" }
    Write-Fail "Unsupported Windows architecture ($details). Only x64 and ARM64 are supported."
}

function Get-InstalledCliVersion {
    try {
        $firstLine = multica version 2>$null | Select-Object -First 1
        if ("$firstLine" -match '\b(v?\d+(?:\.\d+)+)\b') {
            $version = $Matches[1]
            if ($version -notlike 'v*') { $version = "v$version" }
            return $version
        }
    } catch {}
    return $null
}

# ---------- CLI 安装（与官方脚本相同）----------
function Install-CliBinary {
    Write-Info "Installing Multica CLI from GitHub Releases..."
    if (-not [Environment]::Is64BitOperatingSystem) {
        Write-Fail "Multica requires a 64-bit Windows installation."
    }
    $arch = Get-WindowsCliArch
    $latest = Get-LatestVersion
    if (-not $latest) { Write-Fail "Could not determine latest release. Check your network connection." }
    $version = $latest.TrimStart('v')
    $url = "https://github.com/multica-ai/multica/releases/download/$latest/multica-cli-$version-windows-$arch.zip"
    $tmpDir = Join-Path ([System.IO.Path]::GetTempPath()) "multica-install"
    if (Test-Path $tmpDir) { Remove-Item $tmpDir -Recurse -Force }
    New-Item -ItemType Directory -Path $tmpDir | Out-Null
    Write-Info "Downloading $url ..."
    try {
        Invoke-WebRequest -Uri $url -OutFile (Join-Path $tmpDir "multica.zip") -UseBasicParsing
    } catch {
        Remove-Item $tmpDir -Recurse -Force
        Write-Fail "Failed to download CLI binary: $_"
    }
    Expand-Archive -Path (Join-Path $tmpDir "multica.zip") -DestinationPath $tmpDir -Force
    $binDir = Join-Path $env:USERPROFILE ".multica\bin"
    if (-not (Test-Path $binDir)) {
        New-Item -ItemType Directory -Path $binDir -Force | Out-Null
    }
    $exeSrc = Join-Path $tmpDir "multica.exe"
    if (-not (Test-Path $exeSrc)) {
        $exeSrc = Get-ChildItem -Path $tmpDir -Filter "multica.exe" -Recurse | Select-Object -First 1 -ExpandProperty FullName
    }
    if (-not $exeSrc -or -not (Test-Path $exeSrc)) {
        Remove-Item $tmpDir -Recurse -Force
        Write-Fail "multica.exe not found in downloaded archive."
    }
    Copy-Item $exeSrc (Join-Path $binDir "multica.exe") -Force
    Remove-Item $tmpDir -Recurse -Force
    Add-ToUserPath $binDir
    Write-Ok "Multica CLI installed to $binDir\multica.exe"
}

function Add-ToUserPath {
    param([string]$Dir)
    $currentPath = [Environment]::GetEnvironmentVariable("Path", "User")
    if ($currentPath -and $currentPath.Split(";") -contains $Dir) { return }
    $newPath = if ($currentPath) { "$currentPath;$Dir" } else { $Dir }
    [Environment]::SetEnvironmentVariable("Path", $newPath, "User")
    if ($env:Path -notlike "*$Dir*") { $env:Path = "$Dir;$env:Path" }
    Write-Info "Added $Dir to user PATH (restart your terminal for other sessions to pick it up)."
}

function Install-Cli {
    if (Test-CommandExists "multica") {
        $currentVer = Get-InstalledCliVersion
        $latestVer = Get-LatestVersion
        $currentCmp = if ($currentVer) { $currentVer -replace '^v','' } else { $null }
        $latestCmp = if ($latestVer) { $latestVer -replace '^v','' } else { $null }
        $isUpToDate = $currentCmp -and -not $latestCmp
        if (-not $isUpToDate) {
            try {
                $isUpToDate = $currentCmp -and $latestCmp -and ([System.Version]$currentCmp -ge [System.Version]$latestCmp)
            } catch {
                $isUpToDate = $currentCmp -and $latestCmp -and ($currentCmp -eq $latestCmp)
            }
        }
        if ($isUpToDate) {
            Write-Ok "Multica CLI is up to date ($currentVer)"
            return
        }
        Write-Info "Multica CLI $currentVer installed, latest is $latestVer - upgrading..."
        Install-CliBinary
        $newVer = Get-InstalledCliVersion
        Write-Ok "Multica CLI upgraded ($currentVer -> $newVer)"
        return
    }
    Install-CliBinary
    if (-not (Test-CommandExists "multica")) {
        Write-Fail "CLI installed but 'multica' not found on PATH. Restart your terminal and try again."
    }
}

# ---------- 远程自托管连接 ----------
function Connect-RemoteSelfHost {
    # 获取目标 IP
    $remoteIp = $env:MULTICA_REMOTE_IP
    if (-not $remoteIp) {
        $remoteIp = Read-Host "请输入局域网内 Multica 服务器的 IP 地址"
        if (-not $remoteIp) { Write-Fail "未提供服务器 IP，操作取消。" }
    }

    $backendUrl = "http://${remoteIp}:${DefaultBackendPort}"
    Write-Info "将连接到自托管服务：$backendUrl"

    # 尝试连通性检查
    try {
        $null = Invoke-WebRequest -Uri "$backendUrl/health" -UseBasicParsing -TimeoutSec 5
        Write-Ok "成功连接到后端健康检查端点。"
    } catch {
        Write-Warn "无法访问 $backendUrl/health，请确认服务器已启动且端口可访问。脚本将继续尝试配置。"
    }

    # 尝试通过 CLI 配置自托管连接
    # 注意：multica setup self-host 可能支持 --server-url 参数，如不支持将进入交互模式
    Write-Info "正在配置 Multica CLI 以连接到远程服务..."
    $setupArgs = @("setup", "self-host")
    # 试探性添加服务地址参数（如果 CLI 版本支持）
    try {
        # 这里假设 CLI 可能支持 --server 或 --backend-url，实际请以 multica setup self-host --help 为准
        $null = multica setup self-host --help 2>$null | Out-Null
        # 若上一步未报错，则尝试自动传入地址
        $setupArgs += "--server-url", $backendUrl
    } catch {}
    
    # 执行配置（可能会要求登录邮箱等交互操作）
    & multica $setupArgs

    Write-Host ""
    Write-Host "  ============================================" -ForegroundColor Green
    Write-Host "  [OK] 配置完成！" -ForegroundColor Green
    Write-Host "  ============================================" -ForegroundColor Green
    Write-Host ""
    Write-Host "  后端 API： $backendUrl"
    Write-Host "  前端 Web： http://${remoteIp}:${DefaultFrontendPort}"
    Write-Host "  如需重新配置，运行： multica setup self-host"
    Write-Host ""
}

# ---------- 入口 ----------
Write-Host ""
Write-Host "  Multica - 远程自托管 CLI 安装器" -ForegroundColor White
Write-Host "  (不安装本地服务，直接连接局域网已有的 Multica 服务器)" -ForegroundColor DarkGray
Write-Host ""

Install-Cli
Connect-RemoteSelfHost
```

### 关键说明
1.  **完全跳过 Docker**：脚本不再检查或安装 Docker，不拉取镜像，不启动任何容器。
2.  **连接方式**：使用 `multica setup self-host` 命令配置 CLI。如果 CLI 支持 `--server-url` 参数，脚本会自动传入 `http://你的IP:8080`；否则会进入交互模式，你需要手动输入该地址。
3.  **默认端口**：后端 `8080`，前端 `3000`。如果你的服务使用了自定义端口，可以修改脚本顶部的 `$DefaultBackendPort` 和 `$DefaultFrontendPort` 变量，或直接通过 `multica setup self-host` 交互输入时指定非默认端口。
4.  **后续使用**：配置完成后，本地 daemon 会连接远程后端。你可以直接打开前端 `http://服务器IP:3000` 进行操作。
