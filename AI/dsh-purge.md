# DSH 插件安装教程：dsh-purge

适用于 Windows、已初始化的 DSH profile。本次安装使用的包是 `dsh-purge-community`，不要把它与旧配置里的模块名 `dsh-purge` 混淆。

## 1. 确认 DSH 和 profile

先确认要安装到哪个 profile，以及当前 DSH 版本：

```powershell
$env:DSH_PROFILE
$env:DSH_PROFILE_DIR
dsh --version
```

如果当前 profile 不是目标 profile，后续命令使用 `--profile <名称>` 明确指定，避免装错位置。

## 2. 阅读仓库安装说明并确认稳定版

从插件仓库读取 `AGENTS.md` 的 Installation 章节，按其指定的安装入口、稳定版本和数据迁移方式操作。不要直接猜 npm 包名；本项目的正式包名是 `dsh-purge-community`，仓库说明指定 CLI 安装时使用版本化 Release TGZ。

本次使用：

```text
https://github.com/kingcharleslzy-ai/dsh-purge/releases/download/v1.7.20/dsh-purge-community-1.7.20.tgz
```

安装前可核对 TGZ 与发布校验文件中的 SHA256。实际安装时，沿用目标 profile 已有的 pnpm/npm 配置。

## 3. 检查旧版和旧配置

检查 profile 目录里的 `package.json`、`cordis.patch.yml`、`pnpm-workspace.yaml`，以及 `$DSH_HOME` 下的插件状态和自定义预设。

重点识别：

- profile 中是否已有 `dsh-purge-community` 依赖。
- 是否有手写的 `id: dsh-purge` 条目，但模块名仍为 `name: dsh-purge`。
- 是否有上次失败安装留下的 `allowBuilds: dsh-purge: true`。
- 提示词文件、自定义预设、规则和插件数据目录是否已有用户内容。

修改前先备份 profile 清单、锁文件、补丁文件和自定义预设。保留用户提示词、规则和其他插件配置；不要用新包内的模板覆盖用户文件。

## 4. 安装指定稳定版

把 `<PROFILE>` 替换成实际 profile 名称：

```powershell
dsh plugin --profile <PROFILE> add "https://github.com/kingcharleslzy-ai/dsh-purge/releases/download/v1.7.20/dsh-purge-community-1.7.20.tgz"
```

成功后确认 `package.json` 中：

- `dependencies` 包含 `dsh-purge-community`，值指向选定的版本化 TGZ。
- `dsh.profile.bundles` 包含 `dsh-purge-community`。
- 锁文件记录的版本为 `1.7.20`。

## 5. 清理旧模块引用并保留设置

如果旧补丁手写了指向不存在包名的条目，删除旧 `insert`，让 bundle 自带的 `id: dsh-purge`、`name: dsh-purge-community` 条目接管。对旧配置中已有的设置逐项比对，将不同值以 profile override 形式保留；相同的默认值无需重复写入。

清理失败安装遗留的无效 `allowBuilds` 项。不要清除有效的 registry、代理或其他 build 配置。

本次迁移中，原有 `enabled`、`stripMnemon`、`capabilityProtocol`、`hooksDenyBypass` 均与新 bundle 默认值一致，因此没有改变有效设置。旧红队预设中的 `dsh-purge/redteam/tools` 引用由插件首次加载时迁移，并保留原预设及迁移备份。

## 6. 验证安装

可检查磁盘包版本：

```powershell
$pkg = "$env:DSH_PROFILE_DIR\node_modules\dsh-purge-community\package.json"
(Get-Content $pkg -Raw -Encoding UTF8 | ConvertFrom-Json) |
  Select-Object name, version
```

检查 profile 组合结果和旧模块引用：

```powershell
dsh --profile <PROFILE> --dump-config
```

确认：

- 命令退出码为 0。
- 组合结果只有一个 `id: dsh-purge` 条目，模块名为 `dsh-purge-community`。
- 不再出现 `Cannot find package 'dsh-purge'`。
- 可选入口能解析到已安装包中的文件。

## 7. 应用与重启

安装、应用补丁、重启是不同步骤。安装本身不会让宿主补丁立即生效；插件说明要求的宿主 apply 需按该实例的宿主定位方式另行操作。完成后由用户自行退出并重启 DSH，再运行插件自检确认状态。

本次实际安装结果：**dsh-purge-community 1.7.20，profile `web`，宿主 DSH 0.2.1-rc.5**。配置备份位于 `D:\work\PyPasser\.dsh-migration-backup\20261118-142248`。
