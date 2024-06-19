#
Planck.js is JavaScript rewrite of the famous Box2D physics engine. The following post is about, converting and connecting Plank.js world with Pixi.js world.

https://github.com/kalanadis/helloPlankByPixi/tree/master

## 0.Defining Plank world
```typescript
        //Init Physics world
        plank_world = PLANK.World(
            {
                gravity: PLANK.Vec2(0, -10)
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

坐标转换
```typescript
function pixiPositionToPlank(x, y) {
    /*
    Orientation Change
    ==================
    Pixi cordinate system      +
    -------------->+           ^
    |                          |
    |                          |
    |                  ==>     |
    |                          |
    v                           ------------->+
    +                           Plank cordinate system 
    */

    //change Y origin point and direction
    y=(y-1024)*-1;
    //convert pixels to meters (64px = 0.1m)
    y *= 0.0015625;
    x *= 0.0015625;
    
    return PLANK.Vec2(x,y);
}

function plankPositionToPixi(v) {


     /*
    Orientation Change
    ==================
    Pixi cordinate system      +
    -------------->+           ^
    |                          |
    |                          |
    |                  <==     |
    |                          |
    v                           ------------->+
    +                           Plank cordinate system 
    */

    
    //convert pixels to meters (64px = 0.1m)
    var retY = v.y*640;
    var retX = v.x*640;
    //change Y origin point and direction
    retY = (retY*-1)+1024;
    return { x: retX, y: retY };
}
```

## 4.Updating the Physics world
In the Game loop,
```typescript
function game_update(tFrame) {

    tDelta = tFrame - tLastFrame;
    request_Anim_ID = requestAnimationFrame(game_update);

    if (isLoaded) {
        //Update Physics
        plank_world.step(tDelta * 0.001);
        var po = plankPositionToPixi(ball_body.getPosition());
        ball_sprite.position.set(po.x,po.y);
        ball_sprite.rotation = ball_body.getAngle()*-1;
    }

    tLastFrame = tFrame;
    renderer.render(stage);
}
```
