# 使用 Three.js 实现 2.5D 等距游戏整体框架设计

本文档提供一套基于 Three.js 的 2.5D 等距游戏完整架构设计，涵盖从底层 3D 场景搭建到上层游戏逻辑的各个模块。与 2D 精灵方案不同，Three.js 利用 WebGL 提供硬件加速渲染、自动深度测试和丰富的 3D 特性，使开发者能够专注于游戏逻辑，同时获得更好的视觉效果和性能。

---

## 一、 设计目标

- 利用 Three.js 实现固定等距视角的 2.5D 游戏世界。
- 提供清晰的模块划分，支持模拟经营、RPG 等常见玩法。
- 兼容 2D 精灵资源（角色、建筑、道具）和 3D 模型（可选）。
- 自动处理深度遮挡，无需手动排序。
- 支持鼠标/触摸交互、网格寻路、动态加载。
- 具备良好的调试能力和性能优化空间。

---

## 二、 整体架构

```
┌─────────────────────────────────────────────────────────────┐
│                       Game Application                        │
├─────────────────────────────────────────────────────────────┤
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────────┐  │
│  │ SceneManager │  │ ResourceMgr  │  │   EventBus       │  │
│  └──────────────┘  └──────────────┘  └──────────────────┘  │
├─────────────────────────────────────────────────────────────┤
│  ┌──────────────────────────────────────────────────────┐   │
│  │               Three.js Core (渲染核心)                │   │
│  │  ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌────────┐ │   │
│  │  │Renderer  │ │ Camera   │ │  Scene   │ │ Lights │ │   │
│  │  └──────────┘ └──────────┘ └──────────┘ └────────┘ │   │
│  └──────────────────────────────────────────────────────┘   │
├─────────────────────────────────────────────────────────────┤
│  ┌──────────────────────────────────────────────────────┐   │
│  │               Game World Manager                      │   │
│  │  ┌──────────────┐ ┌──────────────┐ ┌─────────────┐  │   │
│  │  │ GridSystem   │ │ EntityManager│ │ PathFinder  │  │   │
│  │  └──────────────┘ └──────────────┘ └─────────────┘  │   │
│  └──────────────────────────────────────────────────────┘   │
├─────────────────────────────────────────────────────────────┤
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────────┐  │
│  │ InputManager │  │  UIManager   │  │   DebugSystem    │  │
│  └──────────────┘  └──────────────┘  └──────────────────┘  │
└─────────────────────────────────────────────────────────────┘
```

---

## 三、 核心模块详解

### 1. Three.js 核心层

负责初始化 Three.js 渲染器、场景、相机和灯光，提供基础的 3D 环境。

#### 1.1 渲染器 (Renderer)
- 使用 `WebGLRenderer`，开启抗锯齿和透明度。
- 设置背景色（如深灰色或天空盒）。
- 监听窗口 resize 事件，调整相机比例和渲染尺寸。

#### 1.2 相机 (Camera)
- 采用 **正交相机 (OrthographicCamera)** 以保持无透视的等距效果。
- 相机位置和朝向固定，模拟经典等距视角（例如俯角 35°，水平旋转 45°）。
- 提供公共方法供外部调整相机（如缩放、旋转，但通常固定）。

```typescript
// 示例：固定等距相机
const frustumSize = 15;
const aspect = width / height;
camera = new THREE.OrthographicCamera(
    -frustumSize * aspect, frustumSize * aspect,
    frustumSize, -frustumSize,
    0.1, 100
);
camera.position.set(10, 12, 10);
camera.lookAt(0, 0, 0);
```

#### 1.3 场景 (Scene)
- 添加必要的辅助元素（调试用 AxesHelper、GridHelper）。
- 管理所有 3D 对象：地面网格、实体、特效等。

#### 1.4 灯光 (Lights)
- 环境光 (AmbientLight) 提供基础照明。
- 方向光 (DirectionalLight) 模拟主光源（如左上角光照），产生立体感。
- 可选：点光源用于局部高亮。

---

### 2. 游戏世界管理模块

