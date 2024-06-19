#
Planck.js is JavaScript rewrite of the famous Box2D physics engine. The following post is about, converting and connecting Plank.js world with Pixi.js world.

## 0.Defining Plank world
```typescript
plank_world = PLANK.World(
{
gravity:PLANK.Vec2(0, -10)
}
);
```

## 1.Understanding the scale（比例）
Plank uses SI units (meters, seconds, kilograms), to make things visually realistic, pixel scales in Pixi must have a relationship with the real-world scale. In the example, I ‘ve used a football. In a 576*1024 pixel renderer view, I wanted my football to be 128*128 pixel image. So I have defined the view.
Now a football is approximately 0.20m in diameter.
128 pixels = 0.20 m

## 2.Understanding coordinate systems（坐标系统）
To makes things easier I ‘ve placed the origin of the Plank world on the bottom left corner of the view.

## 3.Pixi to Plank and Plank to Pixi
According to the above definitions,
```typescript
function pixiPositionToPlank(x, y) {

//change Y origin point and direction
y=(y-1024)*-1;

//convert pixels to meters (64px = 0.1m)
y*=0.0015625;
x*=0.0015625;

returnPLANK.Vec2(x,y);
}
function plankPositionToPixi(v) {

//convert pixels to meters (64px = 0.1m)
var retY=v.y*640;
var retX=v.x*640;

//change Y origin point and direction
retY= (retY*-1)+1024;

return { x:retX, y:retY };
}
```
