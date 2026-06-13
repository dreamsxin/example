# 静态分析 UI 布局

## 方法一：直接恢复 Prefab（成功率最高）

### AssetRipper

[AssetRipper](https://github.com/AssetRipper/AssetRipper?utm_source=chatgpt.com)

如果游戏没有对资源做特殊处理，可以直接恢复：

```text
Canvas
Panel
Button
Image
Text
TMP_Text
LayoutGroup
Animator
```

导出的 Unity 工程中能看到：

```text
Canvas
 ├─ TopBar
 ├─ HeroPanel
 ├─ BottomMenu
```

并保留：

```text
RectTransform
Anchor
Pivot
Width
Height
```

这些就是 UI 布局数据。

---

## 方法二：直接读取 Prefab 数据

即使 AssetRipper 无法完整恢复，也能从资源文件中解析。

Unity UI 本质是：

```text
GameObject
 └─ RectTransform
     └─ Component
```

关键组件：

```text
RectTransform
CanvasRenderer
Image
Button
TextMeshProUGUI
```

例如：

```yaml
RectTransform:
  m_AnchorMin: {x:0,y:0}
  m_AnchorMax: {x:1,y:1}
  m_AnchoredPosition: {x:0,y:0}
  m_SizeDelta: {x:0,y:0}
```

可以直接重建布局。

---

## 方法三：AssetStudio 查看层级

[AssetStudio](https://github.com/Perfare/AssetStudio?utm_source=chatgpt.com)

虽然不能像 AssetRipper 那样恢复工程，但可以查看：

```text
Prefab
MonoBehaviour
GameObject
```

关联关系。

例如：

```text
HeroPanel
 ├─ Bg
 ├─ HeroList
 ├─ BtnClose
```

---

# 方法四：解析 MonoBehaviour 字段

很多项目采用：

```text
空Prefab
+
代码绑定
```

例如：

```csharp
public Button btnClose;
public Image avatar;
public TMP_Text nameText;
```

通过 Il2CppDumper 导出后可看到：

```csharp
HeroPanel
{
    btnClose
    avatar
    nameText
}
```

从而知道 UI 结构。

---

# 方法五：分析 FairyGUI

很多国产手游不用 Unity UI。

常见：

```text
FairyGUI
FGUI
```

特征：

```text
ui.package
_fui
UIPackage
```

相关项目：

[FairyGUI](https://fairygui.com?utm_source=chatgpt.com)

布局数据通常在：

```xml
package.xml

component.xml
```

里面。

例如：

```xml
<component>
    <image id="bg"/>
    <button id="btn_ok"/>
</component>
```

直接描述界面结构。

---

# 方法六：分析 Lua UI 配置

很多 Lua 框架：

```text
xlua
tolua
slua
```

UI 不在 Prefab 中，而是配置驱动。

例如：

```lua
HeroPanel = {
    path = "ui/hero/HeroPanel"
}
```

或者：

```lua
self.btnClose
self.txtName
self.imgAvatar
```

从 Lua 就能恢复层级关系。

---

# 方法七：静态恢复 Spine UI

很多界面动画实际上是：

```text
Canvas
 └─ SkeletonGraphic
```

而非普通 Image。

搜索：

```text
SkeletonGraphic
```

即可定位：

```text
LoginPanel
 └─ SpineLogo

HeroPanel
 └─ SpineCharacter
```

对应的骨骼资源引用。

---

# 方法八：直接解析 RectTransform

如果你想自动生成 UI 复刻工程，可以重点提取：

```text
GameObject
RectTransform
Image
Text
Button
LayoutGroup
```

其中：

```text
AnchorMin
AnchorMax
Position
SizeDelta
Pivot
```

就足以还原 90% 的界面布局。

例如：

```json
{
  "name":"BtnStart",
  "anchorMin":[0.5,0],
  "anchorMax":[0.5,0],
  "position":[0,100],
  "size":[300,80]
}
```

然后自动生成：

```csharp
new GameObject("BtnStart")
```

对应的 Unity UI。

---

## 针对 IL2CPP 商业手游

我通常按下面顺序分析：

```text
AssetRipper
 ↓
查看Prefab
 ↓
查看Canvas结构
 ↓
查看SkeletonGraphic
 ↓
查看Animator
 ↓
Il2CppDumper分析UI类
 ↓
Lua分析
```

这样大约能静态恢复：

```text
UI层级     90%
布局       95%
图片引用   90%
Spine引用  80%
动画逻辑   50%
```

如果你手上的游戏已经确认是 **IL2CPP + Lua + 自定义 `.skel`**，那么下一步最值得做的是先确认 UI 是：

* Unity UGUI（Canvas + RectTransform）
* FairyGUI
* NGUI
* 自研 UI

以及资源是：

* 原始 AssetBundle
* Addressables
* 加密 AssetBundle

不同架构下，静态恢复方案会差别很大。你可以贴出 APK 解包后的目录结构（尤其是 `assets/bin/Data`、`StreamingAssets`、`AssetBundles` 部分），我可以帮你判断最有效的分析路径。