#### 2.1 网格系统 (GridSystem)
- 维护地图的二维数组，存储每个格子的数据：类型、高度、是否可行走、占用实体等。
- 提供世界坐标 ↔ 网格坐标的转换函数（基于菱形公式，与 2D 方案一致）。
- 负责动态生成/更新地面图块（可使用 InstancedMesh 或单个 Mesh 复用纹理）。

```typescript
interface TileData {
    type: string;          // 'grass', 'water', 'road'
    height: number;        // 海拔高度（单位）
    walkable: boolean;
    entity: Entity | null;
}
```

**地面生成**：根据地图尺寸，循环创建平面网格或使用单一平面 + 纹理偏移（TileMap）。为性能考虑，推荐将地面合并为几个大网格，或使用 `InstancedMesh` 批量绘制相同图块。

#### 2.2 实体管理 (EntityManager)
- 管理所有动态和静态实体（角色、建筑、物品）。
- 每个实体继承自 `Entity` 基类，包含：
  - 网格坐标 (col, row)
  - 世界坐标 (x, y, z)（自动根据网格坐标计算）
  - 视觉对象（`THREE.Object3D`：可能是 Sprite、Mesh 或 Group）
  - 更新逻辑（移动、动画等）
- 提供添加、删除、查找实体的方法。

```typescript
class Entity {
    col: number;
    row: number;
    height: number;       // 实体自身高度（楼层或视觉高度）
    object3d: THREE.Object3D;
    // 更新位置（根据网格坐标和地面高度重新计算世界坐标）
    updatePosition() {
        const { x, z } = GridSystem.gridToWorld(this.col, this.row);
        const y = GridSystem.getGroundHeight(this.col, this.row) + this.height;
        this.object3d.position.set(x, y, z);
    }
}
```

#### 2.3 寻路系统 (PathFinder)
- 基于网格的 A* 算法，使用 `walkable` 和地形代价。
- 返回路径点（网格坐标数组），供实体移动时使用。
- 可集成障碍物动态更新（如建筑放置后标记不可行走）。

---

### 3. 输入与交互

#### 3.1 输入管理器 (InputManager)
- 封装鼠标、键盘、触摸事件。
- 提供屏幕坐标 → 世界坐标 → 网格坐标的转换（通过射线检测）。
- 触发事件：`clickTile`, `clickEntity`, `keydown` 等。

**射线检测实现**：
```typescript
const raycaster = new THREE.Raycaster();
const mouse = new THREE.Vector2();

function onMouseClick(event: MouseEvent) {
    // 计算鼠标位置归一化坐标
    mouse.x = (event.clientX / renderer.domElement.clientWidth) * 2 - 1;
    mouse.y = -(event.clientY / renderer.domElement.clientHeight) * 2 + 1;
    raycaster.setFromCamera(mouse, camera);
    // 检测地面和实体
    const intersects = raycaster.intersectObjects(groundMeshes);
    if (intersects.length) {
        const point = intersects[0].point;
        const grid = GridSystem.worldToGrid(point.x, point.z);
        EventBus.emit('tileClick', grid);
    }
}
```

#### 3.2 UI 管理器 (UIManager)
- 管理所有 2D UI 元素（使用 HTML/CSS 或 Three.js 的 CSS2DRenderer）。
- 与游戏世界解耦，通过 EventBus 接收事件并更新 UI。
- 处理模态对话框、HUD、小地图等。

---

### 4. 资源管理

- 使用 Three.js 的 `LoadingManager` 和 `TextureLoader` 加载纹理、模型。
- 资源预加载，进度条显示。
- 提供资源缓存，避免重复加载。

---

### 5. 调试系统

- 在开发模式下开启辅助显示：
  - 显示网格线（Three.js 的 GridHelper 或自定义）。
  - 显示每个格子的坐标标签（使用 CSS2DRenderer 或 Sprite）。
  - 显示实体的包围盒（BoxHelper）。
  - 显示当前鼠标位置的格子高亮（叠加半透明平面）。
- 快捷键 `~` 切换调试模式。

---

## 四、 坐标系统与转换

