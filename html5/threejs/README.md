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

环境光就是在场景中无处不在的光，它对物体的影响是均匀的，也就是无论你从物体的那个角度观察，物体的颜色都是一样的，这就是伟大的环境光。可以把环境光放在任何一个位置，它的光线是不会衰减的，是永恒的某个强度的一种光源。
```js
// 添加环境光
var ambientLight = new THREE.AmbientLight( 0x000000 );
scene.add( ambientLight );
```

### 点光源 `THREE.PointLight`

点光源是理想化为质点的向四面八方发出光线的光源。点光源是抽象化了的物理概念，为了把物理问题的研究简单化。就像平时说的光滑平面，质点，无空气阻力一样，点光源在现实中也是不存在的，指的是从一个点向周围空间均匀发光的光源。

参数：
- Color：光的颜色
- Intensity：光的强度，默认是1.0,就是说是100%强度的灯光，
- distance：光的距离，从光源所在的位置，经过distance这段距离之后，光的强度将从Intensity衰减为0。 默认情况下，这个值为0.0，表示光源强度不衰减。

```js
// 添加点光源
var lights = [];
lights[ 0 ] = new THREE.PointLight( 0xffffff, 1, 0 );
lights[ 1 ] = new THREE.PointLight( 0xffffff, 1, 0 );
lights[ 2 ] = new THREE.PointLight( 0xffffff, 1, 0 );

lights[ 0 ].position.set( 0, 200, 0 );
lights[ 1 ].position.set( 100, 200, 100 );
lights[ 2 ].position.set( - 100, - 200, - 100 );

scene.add( lights[ 0 ] );
scene.add( lights[ 1 ] );
scene.add( lights[ 2 ] );
```

### 聚光灯 `THREE.SpotLight`

参数：

- Hex：聚光灯发出的颜色，如0xFFFFFF
- Intensity：光源的强度，默认是1.0，如果为0.5，则强度是一半，意思是颜色会淡一些。和上面点光源一样。
- Distance：光线的强度，从最大值衰减到0，需要的距离。 默认为0，表示光不衰减，如果非0，则表示从光源的位置到Distance的距离，光都在线性衰减。到离光源距离Distance时，光源强度为0.
- Angle：聚光灯着色的角度，用弧度作为单位，这个角度是和光源的方向形成的角度。
- exponent：光源模型中，衰减的一个参数，越大衰减约快。

### 方向光 `THREE.DirectionalLight`

当环境光和方向光同时存在的时候，可以把这种情况想成两种光源同时作用于物体，它产生的情况，和每种光源分别作用于物体，然后将两者的结果相加，是一样的效果：

0x00FF00(绿色) + 0xFF0000(红色) = 0xFFFF00 (黄色)。

```js
light = new THREE.DirectionalLight(0xFF0000);
light.position.set(0, 0,1);
scene.add(light);
```

## 材质 `Material`

在渲染程序中，它是表面各可视属性的结合，这些可视属性是指表面的色彩、纹理、光滑度、透明度、反射率、折射率、发光度等。正是有了这些属性，才能让我们识别三维中的模型是什么做成的，也正是有了这些属性，我们计算机三维的虚拟世界才会和真实世界一样缤纷多彩。

在场景中没有任何光源的情况下，物体不能反射光源到人的眼里，所以物体应该是黑色的。

### Lambert 材质 `THREE.MeshLambertMaterial`

这是在灰暗的或不光滑的表面产生均匀散射而形成的材质类型。Lambert 材质表面会在所有方向上均匀地散射灯光。Lambert材质会受环境光的影响，呈现环境光的颜色，与材质本身颜色关系不大。

## 纹理 `THREE.Texture`

3D世界的纹理由图片组成。将纹理以一定的规则映射到几何体上，一般是三角形上，那么这个几何体就有纹理皮肤了。

那么在threejs中，或者任何3D引擎中，纹理应该怎么来实现呢？首先应该有一个纹理类，其次是有一个加载图片的方法，将这张图片和这个纹理类捆绑起来。

