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

# 3D世界的组成

在计算机世界里，3D世界是由点组成，两个点能够组成一条直线，三个不在一条直线上的点就能够组成一个三角形面，无数三角形面就能够组成各种形状的物体，我们通常把这种网格模型叫做Mesh模型。给物体贴上皮肤，或者专业点就叫做纹理，那么这个物体就活灵活现了。最后无数的物体就组成了我们的3D世界。

## 定义一个点

在三维空间中的某一个点可以用一个坐标点来表示。一个坐标点由x,y,z三个分量构成。在three.js中，点可以在右手坐标系中表示：

在 Three.js 是用一个向量来表示的：

```js
THREE.Vector3 = function ( x, y, z ) {
	this.x = x || 0;
	this.y = y || 0;
	this.z = z || 0;
};
```

假设有一个点x=4，y=8，z=9，可以这样定义它：

```js
var point1 = new THREE.Vecotr3(4,8,9);
# or
var point1 = new THREE.Vector3();
point1.set(4,8,9);
```

## 画一条彩色线

在 Three.js 中，一条线由点，材质和颜色组成。Threejs中没有提供单独画点的函数，它必须被放到一个THREE.Geometry形状中，这个结构中包含一个数组vertices，这个vertices就是存放无数的点（THREE.Vector3）的数组。

Three.js 中自定义形状使用的是 Geometry 类,它是其他如 CubeGeometry、SphereGeometry 等几何形状的父类。

线类型常量：有不闭合折线(THREE.LineStrip)、多组双顶点线段(THREE.LinePieces)

```js
var geometry = new THREE.Geometry();
var material = new THREE.LineBasicMaterial( { vertexColors: true } );
var color1 = new THREE.Color( 0x444444 ), color2 = new THREE.Color( 0xFF0000 );

// 线的材质可以由2点的颜色决定
var p1 = new THREE.Vector3( -100, 0, 100 );
var p2 = new THREE.Vector3(  100, 0, -100 );
geometry.vertices.push(p1);
geometry.vertices.push(p2);
geometry.colors.push( color1, color2 );

var line = new THREE.Line( geometry, material, THREE.LinePieces );
scene.add(line);
```

## 线条的材质 `THREE.LineBasicMaterial`

参数：

- parameters 是一个定义材质外观的对象，它包含多个属性来定义材质，这些属性是：
- color 线条的颜色，用16进制来表示，默认的颜色是白色。
- linewidth 线条的宽度，默认时候1个单位宽度。
- linecap 线条两端的外观，默认是圆角端点，当线条较粗的时候才看得出效果，如果线条很细，那么你几乎看不出效果了。
- linejoin 两个线条的连接点处的外观，默认是“round”，表示圆角。
- vertexcolors 定义线条材质是否使用顶点颜色，这是一个boolean值。意思是，线条各部分的颜色会根据顶点的颜色来进行插值。（如果关于插值不是很明白，可以qq问我，qq在前言中你一定能够找到，嘿嘿，虽然没有明确写出）

## 长方体 `Three.BoxGeometry` `CubeGeometry`

```js
var geometry = new THREE.BoxGeometry(10,10,10);
var material = new THREE.MeshBasicMaterial({color: 0x00ff00});
var cube = new THREE.Mesh(geometry, material);
scene.add(cube);
```

## 光源 `THREE.Light`

### 环境光 `THREE.AmbientLight`

### 点光源 `THREE.PointLight`

### 聚光灯 `THREE.SpotLight`