与 2D 方案一致，但世界坐标采用 Three.js 的 XZ 平面（地面），Y 轴向上。

### 4.1 网格坐标 → 世界坐标 (XZ)
给定菱形网格参数 `tileW`（宽度，世界单位）和 `tileH`（深度，世界单位），一个格子的中心世界坐标为：
```typescript
function gridToWorld(col: number, row: number): { x: number, z: number } {
    const x = (col - row) * (tileW / 2);
    const z = (col + row) * (tileH / 2);
    return { x, z };
}
```
- `tileW` 和 `tileH` 通常取 1.0 或根据纹理尺寸确定。
- 地面高度 Y = `groundHeight * tileHeightUnit`（例如每层高度 0.5）。

### 4.2 世界坐标 → 网格坐标
```typescript
function worldToGrid(x: number, z: number): { col: number, row: number } {
    const col = (x / (tileW/2) + z / (tileH/2)) / 2;
    const row = (z / (tileH/2) - x / (tileW/2)) / 2;
    return { col: Math.round(col), row: Math.round(row) };
}
```

### 4.3 实体 Y 坐标
实体的世界 Y 坐标 = 地面高度 + 实体自身高度（如角色站立高度 0.5）。

---

## 五、 深度处理与透明度

由于 Three.js 使用深度缓冲区，大多数情况下自动解决遮挡。但需要注意：

- **Sprite 默认透明**：如果角色使用 Sprite，且建筑使用 Mesh，Sprite 会受深度测试影响，但半透明物体的渲染顺序可能导致错误。解决方案：
  - 设置 `sprite.material.depthTest = true`，并确保 Sprite 的 `renderOrder` 合适。
  - 对于透明纹理，使用 `alphaTest` 或 `transparent` 并启用 `depthWrite: false` 可能需要调整。
- **建筑半透明效果**：当角色被建筑遮挡时，可通过射线检测动态设置建筑材物的透明度（或使用 X-Ray 材质）。

---

## 六、 性能优化

1. **合并地面网格**：将相同材质的地面图块合并为一个大的 Mesh（使用 `BufferGeometry` 或 `InstancedMesh`），减少 draw calls。
2. **视锥体裁剪**：Three.js 自动进行，但需确保每个物体的包围盒正确。
3. **LOD（细节层次）**：远处的实体可使用低精度模型或不再渲染。
4. **纹理图集**：将多个小纹理打包到一张图集中，减少纹理切换。
5. **对象池**：对于频繁创建/销毁的特效、拾取物，使用对象池复用。

---

## 七、 游戏循环与事件驱动

- 使用 `requestAnimationFrame` 驱动 `renderer.render` 和实体更新（位置插值、动画）。
- 游戏逻辑更新（如 AI、生产）可使用固定时间步长或基于帧的 update。
- 所有模块通过 **EventBus** 通信，实现松耦合。

---

## 八、 模块初始化顺序（伪代码）

```typescript
class Game {
    async init() {
        await ResourceManager.load();
        const renderCore = new ThreeCore(); // 创建渲染器、相机、场景
        const gridSystem = new GridSystem(mapData);
        const entityManager = new EntityManager(gridSystem);
        const inputManager = new InputManager(renderCore.camera, renderCore.renderer);
        const uiManager = new UIManager();
        
        // 构建世界
        gridSystem.buildGround(renderCore.scene);
        
        // 创建玩家
        const player = new Player(5, 5);
        entityManager.add(player);
        
        // 启动循环
        this.startLoop(renderCore, entityManager);
    }
    
    startLoop(renderCore, entityManager) {
        function animate() {
            requestAnimationFrame(animate);
            const delta = clock.getDelta();
            entityManager.update(delta);
            renderCore.render();
        }
        animate();
    }
}
```

---

## 九、 与 2D 精灵方案的对比

