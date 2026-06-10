# BrowserOS UI 与布局技术文档

> **文档版本**：2026-06  
> **适用范围**：BrowserOS 作为 Chromium 定制浏览器的 UI 全栈实现  
> **代码路径**：
> - 原生层（C++）：`packages/browseros/chromium_patches/`
> - 扩展层（TypeScript/React）：`packages/browseros-agent/apps/agent/`

---

## 目录

1. [架构总览](#1-架构总览)
2. [Chromium 原生层：侧边栏实现](#2-chromium-原生层侧边栏实现)
3. [扩展层：React 应用架构](#3-扩展层react-应用架构)
4. [主题与样式系统](#4-主题与样式系统)
5. [新标签页 UI](#5-新标签页-ui)
6. [侧边栏聊天 UI](#6-侧边栏聊天-ui)
7. [AI 消息渲染系统](#7-ai-消息渲染系统)
8. [交互与动画](#8-交互与动画)
9. [状态管理与上下文](#9-状态管理与上下文)
10. [原生层与扩展层的交互](#10-原生层与扩展层的交互)
11. [关键文件索引](#11-关键文件索引)

---

## 1. 架构总览

BrowserOS 是一个 **Chromium 定制浏览器**，其 UI 系统分为两个清晰的层次：

```
┌─────────────────────────────────────────────────────────────────────┐
│                      BrowserOS UI 架构                             │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│  ┌──────────────────────┐    ┌──────────────────────────────────┐  │
│  │  扩展层（TypeScript） │    │   原生层（C++ / Chromium）        │  │
│  │  browseros-agent      │    │   chromium_patches/               │  │
│  │                      │    │                                    │  │
│  │  • React 19 UI       │    │  • 侧边栏生命周期管理             │  │
│  │  • 聊天界面          │    │  • 自定义 chrome.sidePanel API     │  │
│  │  • 主题系统          │    │  • ThirdPartyLlm 面板             │  │
│  │  • AI 消息渲染       │    │  • ClashOfGpts 多 LLM 对比窗口   │  │
│  │  • 路由与布局        │    │  • 工具栏按钮集成                 │  │
│  └──────────┬───────────┘    └──────────────┬───────────────────┘  │
│             │                                  │                       │
│             │    chrome.sidePanel API           │                       │
│             └──────────────────────────────────┘                       │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```

### 技术栈

| 层次 | 技术 | 用途 |
|------|------|------|
| **扩展层** | React 19 + TypeScript | UI 框架与类型安全 |
| | Tailwind CSS v4 | 原子化样式（使用 `@theme inline` 新语法） |
| | WXT | Chrome 扩展构建工具 |
| | motion/react (Framer Motion) | 动画效果 |
| | shadcn/ui + Radix UI | 基础 UI 组件（无头组件原语） |
| | Streamdown | Markdown 实时流式渲染 |
| | downshift | 搜索框下拉组合框 |
| **原生层** | C++ / Chromium | 浏览器引擎级 UI 控制 |
| | Chrome Extensions API | 扩展与浏览器原生层通信 |
| | Views UI Framework | Chromium 原生 UI 组件 |

### 双入口架构

扩展层有两个独立的 React 应用入口：

| 入口 | 文件 | 用途 | 路由基础 |
|------|------|------|----------|
| **主应用** | `app.html` | 新标签页 / 选项页 / 设置页 | `/home/*`, `/settings/*` |
| **侧边栏** | `sidepanel.html` | Chrome 侧边栏面板 | `/`, `/history` |

原生层的侧边栏通过 `chrome.sidePanel` API 控制这两个入口的显示/隐藏。

---

## 2. Chromium 原生层：侧边栏实现

> **代码路径**：`packages/browseros/chromium_patches/chrome/browser/`

BrowserOS 在 Chromium 引擎层面对侧边栏进行了深度改造，实现了标准 Chrome 扩展 API 之外的能力。

### 2.1 架构总览

```
chromium_patches/chrome/browser/
├── extensions/api/side_panel/       # 扩展 API 层（C++ ↔ JS 桥接）
│   ├── side_panel_api.cc/h         # browserosToggle / browserosIsOpen
│   └── side_panel_service.cc/h     # 服务端逻辑（状态管理）
└── ui/
    └── side_panel/                 # UI 视图层（原生 UI 组件）
        ├── side_panel_coordinator.cc  # 侧边栏协调器
        ├── side_panel.h/cc          # 侧边栏视图
        ├── side_panel_action_callback.cc/h  # 工具栏按钮回调
        ├── side_panel_entry_id.h    # 侧边栏入口类型定义
        ├── side_panel_prefs.cc      # 侧边栏偏好设置
        ├── third_party_llm/        # 第三方 LLM 面板
        │   ├── third_party_llm_panel_coordinator.cc/h
        │   └── third_party_llm_view.cc/h
        ├── clash_of_gpts/          # 多 LLM 对比窗口
        │   ├── clash_of_gpts_coordinator.cc/h
        │   ├── clash_of_gpts_view.cc/h
        │   └── clash_of_gpts_window.cc/h
        └── extensions/
            └── extension_side_panel_manager.cc
```

### 2.2 自定义扩展 API

BrowserOS 在标准 `chrome.sidePanel` API 基础上，新增了两个自定义方法：

#### `chrome.sidePanel.browserosToggle(options)`

切换指定 Tab 的侧边栏显示状态。

**扩展层调用方式（TypeScript）**：
```typescript
// 切换侧边栏（toggle）
chrome.sidePanel.browserosToggle({ tabId: 123 }, (result) => {
  console.log(result.opened) // boolean：切换后的状态
})

// 强制打开
chrome.sidePanel.browserosToggle({ tabId: 123, open: true }, ...)

// 强制关闭
chrome.sidePanel.browserosToggle({ tabId: 123, open: false }, ...)
```

**C++ 实现**（`side_panel_api.cc`）：
```cpp
ExtensionFunction::ResponseAction
SidePanelBrowserosToggleFunction::RunFunction() {
  // 1. 解析可选参数：desired_state (open/close/toggle)
  std::optional<bool> desired_state = std::nullopt;
  if (params->options.open.has_value()) {
    desired_state = params->options.open.value();
  }

  // 2. 查找 Tab 和 BrowserWindow
  // 3. 委托给 SidePanelService
  base::expected<bool, std::string> toggle_result =
      service->BrowserosToggleSidePanelForTab(
          *extension(), browser_context(),
          params->options.tab_id,
          include_incognito_information(), desired_state);

  // 4. 返回 { opened: boolean }
  api::side_panel::BrowserosToggleResult result;
  result.opened = toggle_result.value();
  return RespondNow(WithArguments(result.ToValue()));
}
```

#### `chrome.sidePanel.browserosIsOpen(options)`

检查指定 Tab 的侧边栏是否打开。

```typescript
chrome.sidePanel.browserosIsOpen({ tabId: 123 }, (isOpen) => {
  console.log(isOpen) // boolean
})
```

#### 移除用户手势限制

BrowserOS **注释掉了** `sidePanel.open()` 的用户手势要求，使得扩展可以在没有用户交互的情况下程序化打开侧边栏：

```cpp
// side_panel_api.cc (原有代码被注释)
// if (!user_gesture()) {
//   return RespondNow(
//       Error("`sidePanel.open()` may only be called in "
//             "response to a user gesture."));
// }
```

### 2.3 SidePanelService 核心逻辑

`SidePanelService` 是侧边栏状态管理的核心服务，新增了两个关键方法：

#### `BrowserosToggleSidePanelForTab()`

```cpp
base::expected<bool, std::string>
SidePanelService::BrowserosToggleSidePanelForTab(
    const Extension& extension,
    content::BrowserContext* context,
    int tab_id,
    bool include_incognito_information,
    std::optional<bool> desired_state) {
  // 1. 查找 Tab 和 BrowserWindow
  WindowController* window = nullptr;
  content::WebContents* web_contents = nullptr;
  ExtensionTabUtil::GetTabById(tab_id, context, ...);

  BrowserWindowInterface* browser_window =
      window->GetBrowserWindowInterface();

  // 2. 自动注册 contextual panel options（如果不存在）
  //    这确保面板是 Tab 级别的，不会泄漏到其他 Tab
  if (!has_contextual_options) {
    api::side_panel::PanelOptions contextual_options;
    contextual_options.tab_id = tab_id;
    contextual_options.path = std::move(default_options.path);
    contextual_options.enabled = true;
    SetOptions(extension, std::move(contextual_options));
  }

  // 3. 调用底层的 contextual toggle
  bool is_now_open = side_panel_util::ToggleContextualExtensionSidePanel(
      *browser_window, *web_contents, extension.id(), desired_state);

  return is_now_open;
}
```

**关键设计**：每个 Tab 拥有独立的侧边栏状态，通过 `panels_` 字典以 `extension.id()` 为 key 存储每个 Tab 的 `PanelOptions`。

#### `BrowserosIsSidePanelOpenForTab()`

```cpp
base::expected<bool, std::string>
SidePanelService::BrowserosIsSidePanelOpenForTab(
    const Extension& extension,
    content::BrowserContext* context,
    int tab_id,
    bool include_incognito_information) {
  // 检查面板是否禁用
  api::side_panel::PanelOptions current_options =
      GetOptions(extension, tab_id);
  if (!current_options.enabled.value_or(true)) {
    return false;  // 禁用状态不算错误，返回 false
  }

  // 调用底层检查
  bool is_open = side_panel_util::IsContextualExtensionSidePanelOpen(
      browser_window, web_contents, extension.id());

  return is_open;
}
```

### 2.4 ThirdPartyLlm 面板

`ThirdPartyLlmPanelCoordinator` 管理一个可以加载任意 LLM 提供商 URL 的侧边栏面板。

#### 功能特性

| 功能 | 说明 |
|------|------|
| **多提供商支持** | ChatGPT、Claude、Grok、Gemini、Perplexity |
| **提供商切换** | Combobox 下拉选择，支持快捷键 `Ctrl+Shift+L` 循环切换 |
| **页面内容复制** | 通过 Accessibility Tree 提取网页文本，复制到剪贴板 |
| **截图功能** | 捕获可见区域截图，复制到剪贴板 |
| **刷新** | 重置为提供商默认 URL |
| **新标签页打开** | 在浏览器主标签页中打开当前面板 URL |
| **媒体权限自动授予** | 对已知 LLM 域名自动授予麦克风权限 |

#### 面板布局

```
┌─────────────────────────────────────────────────────┐
│ [提供商▼]  [📋] [📷] [🔄] [↗] [⋮]          │  <- Header
│ Copied to clipboard                            │  <- Feedback label
├─────────────────────────────────────────────────┤
│                                                 │
│         WebView (LLM 网页内容)                   │
│                                                 │
├─────────────────────────────────────────────────┤
│  ⌨️  Toggle: Ctrl+Shift+K • Next: Ctrl+Shift+L  │  <- Footer
└─────────────────────────────────────────────────┘
```

#### 状态持久化

```cpp
// 偏好设置键名
const char kThirdPartyLlmProvidersPref[] =
    "browseros.third_party_llm.providers";       // List: [{name, url}, ...]
const char kThirdPartyLlmSelectedProviderPref[] =
    "browseros.third_party_llm.selected_provider"; // Integer: index
```

提供商列表和当前选中项都保存在用户偏好设置中，面板关闭重启后恢复。

#### 键盘快捷键

| 快捷键 (Windows) | 快捷键 (macOS) | 功能 |
|-------------------|------------------|------|
| `Ctrl+Shift+K` | `⌘+Shift+K` | 切换面板显示/隐藏 |
| `Ctrl+Shift+L` | `⌘+Shift+L` | 切换到下一个 LLM 提供商 |

### 2.5 ClashOfGpts — 多 LLM 对比窗口

`ClashOfGptsCoordinator` 管理一个**独立弹出窗口**，支持同时显示多个 LLM 面板进行对比。

#### 功能特性

| 功能 | 说明 |
|------|------|
| **多 Pane 显示** | 2 个或 3 个 Pane，每个加载不同的 LLM |
| **独立窗口** | 不是侧边栏，而是顶级窗口 (`TYPE_WINDOW`) |
| **提供商切换** | 每个 Pane 独立切换 LLM 提供商 |
| **状态持久化** | Pane 数量、每个 Pane 的提供商选择、最后访问的 URL |
| **内容复制** | 将当前网页内容复制到所有 Pane 的剪贴板 |

#### 窗口布局

```
┌────────────────────────────────────────────────────────────────┐
│  Clash of GPTs                                    [×]    │
├─────────────────────┬─────────────────────┬─────────────────┤
│  [ChatGPT ▼]       │  [Claude ▼]       │  [Grok ▼]      │
├─────────────────────┼─────────────────────┼─────────────────┤
│                     │                     │                   │
│   WebView           │   WebView           │   WebView        │
│   (ChatGPT)        │   (Claude)         │   (Grok)        │
│                     │                     │                   │
│                     │                     │                   │
├─────────────────────┴─────────────────────┴─────────────────┤
│  ⌨️  Next: Ctrl+Shift+L │ [Copy Content to All Panes]     │
└────────────────────────────────────────────────────────────────┘
```

#### Pane 状态管理

```cpp
// 每个 Pane 的提供商索引
size_t pane_provider_indices_[kMaxPanes];  // kMaxPanes = 3

// 每个 (pane_index, provider_index) 的最后一个 URL
std::map<std::pair<int, size_t>, GURL> last_urls_;

// 当前 Pane 数量 (2 或 3)
int current_pane_count_;
```

#### 窗口大小

```cpp
// 2 个 Pane: ~1000px 宽度
// 3 个 Pane: ~1400px 宽度
int window_width = 1400;
gfx::Size window_size(window_width, 800);
widget_->CenterWindow(window_size);
```

### 2.6 侧边栏入口类型 (SidePanelEntryId)

BrowserOS 在标准 Chromium 侧边栏入口类型基础上，新增了两个类型：

```cpp
// side_panel_entry_id.h
V(kThirdPartyLlm, kActionSidePanelShowThirdPartyLlm, "ThirdPartyLlm")
V(kClashOfGpts, kActionSidePanelShowClashOfGpts, "ClashOfGpts")
```

这允许侧边栏注册并展示这些自定义面板。

### 2.7 动画控制

BrowserOS 添加了 `animations_disabled_browseros_` 标志来控制侧边栏动画：

```cpp
// side_panel.h
bool animations_disabled_ = false;
bool animations_disabled_browseros_ = true;  // BrowserOS: 默认禁用动画

// side_panel.cc
bool SidePanel::ShouldShowAnimation() const {
  bool should_show_animations =
      gfx::Animation::ShouldRenderRichAnimation() &&
      !animations_disabled_ &&
      animations_disabled_browseros_;  // 默认 false，不显示动画
  // ...
}
```

**效果**：侧边栏打开/关闭时没有滑动动画，立即显示/隐藏。

### 2.8 焦点行为修改

BrowserOS 修改了 `SidePanelCoordinator::PopulateSidePanel()` 的焦点逻辑，确保每次切换侧边栏时，侧边栏内容都能获得焦点：

```cpp
// 原代码：只有从空白状态切换时才请求焦点
// if (previous_entry) {
//   previous_entry->OnEntryHidden();
// } else {
//   content->RequestFocus();
// }

// BrowserOS 修改：每次切换都请求焦点
previous_entry->OnEntryHidden();
content->RequestFocus();  // 确保侧边栏内容获得焦点
```

### 2.9 工具栏按钮集成

`CreateBrowserosToggleSidePanelActionCallback()` 创建了一个自定义的动作回调，绑定到浏览器工具栏的 BrowserOS 图标按钮：

```cpp
// side_panel_action_callback.cc
actions::ActionItem::InvokeActionCallback
CreateBrowserosToggleSidePanelActionCallback(
    const extensions::ExtensionId& extension_id,
    BrowserWindowInterface* bwi) {
  return base::BindRepeating(
      [](extensions::ExtensionId extension_id,
         BrowserWindowInterface* bwi, ...) {
        // 获取当前活动 Tab
        tabs::TabInterface* active_tab = bwi->GetActiveTabInterface();
        content::WebContents* active_contents =
            active_tab->GetContents();
        int tab_id = extensions::ExtensionTabUtil::GetTabId(
            active_contents);

        // 调用 SidePanelService 的切换方法
        auto result = service->BrowserosToggleSidePanelForTab(
            *extension, profile, tab_id,
            /*include_incognito_information=*/true,
            /*desired_state=*/std::nullopt);  // nullopt = toggle
      }, ...);
}
```

当用户点击浏览器工具栏上的 BrowserOS 图标时，这个回调被触发，实现侧边栏的切换。

### 2.10 媒体权限自动授予

对于已知的 LLM 提供商域名，BrowserOS 自动授予麦克风权限（用于语音输入），无需用户授权弹窗：

```cpp
void ThirdPartyLlmPanelCoordinator::RequestMediaAccessPermission(
    content::WebContents* web_contents,
    const content::MediaStreamRequest& request,
    content::MediaResponseCallback callback) {
  // 检查是否来自已知 LLM 提供商
  const GURL origin_url = request.url_origin.GetURL();
  bool is_known_llm = false;
  for (const auto& provider : providers_) {
    if (origin_url.host() == provider.url.host()) {
      is_known_llm = true;
      break;
    }
  }

  if (is_known_llm) {
    // 自动授予权限
    blink::mojom::StreamDevicesSetPtr stream_devices_set =
        blink::mojom::StreamDevicesSet::New();
    // ... 填充默认音频设备 ...
    std::move(callback).Run(*stream_devices_set, ...);
  } else {
    // 非 LLM 网站，使用标准权限流程
    MediaCaptureDevicesDispatcher::GetInstance()
        ->ProcessMediaAccessRequest(...);
  }
}
```

---

## 3. 扩展层：React 应用架构

> **代码路径**：`packages/browseros-agent/apps/agent/`

### 3.1 双入口应用架构

BrowserOS 有两个独立的 React 应用入口，分别打包为不同的 HTML 页面：

#### 主应用入口 (`entrypoints/app/App.tsx`)

主应用使用 `HashRouter`，配置三种布局：

```tsx
<HashRouter>
  <Routes>
    {/* 主导航布局 - 悬停展开侧边栏 */}
    <Route element={<SidebarLayout />}>
      <Route path="home" element={<NewTabLayout />}>
        <Route index element={<NewTab />} />
        <Route path="chat" element={<InlineChat />} />
      </Route>
      <Route path="connect-apps" element={<ConnectMCP />} />
      <Route path="scheduled" element={<ScheduledTasksPage />} />
    </Route>

    {/* 设置页布局 - 固定侧边栏 */}
    <Route element={<SettingsSidebarLayout />}>
      <Route path="settings">...</Route>
    </Route>

    {/* Onboarding - 无侧边栏 */}
    <Route path="onboarding" element={<Onboarding />} />
  </Routes>
</HashRouter>
```

#### 侧边栏入口 (`entrypoints/sidepanel/App.tsx`)

独立的聊天应用，仅包含 `ChatLayout`：

```tsx
<HashRouter>
  <Routes>
    <Route element={<ChatLayout />}>
      <Route path="/" element={<Chat />} />
      <Route path="/history" element={<ChatHistory />} />
    </Route>
  </Routes>
</HashRouter>
```

### 3.2 路由系统

#### HashRouter

由于 Chrome 扩展的页面通过 `chrome-extension://` 协议加载，无法使用服务端路由，因此统一采用 **客户端 HashRouter**：

- 主应用：`/app.html#/home/chat?q=...&mode=agent`
- 侧边栏：`/sidepanel.html#/` 或 `/sidepanel.html#/history`

#### 页面间导航

侧边栏与主应用通过 `window.open('/app.html#/settings', '_blank')` 进行跨页面导航。Background Service Worker 通过 `chrome.sidePanel.browserosToggle` 控制侧边栏开关。

### 3.3 布局系统

| 布局 | 组件文件 | 用途 | 侧边栏类型 | 关键特征 |
|------|---------|------|-----------|----------|
| **SidebarLayout** | `components/layout/SidebarLayout.tsx` | 主应用（Home、Connect Apps、Scheduled） | 悬停展开 Rail | `fixed` 定位，`w-14` → `w-64` |
| **SettingsSidebarLayout** | `components/layout/SettingsSidebarLayout.tsx` | 设置页 | 固定宽度 | `w-64`，不折叠 |
| **ChatLayout** | `components/layout/ChatLayout.tsx` | 侧边栏聊天 | 无侧边栏 | 全屏 `h-screen` |
| **NewTabLayout** | `screens/newtab/layout/NewTabLayout.tsx` | 新标签页 | 继承 SidebarLayout | 背景网格 + `Outlet` |

#### SidebarLayout（悬停展开侧边栏）

核心实现逻辑：

```tsx
// 桌面端：fixed 定位侧边栏 + pl-14 偏移内容区
<div className="fixed inset-y-0 left-0 z-40"
  onMouseEnter={handleMouseEnter}
  onMouseLeave={handleMouseLeave}>
  <AppSidebar expanded={sidebarOpen} />
</div>

<main className="pl-14"> {/* 为折叠的侧边栏留出空间 */}
  <Outlet />
</main>
```

- **折叠宽度**：`w-14` (56px)
- **展开宽度**：`w-64` (256px)
- **延迟折叠**：`COLLAPSE_DELAY = 150ms`，防止鼠标快速滑过导致闪烁
- **过渡动画**：`transition-all duration-200 ease-in-out`
- **移动端**：顶部 Header + `Sheet` 抽屉展示侧边栏

#### ChatLayout（侧边栏聊天布局）

全屏布局，无侧边栏，结构简单：

```tsx
<div className="flex h-screen w-screen flex-col overflow-hidden">
  <ChatHeader ... />
  <div className="flex-1 overflow-hidden">
    <Outlet /> {/* Chat 或 ChatHistory */}
  </div>
</div>
```

### 3.4 侧边栏组件体系

#### 组件层级

```
SidebarLayout
└── AppSidebar (expanded: boolean)
    ├── SidebarBranding
    │   ├── 用户头像 / 产品 Logo
    │   └── DropdownMenu (Profile / Sign out / Sign in)
    ├── SidebarNavigation
    │   └── NavItem[] (Home / Connect Apps / Scheduled / Settings)
    └── SidebarUserFooter
        ├── Shortcuts 按钮
        └── About BrowserOS 链接
```

#### AppSidebar

侧边栏容器，通过 `expanded` prop 控制宽度：

```tsx
<div className={cn(
  'flex h-full flex-col border-r bg-sidebar text-sidebar-foreground transition-all duration-200 ease-in-out',
  expanded ? 'w-64' : 'w-14',
)}>
```

- **展开状态**：显示文字标签、品牌名称、用户信息
- **折叠状态**：仅显示图标，文字通过 `opacity-0` 隐藏
- **Tooltip**：折叠时为每个导航项显示 Tooltip

#### SidebarNavigation

主导航菜单，4 个导航项：

| 路由 | 图标 | 功能 |
|------|------|------|
| `/home` | Home | 新标签页 |
| `/connect-apps` | Plug | 连接 MCP 应用 |
| `/scheduled` | Calendar | 定时任务 |
| `/settings` | Settings | 设置页 |

导航项通过 `useCapabilities()` 过滤 feature-gated 功能：

```tsx
const { supports } = useCapabilities()
if (!supports(Feature.SOME_FEATURE)) return null
```

#### SettingsSidebar（设置页专用侧边栏）

固定 `w-64`，不折叠，分组导航：

- **Provider Settings**：AI & Agents / Chat & Council Provider
- **Other**：Appearance / Shortcuts / Notifications / Privacy
- **Help**：Documentation / Report Issue

支持两种导航项类型：
- 内部路由：`NavLink`（如 `/settings/general`）
- 外部链接：`<a>`（如 GitHub Issues）

---

## 4. 主题与样式系统

### 4.1 Tailwind CSS v4 配置

使用 Tailwind v4 的新语法，在 `styles/global.css` 中通过 `@theme inline` 定义设计令牌：

```css
@theme inline {
  --font-sans: "Geist", "Geist Fallback";
  --font-mono: "Geist Mono", "Geist Mono Fallback";
  --color-background: var(--background);
  --color-foreground: var(--foreground);
  --color-accent-orange: var(--accent-orange);
  ...
}
```

### 4.2 CSS 变量主题系统

使用 OKLCH 色彩空间定义 Light/Dark 主题（更科学的色彩管理）：

```css
:root {
  --background: oklch(0.985 0.002 85);
  --foreground: oklch(0.141 0.005 285.823);
  --accent-orange: oklch(0.6781 0.1663 43.21);
  --accent-orange-bright: oklch(0.7531 0.1963 43.21);
  ...
}

.dark {
  --background: oklch(0.25 0.01 265);
  --foreground: oklch(0.92 0 0);
  ...
}
```

### 4.3 ThemeProvider

基于 React Context 的主题管理：

```tsx
<ThemeProvider defaultTheme="system">
  {/* 监听系统主题变化 */}
  {/* 通过 className 切换 .dark */}
</ThemeProvider>
```

- 支持 `system` / `light` / `dark` 三种模式
- 使用 `matchMedia('(prefers-color-scheme: dark)')` 监听系统主题
- 通过 `themeStorage` (chrome.storage) 持久化用户选择

### 4.4 品牌色

BrowserOS 使用橙色作为品牌强调色：
- `--accent-orange`: `oklch(0.6781 0.1663 43.21)`
- 用于：发送按钮、选中状态、加载动画、Tab 计数徽章

---

## 5. 新标签页 UI

### 5.1 NewTab 组件架构

`NewTab.tsx` 是项目中最复杂的 UI 组件，功能包括：

1. **搜索框**（downshift `useCombobox`）
2. **语音输入**
3. **Tab 上下文选择**（`@mention` 功能）
4. **AI 建议**
5. **MCP Apps 入口**

### 5.2 搜索框实现

搜索框使用 **downshift** 的 `useCombobox` hook 实现下拉建议：

```tsx
const {
  isOpen,
  getMenuProps,
  getInputProps,
  highlightedIndex,
  getItemProps,
} = useCombobox<SuggestionItem>({
  items: flatItems,
  onSelectedItemChange({ selectedItem }) {
    runSelectedAction(selectedItem)
  },
})
```

### 5.3 动作类型

搜索建议分为三种动作类型：

| 类型 | 行为 |
|------|------|
| `search` | 跳转到搜索引擎（Google/Bing 等） |
| `ai-tab` | 创建 AI Tab（Agent 模式浏览） |
| `browseros` | 打开 AI 对话（Chat 或 Agent 模式） |

### 5.4 GlowingBorder 发光边框

搜索框使用自定义 SVG 动画实现动态发光边框：

```tsx
<GlowingBorder duration={2000} rx="1.5rem" ry="1.5rem">
  <GlowingElement />
</GlowingBorder>
```

- 使用 `motion/react` 的 `useAnimationFrame` 驱动动画
- 两个发光点沿 SVG 路径反向运动
- 使用 `getTotalLength()` 和 `getPointAtLength()` 计算路径位置

### 5.5 @mention Tab 选择器

输入 `@` 触发 `TabPickerPopover`，支持：
- 模糊搜索 Tab 标题和 URL
- 多选 Tab
- 键盘导航（↑/↓/Enter/Escape）

---

## 6. 侧边栏聊天 UI

### 6.1 Chat 组件状态机

```tsx
const {
  mode,           // 'chat' | 'agent'
  messages,       // UIMessage[]
  status,         // 'streaming' | 'submitted' | 'ready' | 'error'
  sendMessage,
  stop,
  agentUrlError,
  chatError,
  canSend,
} = useChatSessionContext()
```

### 6.2 ChatHeader

顶部导航栏，包含：
- **Provider 选择器**：切换 AI 提供商（BrowserOS / OpenAI / Anthropic 等）
- **Credits 徽章**：BrowserOS 提供商显示剩余额度
- **新对话按钮**：`Plus` 图标
- **历史记录**：`History` 图标，链接到 `/history`
- **GitHub 星标**：外链到仓库
- **设置**：外链到 `/app.html#/settings`
- **主题切换**：`ThemeToggle`
- **快捷键**：`Ctrl+B`（或 `Cmd+B`）切换侧边栏

### 6.3 ChatFooter

底部输入区，包含多层工具栏：

```
ChatFooter
├── ChatAttachedTabs      // 已附加的 Tab 标签
├── ChatSelectedText      // 页面选中的文本
├── 工具栏
│   ├── ChatModeToggle    // Chat / Agent 模式切换
│   ├── Tab 附加按钮 (@)  
│   ├── WorkspaceSelector // 工作区选择
│   └── AppSelector       // MCP 应用选择
└── ChatInput             // 输入框 + 发送按钮
```

### 6.4 ChatInput

基于 `textarea` 的输入组件，支持：

- **多行输入**：`field-sizing-content max-h-60`
- **@mention Tab 选择**：自定义 mention 状态机
- **语音输入**：录音波形可视化
- **快捷键**：`Enter` 发送，`Shift+Enter` 换行

Mention 状态机实现：

```tsx
interface MentionState {
  isOpen: boolean      // 弹窗是否打开
  filterText: string   // @ 后的过滤文本
  startPosition: number // @ 的位置
}
```

### 6.5 ChatMessages

消息列表渲染，核心特征：

- 使用 `use-stick-to-bottom` 实现自动滚动
- 消息分段渲染：`text` / `reasoning` / `tool-batch` / `nudge`
- 每条 Assistant 消息附带操作栏（复制 / 点赞 / 点踩）
- 流式加载指示器：橙色圆点弹跳动画

---

## 7. AI 消息渲染系统

### 7.1 ai-elements 组件库

位于 `components/ai-elements/`，一套专门用于渲染 AI 对话的复合组件：

| 组件 | 用途 |
|------|------|
| `Conversation` | 消息列表容器，自动滚动到底部 |
| `Message` | 单条消息（User / Assistant） |
| `MessageContent` | 消息内容容器 |
| `MessageResponse` | Markdown 渲染（基于 Streamdown） |
| `MessageActions` | 消息操作栏 |
| `MessageAttachment` | 文件/图片附件 |
| `Reasoning` | 推理过程折叠面板 |
| `MessageBranch` | 消息分支切换（多回复） |

### 7.2 消息分段渲染

`getMessageSegments()` 将消息拆分为多个段落类型：

```tsx
segments.map((segment) => {
  switch (segment.type) {
    case 'text': return <MessageResponse>{segment.text}</MessageResponse>
    case 'reasoning': return <Reasoning>...</Reasoning>
    case 'tool-batch': return <ToolBatch tools={segment.tools} />
    case 'nudge': return <ConnectAppCard data={segment.data} />
  }
})
```

### 7.3 Reasoning（推理面板）

使用 Radix Collapsible 实现可折叠的推理过程：

- **自动展开**：流式输出时自动展开
- **自动关闭**：流式结束后 1 秒自动折叠（仅一次）
- **计时显示**：记录推理耗时

```tsx
<Reasoning isStreaming={segment.isStreaming}>
  <ReasoningTrigger />  {/* "Thinking..." / "Thought for X seconds" */}
  <ReasoningContent>{segment.text}</ReasoningContent>
</Reasoning>
```

### 7.4 Streamdown 实时渲染

`MessageResponse` 使用 `Streamdown` 组件实时渲染 Markdown：

```tsx
<Streamdown
  shikiTheme={['catppuccin-latte', 'catppuccin-mocha']}
>
  {segment.text}
</Streamdown>
```

- 支持代码高亮（Shiki）
- 支持深色/浅色主题切换
- 表格宽度自适应侧边栏宽度：`w-[calc(100vw-64px)]`

---

## 8. 交互与动画

### 8.1 动画库选择

| 库 | 用途 |
|----|------|
| **motion/react** | 组件进入/退出动画、布局动画、SVG 路径动画 |
| **Tailwind CSS** | 简单过渡（hover、focus） |
| **CSS @keyframes** | 循环动画（float、shimmer、pulse） |

### 8.2 常用动画模式

**消息进入动画**：
```tsx
<motion.div
  initial={{ opacity: 0, height: 0 }}
  animate={{ opacity: 1, height: 'auto' }}
  exit={{ opacity: 0, height: 0 }}
  transition={{ duration: 0.2 }}
/>
```

**流式加载指示器**：
```tsx
<div className="flex items-center gap-1">
  <span className="h-1.5 w-1.5 animate-bounce rounded-full bg-[var(--accent-orange)] [animation-delay:-0.3s]" />
  <span className="h-1.5 w-1.5 animate-bounce rounded-full bg-[var(--accent-orange)] [animation-delay:-0.15s]" />
  <span className="h-1.5 w-1.5 animate-bounce rounded-full bg-[var(--accent-orange)]" />
</div>
```

**空状态入场**：
```tsx
<div className={cn(
  'opacity-0 transition-all duration-700',
  mounted ? 'translate-y-0 opacity-100' : 'translate-y-4 opacity-0'
)}>
```

### 8.3 TabPickerPopover

两种使用模式：

1. **mention 模式**：输入 `@` 触发，依附于输入框光标位置
2. **selector 模式**：点击按钮触发，下拉菜单位置

基于 Radix Popover + Command 组件实现：
- `CommandInput` 过滤 Tab
- `CommandList` 展示 Tab 列表
- `TabListItem` 渲染单个 Tab 项（favicon + 标题 + URL）

---

## 9. 状态管理与上下文

### 9.1 ChatSessionProvider

聊天状态的核心 Context，提供：

```tsx
interface ChatSessionContext {
  mode: ChatMode                    // 'chat' | 'agent'
  messages: UIMessage[]
  status: ChatStatus
  sendMessage: (msg: SendMessageArgs) => void
  stop: () => void
  selectedProvider: Provider
  providers: Provider[]
  liked: Record<string, boolean>
  disliked: Record<string, boolean>
  onClickLike: (id: string) => void
  onClickDislike: (id: string, comment?: string) => void
}
```

### 9.2 Feature Flag 系统

通过 `useCapabilities()` hook 控制功能可见性：

```tsx
enum Feature {
  MANAGED_MCP_SUPPORT    // 托管 MCP 服务器
  ALPHA_FEATURES_SUPPORT // Alpha 功能
  NEWTAB_CHAT_SUPPORT    // 新标签页内联聊天
  WORKSPACE_FOLDER_SUPPORT // 工作区文件夹
  CREDITS_SUPPORT        // 额度系统
}
```

功能标志从服务器配置动态获取，支持运行时切换。

### 9.3 语音输入

`useVoiceInput()` hook 封装语音功能：

```tsx
const voice = useVoiceInput()
// voice.isRecording      // 是否正在录音
// voice.isTranscribing   // 是否正在转录
// voice.transcript       // 转录文本
// voice.audioLevels      // 音频电平数组（用于波形可视化）
// voice.startRecording() // 开始录音
// voice.stopRecording()  // 停止录音
```

---

## 10. 原生层与扩展层的交互

### 10.1 通信架构

```
┌─────────────────────────────────────────────────────────────┐
│                     用户操作                                │
│  (点击工具栏图标 / 按 Ctrl+B / 点击关闭按钮)                │
└────────────────────┬───────────────────────────────────────┘
                     │
                     ▼
┌─────────────────────────────────────────────────────────────┐
│           扩展层 (TypeScript)                               │
│  ┌─────────────────────────────────────────────────────┐    │
│  │  ChatHeader.tsx / toggleSidePanel.ts               │    │
│  │  chrome.sidePanel.browserosToggle({ tabId })       │    │
│  └───────────────────────┬─────────────────────────┘    │
└──────────────────────────┼───────────────────────────────┘
                           │
                           ▼
┌─────────────────────────────────────────────────────────────┐
│          原生层 (C++)                                       │
│  ┌─────────────────────────────────────────────────────┐    │
│  │  side_panel_api.cc                                  │    │
│  │  SidePanelBrowserosToggleFunction::RunFunction()    │    │
│  └───────────────────────┬─────────────────────────┘    │
│                           │                               │
│  ┌───────────────────────▼─────────────────────────┐    │
│  │  side_panel_service.cc                            │    │
│  │  BrowserosToggleSidePanelForTab()                 │    │
│  └───────────────────────┬─────────────────────────┘    │
│                           │                               │
│  ┌───────────────────────▼─────────────────────────┐    │
│  │  side_panel_coordinator.cc                       │    │
│  │  ToggleContextualExtensionSidePanel()             │    │
│  └───────────────────────┬─────────────────────────┘    │
└──────────────────────────┼───────────────────────────────┘
                           │
                           ▼
┌─────────────────────────────────────────────────────────────┐
│                  Chromium 侧边栏 UI                         │
│  (显示 / 隐藏 sidepanel.html)                              │
└─────────────────────────────────────────────────────────────┘
```

### 10.2 关键交互流程

#### 打开侧边栏

1. 用户点击工具栏图标
2. `side_panel_action_callback.cc` 中的回调被触发
3. 获取当前活动 Tab 的 `tabId`
4. 调用 `SidePanelService::BrowserosToggleSidePanelForTab()`
5. 自动注册 contextual panel options（如果不存在）
6. 调用 `side_panel_util::ToggleContextualExtensionSidePanel()`
7. 侧边栏显示 `sidepanel.html`

#### 在侧边栏中切换 Provider

1. 用户在 `ChatHeader` 中选择 Provider
2. 更新 `ChatSessionContext.selectedProvider`
3. 后续消息发送到新 Provider

#### 从侧边栏打开设置页

1. 用户点击 `ChatHeader` 中的设置图标
2. 调用 `window.open('/app.html#/settings', '_blank')`
3. 新标签页打开主应用，显示设置页
4. 设置页使用 `SettingsSidebarLayout`，固定宽度侧边栏

---

## 11. 关键文件索引

### 扩展层（TypeScript/React）

| 文件 | 说明 |
|------|------|
| `entrypoints/app/App.tsx` | 主应用路由配置 |
| `entrypoints/sidepanel/App.tsx` | 侧边栏路由配置 |
| `entrypoints/background/index.ts` | Background Service Worker |
| `components/layout/SidebarLayout.tsx` | 悬停展开侧边栏布局 |
| `components/layout/SettingsSidebarLayout.tsx` | 设置页固定侧边栏布局 |
| `components/layout/ChatLayout.tsx` | 侧边栏聊天全屏布局 |
| `components/sidebar/AppSidebar.tsx` | 侧边栏容器 |
| `components/sidebar/SidebarNavigation.tsx` | 主导航菜单 |
| `components/sidebar/SidebarBranding.tsx` | 侧边栏品牌/用户区 |
| `components/sidebar/SidebarUserFooter.tsx` | 侧边栏底部区域 |
| `components/sidebar/SettingsSidebar.tsx` | 设置页侧边栏 |
| `screens/newtab/index/NewTab.tsx` | 新标签页主组件 |
| `screens/sidepanel/index/Chat.tsx` | 侧边栏聊天主组件 |
| `screens/sidepanel/index/ChatHeader.tsx` | 聊天头部 |
| `screens/sidepanel/index/ChatFooter.tsx` | 聊天底部工具栏 |
| `screens/sidepanel/index/ChatInput.tsx` | 聊天输入框 |
| `screens/sidepanel/index/ChatMessages.tsx` | 消息列表 |
| `components/ai-elements/conversation.tsx` | 对话容器 |
| `components/ai-elements/message.tsx` | 消息组件 |
| `components/ai-elements/reasoning.tsx` | 推理面板 |
| `components/elements/glowing-border.tsx` | 发光边框动画 |
| `components/elements/tab-picker-popover.tsx` | Tab 选择器 |
| `components/elements/theme-toggle.tsx` | 主题切换按钮 |
| `styles/global.css` | 全局样式与主题变量 |
| `components/theme-provider.tsx` | 主题上下文 |
| `lib/browseros/toggleSidePanel.ts` | 侧边栏切换 API 封装 |

### 原生层（C++）

| 文件 | 说明 |
|------|------|
| `chromium_patches/.../extensions/api/side_panel/side_panel_api.cc` | 自定义扩展 API 实现 |
| `chromium_patches/.../extensions/api/side_panel/side_panel_service.cc` | 侧边栏服务逻辑 |
| `chromium_patches/.../ui/views/side_panel/third_party_llm/third_party_llm_panel_coordinator.cc` | 第三方 LLM 面板协调器 |
| `chromium_patches/.../ui/views/side_panel/clash_of_gpts/clash_of_gpts_coordinator.cc` | 多 LLM 对比窗口协调器 |
| `chromium_patches/.../ui/views/side_panel/side_panel_coordinator.cc` | 侧边栏协调器（焦点修改） |
| `chromium_patches/.../ui/views/side_panel/side_panel.cc` | 侧边栏视图（动画控制） |
| `chromium_patches/.../ui/side_panel/side_panel_entry_id.h` | 侧边栏入口类型定义 |
| `chromium_patches/.../ui/side_panel/side_panel_prefs.cc` | 侧边栏偏好设置 |
| `chromium_patches/.../ui/side_panel/side_panel_action_callback.cc` | 工具栏按钮回调 |