各个参数的意义是：

- Image：这是一个图片类型。
- Mapping：是一个THREE.UVMapping()类型，它表示的是纹理坐标。
- wrapS：表示x轴的纹理的回环方式，就是当纹理的宽度小于需要贴图的平面的宽度的时候，平面剩下的部分应该以何种方式贴图的问题。
- wrapT：表示y轴的纹理的回环方式。
- magFilter 表示过滤的方式，这是OpenGL的基本概念。
- minFilter 表示过滤的方式，这是OpenGL的基本概念。
- format：表示加载的图片的格式，这个参数可以取值 `THREE.RGBAFormat`、`THREE.RGBFormat`等。THREE.RGBAFormat表示每个像素点要使用四个分量表示，分别是红、绿、蓝、透明来表示。THREE.RGBFormat则不使用透明，也就是说纹理不会有透明的效果。
- type：表示存储纹理的内存的每一个字节的格式，是有符号，还是没有符号，是整形，还是浮点型。不过这里默认是无符号型（THREE.UnsignedByteType）。
- anisotropy：各向异性过滤。使用各向异性过滤能够使纹理的效果更好，但是会消耗更多的内存、CPU、GPU时间。

* wrapS/wrapT

默认为THREE.ClampToEdgeWrapping(夹取),超过1.0的值被固定为1.0。超过1.0的其它地方的纹理，沿用最后像素的纹理。
用于当叠加过滤时，需要从0.0到1.0精确覆盖且没有模糊边界的纹理。
THREE.RepeatWrapping(重复)
THREE.MirroredRepeatWrapping(镜像)

### 纹理坐标

在正常的情况下，你在0.0到1.0的范围内指定纹理坐标，逆时针方向：左下角是[0,0]，左上角[0,1]，右下角[1,0]

### 画一个平面 `THREE.PlaneGeometry`，并贴上一张图
```js
var geometry = new THREE.PlaneGeometry( 5, 20, 32 );
var material = new THREE.MeshBasicMaterial( {color: 0xffff00, side: THREE.DoubleSide} );
var plane = new THREE.Mesh( geometry, material );
scene.add( plane );
```

### 为平面赋予纹理坐标

```js
var geometry = new THREE.PlaneGeometry( 5, 20, 32 );
geometry.vertices[0].uv = new THREE.Vector2(0,0);
geometry.vertices[1].uv = new THREE.Vector2(1,0);
geometry.vertices[2].uv = new THREE.Vector2(1,1);
geometry.vertices[3].uv = new THREE.Vector2(0,1);
```

### 加载纹理

```js
var texture = THREE.ImageUtils.loadTexture("../three.js/examples/textures/758px-Canestra_di_frutta_(Caravaggio).jpg");
# or
var loader = new THREE.TextureLoader();
var texture = loader.load("../three.js/examples/textures/758px-Canestra_di_frutta_(Caravaggio).jpg");
# or
var texture = new THREE.Texture();
var loader = new THREE.ImageLoader();
loader.load("../three.js/examples/textures/758px-Canestra_di_frutta_(Caravaggio).jpg", function(image) {
	texture.image = image;  
	texture.needsUpdate = true;  
});
```

### 将纹理应用于材质

```js
var material = new THREE.MeshBasicMaterial({map:texture});
```

### 多面体材质，每个面一种材质

```js
var materials = [];

var texture1 = new THREE.TextureLoader().load( "../three.js/examples/textures/crate.gif" );
materials.push( new THREE.MeshBasicMaterial( { map: texture1 } ) );

var texture2 = new THREE.TextureLoader().load( "../three.js/examples/textures/hardwood2_diffuse.jpg" );
materials.push( new THREE.MeshBasicMaterial( { map: texture2 } ) );

var texture3 = new THREE.TextureLoader().load( "../three.js/examples/textures/brick_bump.jpg" );
materials.push( new THREE.MeshBasicMaterial( { map: texture3 } ) );

var material = new THREE.MultiMaterial( materials );
```