| 特性 | 2D Sprite 方案 (Pixi.js) | Three.js 方案 |
|------|--------------------------|---------------|
| 深度排序 | 手动计算 sortKey 并排序 | GPU 深度测试，自动处理 |
| 渲染性能 | 受 CPU 排序影响，大量物体时可能瓶颈 | GPU 处理，适合大量物体 |
| 光影效果 | 需预烘焙或后期特效 | 实时动态光照、阴影 |
| 资源类型 | 仅 2D 图块 | 2D 图块 + 3D 模型，可混合使用 |
| 地形高度 | 通过 Y 偏移模拟，需手动处理遮挡 | 真实 3D 高度，自动遮挡 |
| 开发复杂度 | 较低，纯 2D 思维 | 需理解 3D 空间、相机、光照 |
| 调试 | 依赖自定义调试工具 | 可使用 Three.js 内置辅助工具 |

---

## 十、 扩展建议

- **动态地形**：利用 Three.js 的地形几何体（PlaneGeometry 修改顶点高度），实现更真实的地形起伏。
- **阴影**：开启阴影映射，增强立体感（注意性能）。
- **特效**：使用粒子系统（THREE.Points）实现雨雪、火焰等。
- **动画**：角色的行走动画可通过切换 Sprite 纹理或使用带骨骼的 3D 模型实现。
- **网络同步**：将实体位置、状态通过 WebSocket 同步，适用于多人游戏。

---

## 十一、 总结

使用 Three.js 实现 2.5D 等距游戏，能够充分利用 GPU 能力，简化深度处理，并支持丰富的光影和 3D 特性。框架设计以模块化为核心，通过网格系统、实体管理、事件驱动等组件构建可扩展的游戏世界。开发者可以保留原有的 2D 美术资源（作为 Sprite 或平面纹理），同时享受 3D 带来的便利。本框架为后续开发提供了清晰的蓝图，可根据具体游戏需求进一步细化各个模块。

# 整体架构各模块详细设计

本文档深入描述每个模块的职责、核心接口、关键实现和模块间协作方式，为后续编码提供精确指导。

---

## 一、 应用层模块

### 1. SceneManager（场景管理器）

**职责**：管理游戏场景的切换、生命周期和资源清理。

**关键数据结构**：
```typescript
interface Scene {
    name: string;
    onEnter: () => void;      // 进入场景时调用
    onExit: () => void;       // 退出场景时调用
    update: (delta: number) => void;
    render: () => void;       // 可选，通常由渲染循环统一调用
}
```

**主要方法**：
- `registerScene(scene: Scene)`：注册场景。
- `switchTo(sceneName: string)`：切换场景，调用当前场景的 `onExit`，新场景的 `onEnter`，并重建世界对象（如 GridSystem、EntityManager）。
- `getCurrentScene()`：返回当前场景实例。

**实现要点**：
- 维护场景栈，支持叠加场景（如暂停菜单）。
- 切换场景时，确保 Three.js 场景中移除旧世界的所有对象，并加载新世界的资源。
- 与 ResourceManager 协作预加载场景所需资源。

---

### 2. ResourceManager（资源管理器）

**职责**：统一加载、缓存和管理所有游戏资源（纹理、模型、音频、配置）。

**关键数据结构**：
```typescript
class ResourceManager {
    private textures: Map<string, THREE.Texture>;
    private models: Map<string, THREE.Group>;
    private audioBuffers: Map<string, AudioBuffer>;
    private configs: Map<string, any>;
    private loadingManager: THREE.LoadingManager;
}
```

**主要方法**：
- `loadTexture(key: string, url: string): Promise<THREE.Texture>`
- `loadModel(key: string, url: string): Promise<THREE.Group>`
- `loadAudio(key: string, url: string): Promise<AudioBuffer>`
- `loadConfig(key: string, url: string): Promise<any>`
- `getTexture(key: string): THREE.Texture`
- `getModel(key: string): THREE.Group`
- `release(key: string)`：释放资源（纹理 dispose 等）
- `preload(sceneAssets: AssetList)`：批量预加载

**实现要点**：
- 使用 `THREE.LoadingManager` 统一管理加载进度，可触发 `loadingProgress` 事件。
- 纹理和模型缓存，避免重复加载。
- 支持图集（TexturePacker）加载，减少 draw calls。
- 资源释放时机：场景切换时调用 `releaseUnused(sceneKeys)` 释放不再需要的资源。

