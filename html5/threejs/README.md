# 下载 three.js

`https://github.com/mrdoob/three.js`

# 目录说明

- build目录：包含两个文件，three.js 和three.min.js 。这是three.js最终被引用的文件。一个已经压缩，一个没有压缩的js文件。
- docs目录：这里是three.js的帮助文档，里面是各个函数的api，可惜并没有详细的解释。试图用这些文档来学会three.js是不可能的。
- editor目录：一个类似3D-max的简单编辑程序，它能创建一些三维物体。
- examples目录：一些很有趣的例子demo，可惜没有文档介绍。对图像学理解不深入的同学，学习成本非常高。
- src目录：源代码目录，里面是所有源代码。
- test目录：一些测试代码，基本没用。
- utils目录：存放一些脚本，python文件的工具目录。例如将3D-Max格式的模型转换为three.js特有的json模型。

# 三大组建

在Three.js中，要渲染物体到网页中，我们需要3个组建：场景（scene）、相机（camera）和渲染器（renderer）。

## 场景（scene）

场景是所有物体的容器

```js
var scene = new THREE.Scene();  // 场景
```

## 相机（camera）

相机决定了场景中那个角度的景色会显示出来，在Threejs中有多种相机，它们是：

### 普通的相机（THREE.Camera）

有四个参数：
- `fov` 垂直方向的观察角度，也就是眼睛上下俯视的观察角度
- `aspect` 视窗的宽高比
- `near` 最近视角 相机能看见的最近视角
- `far` 最远视角  相机能看见的最远视角

### 正交投影的相机（THREE.OrthographicCamera）

指定位置、投影方向、向上方向和宽度初始化

### 透视投影相机（THREE.PerspectiveCamera）

透视投影相机是继承了普通相机（THREE.Camera）类来创建的一共特殊的相机，透视投影相机的目标是当物体移动时跟随它并保持一定的距离：否则当物体移动到摄像机边界后，将造成摄像机剧烈移动。让摄像机跟随一个物体，如玩家控制角色，你需要定义一下如下参数：

* 1、跟随位置，摄像机跟随的目标物体的位置
* 2、跟随的方向，摄像机沿哪个方向移动来跟随物体
* 3、跟随速度
* 4、跟随距离，摄像机与物体的距离

```js
var camera = new THREE.PerspectiveCamera(75, window.innerWidth/window.innerHeight, 0.1, 1000);
camera.position.set(0, 0, 5); // 右手坐标
scene.add(camera);
```

## 渲染器（Renderer）

最后一步就是设置渲染器，渲染器决定了渲染的结果应该画在页面的什么元素上面，并且以怎样的方式来绘制。
将渲染器对应的Canvas 元素添加到 <body> 中。

```js
var renderer = new THREE.WebGLRenderer();
renderer.setSize(window.innerWidth, window.innerHeight);
document.body.appendChild(renderer.domElement);
```

# 添加长方体（THREE.CubeGeometry）到场景中

```js
var geometry = new THREE.CubeGeometry(1,1,1);
var material = new THREE.MeshBasicMaterial({color: 0x00ff00});
var cube = new THREE.Mesh(geometry, material);
scene.add(cube);
```