---

### 3. EventBus（事件总线）

**职责**：全局事件通信，解耦模块。

**关键数据结构**：
```typescript
type EventCallback = (data?: any) => void;

class EventBus {
    private listeners: Map<string, EventCallback[]>;
}
```

**主要方法**：
- `on(event: string, callback: EventCallback)`：订阅事件。
- `off(event: string, callback: EventCallback)`：取消订阅。
- `emit(event: string, data?: any)`：触发事件。
- `once(event: string, callback: EventCallback)`：一次性订阅。

**实现要点**：
- 采用发布-订阅模式，所有模块通过事件通信。
- 常见事件：`'tileClick'`, `'entitySelected'`, `'resourceLoaded'`, `'sceneChanged'` 等。
- 避免内存泄漏：确保模块销毁时调用 `off` 移除其监听器。

---

## 二、 Three.js 核心渲染模块

### 1. Renderer（渲染器）

**职责**：封装 Three.js WebGLRenderer 的创建和配置。

**关键数据结构**：
```typescript
class ThreeRenderer {
    public renderer: THREE.WebGLRenderer;
    constructor(container: HTMLElement, options?: RenderOptions);
}
```

**主要方法**：
- `resize(width: number, height: number)`：调整尺寸。
- `render(scene: THREE.Scene, camera: THREE.Camera)`：执行渲染。
- `getDomElement(): HTMLCanvasElement`

**实现要点**：
- 开启 `alpha: true` 支持透明背景。
- 开启 `antialias: true` 抗锯齿。
- 设置 `pixelRatio` 适应高 DPI 屏幕。

---

### 2. Camera（相机）

**职责**：创建并管理等距视角的正交相机，提供坐标转换辅助方法。

**关键数据结构**：
```typescript
class IsoCamera {
    public camera: THREE.OrthographicCamera;
    private frustumSize: number;
    private aspect: number;
    constructor(width: number, height: number);
}
```

**主要方法**：
- `updateAspect(width: number, height: number)`：响应窗口大小变化。
- `getWorldPositionFromScreen(screenX: number, screenY: number): THREE.Vector3`：通过射线检测获取世界坐标（由 InputManager 调用）。
- `getScreenPositionFromWorld(world: THREE.Vector3): THREE.Vector2`：将世界坐标转为屏幕坐标（用于 UI 跟随等）。

**实现要点**：
- 相机位置固定，但可提供缩放功能（改变 frustumSize）。
- 坐标转换利用 `Raycaster` 与地面平面（或自定义平面）相交。

---

### 3. Scene（场景）

**职责**：管理 Three.js 场景的层级结构，添加灯光、辅助对象。

**关键数据结构**：
```typescript
class GameScene {
    public scene: THREE.Scene;
    constructor();
    public add(object: THREE.Object3D): void;
    public remove(object: THREE.Object3D): void;
    public clear(): void;   // 清除所有对象，保留灯光和辅助
}
```

**实现要点**：
- 场景包含地面、实体、特效等。
- 维护两个主要组：`staticGroup`（地形、建筑）和 `dynamicGroup`（角色、特效），方便管理。
- 调试时可添加 `AxesHelper` 和 `GridHelper`。

---

### 4. Lights（灯光）

**职责**：设置场景的光照，提供标准照明方案。

**主要组件**：
- `AmbientLight`：基础环境光，强度 0.5~0.7。
- `DirectionalLight`：主光源，方向从左上方（例如 `(5, 10, 5)`），强度 0.8~1.0，开启阴影映射。
- `FillLight`：可选，从右侧补光。

**实现要点**：
- 阴影映射性能开销大，按需开启，并设置阴影贴图分辨率（`mapSize`）。
- 可提供多种预设（如白天、夜晚）供场景切换。

---

## 三、 游戏世界管理模块

### 1. GridSystem（网格系统）

**职责**：管理地图数据、坐标转换、地面渲染和地形查询。

**关键数据结构**：
```typescript
interface TileData {
    type: string;           // 地形类型标识
    height: number;         // 海拔（世界单位）
    walkable: boolean;
    entity: Entity | null;  // 当前占据的实体
    visual: THREE.Object3D; // 地面视觉对象（合并后的网格或单个平面）
}

class GridSystem {
    private width: number;
    private height: number;
    private tiles: TileData[][];
    private tileW: number;   // 格子宽度（世界单位）
    private tileH: number;   // 格子深度（世界单位）
    private groundGroup: THREE.Group; // 地面渲染组
}
```

**主要方法**：
- `init(mapData: MapData)`：根据数据初始化网格。
- `worldToGrid(x: number, z: number): {col, row}`
- `gridToWorld(col: number, row: number): {x, z}`
- `getGroundHeight(col: number, row: number): number`（返回地面 Y 坐标）
- `isWalkable(col: number, row: number): boolean`
- `setEntity(col: number, row: number, entity: Entity | null)`
- `buildGroundMesh()`：生成或更新地面视觉（可使用 InstancedMesh 或合并几何体）。

**实现要点**：
- 坐标转换公式与 2D 方案一致，确保与 UI 和交互对齐。
- 地面渲染可采用多个材质批次，或使用单一纹理图集（TileMap）的平面，通过顶点 UV 偏移实现不同图块。
- 对于大量相同图块，使用 `InstancedMesh` 提升性能。
- 当地形高度变化时，需要更新实体位置（通过事件通知 EntityManager）。

---

### 2. EntityManager（实体管理器）

**职责**：管理所有游戏实体的生命周期、更新和渲染顺序。

**关键数据结构**：
```typescript
abstract class Entity {
    public id: string;
    public col: number;
    public row: number;
    public height: number;          // 实体自身高度（用于 Y 坐标偏移）
    public visual: THREE.Object3D;  // 模型或精灵
    public update(delta: number): void;
    public onTileChanged(): void;   // 当所在格子变化时调用（由 GridSystem 触发）
}

class EntityManager {
    private entities: Map<string, Entity>;
    private dynamicGroup: THREE.Group; // 动态物体组
}
```

**主要方法**：
- `addEntity(entity: Entity)`：添加实体，将其视觉对象添加到场景动态组。
- `removeEntity(id: string)`：移除并 dispose 视觉资源。
- `getEntity(id: string): Entity`
- `getEntitiesAt(col: number, row: number): Entity[]`（获取指定格子的所有实体，用于交互）
- `updateAll(delta: number)`：遍历调用实体 update。
- `syncPositions()`：根据实体网格坐标和地面高度更新其世界坐标（通常由 GridSystem 高度变化时调用）。

**实现要点**：
- 实体的世界坐标 = `gridToWorld(col, row)` + `(groundHeight + height) * Y轴`。
- 实体移动时，需更新其在 GridSystem 中的占用（通过 `GridSystem.setEntity`）。
- 支持排序渲染：Three.js 的深度测试自动处理，但要注意半透明物体的顺序。可设置 `renderOrder` 或使用 `transparent` 材质时启用 `depthWrite: false` 调整。

---

### 3. PathFinder（寻路系统）

**职责**：基于网格数据提供 A* 寻路算法。

**主要方法**：
- `findPath(start: {col, row}, end: {col, row}, gridSystem: GridSystem): {col, row}[]` 返回路径点数组（不含起点）。
- 支持地形代价（如草地代价 1，沼泽代价 2）。

**实现要点**：
- 使用优先队列（二叉堆）优化。
- 启发函数采用曼哈顿距离（因为等距网格下，相邻格子移动代价相等）。
- 检测障碍物：调用 `gridSystem.isWalkable` 和实体占用（`gridSystem.getTile().entity` 是否为空）。
- 若终点不可达，返回空数组。

---

## 四、 交互与辅助模块

### 1. InputManager（输入管理器）

**职责**：统一处理鼠标、键盘、触摸输入，转换为游戏事件。

**关键数据结构**：
```typescript
class InputManager {
    private camera: IsoCamera;
    private raycaster: THREE.Raycaster;
    private groundPlanes: THREE.Object3D[]; // 用于地面点击检测的物体列表
}
```

**主要方法**：
- `init(canvas: HTMLCanvasElement)`：绑定事件。
- `destroy()`：解绑事件。
- `getMouseWorldPosition(event: MouseEvent): THREE.Vector3`：射线检测获取地面交点。
- 内部处理 `click`, `mousemove`, `keydown` 等，并触发 EventBus 事件。

**实现要点**：
- 射线检测时，需要排除 UI 元素（可通过 UIManager 提供的 `isUIBlocking` 标志）。
- 键盘输入映射：WASD 移动，可转换为方向网格坐标变化，调用 `EntityManager` 移动玩家。
- 支持触摸屏的 `touchstart` 和 `touchmove`。

---

### 2. UIManager（UI管理器）

**职责**：管理所有 2D UI 组件，处理 UI 与游戏的交互。

**关键数据结构**：
```typescript
class UIManager {
    private uiContainer: HTMLDivElement;     // 绝对定位的 div
    private components: Map<string, UIComponent>;
    private blocking: boolean;               // 是否阻塞世界输入（如打开菜单时）
}
```

**主要方法**：
- `addComponent(component: UIComponent)`：添加 UI 组件（如按钮、血条）。
- `removeComponent(id: string)`
- `showDialog(content: string, onConfirm: Function)`
- `updateUI(data: any)`：根据游戏状态更新 UI（例如血量变化）。
- `isBlocking(): boolean`：供 InputManager 判断是否将输入传递给世界。

**实现要点**：
- UI 使用 HTML/CSS 或 CSS2DRenderer 实现，便于布局和样式。
- 与 EventBus 交互：监听游戏事件（如 `playerHpChanged`）并更新相应 UI 组件。
- UI 组件内部可触发事件（如 `buttonClick`），再由 UIManager 转发到 EventBus。

---

### 3. DebugSystem（调试系统）

**职责**：提供开发时的可视化调试工具，可动态开关。

**主要功能**：
- 显示网格线（使用 `GridHelper` 或自定义线框）。
- 显示每个格子的坐标文本（使用 `CSS2DRenderer` 或 Sprite）。
- 显示实体包围盒（`BoxHelper`）。
- 显示当前鼠标位置的格子高亮（叠加半透明平面）。
- 显示性能监控（fps、draw calls）。

**实现要点**：
- 通过 `EventBus` 监听 `toggleDebug` 事件（快捷键 `~`）开启/关闭。
- 调试对象应添加到单独的调试组，关闭时从场景移除。
- 性能监控可使用 `stats.js`。

---

## 五、 模块间协作示例

以下是一个典型流程（玩家点击移动）的模块协作：

1. **InputManager** 捕获鼠标点击，调用射线检测获取世界坐标，转换为网格坐标 `(col, row)`。
2. InputManager 通过 `EventBus.emit('tileClick', { col, row })` 发布事件。
3. **SceneManager**（或自定义游戏控制器）订阅事件，调用 `EntityManager.getPlayer()` 获取玩家实体。
4. 调用 `PathFinder.findPath(start, end, GridSystem)` 获取路径。
5. 玩家实体开始移动，每帧更新其网格坐标，并调用 `GridSystem.setEntity` 更新占用。
6. 玩家移动过程中，`GridSystem` 可能会触发 `tileHeightChanged` 事件，`EntityManager` 监听并调用 `syncPositions()` 更新实体位置。
7. **UIManager** 监听玩家属性变化，更新 HUD。
8. **DebugSystem** 在调试模式下高亮当前玩家格子。

---

## 六、 总结

本文档详细定义了每个模块的职责、接口和实现要点。通过这种模块化设计，2.5D 等距游戏能够保持高可维护性、可扩展性，并充分利用 Three.js 的 3D 能力。开发过程中，应遵循依赖倒置原则，模块间通过 EventBus 通信，降低耦合。后续可在此基础上逐步丰富游戏逻辑，如战斗系统、对话系统、网络同步等。
