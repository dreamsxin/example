# p5.js

  > P5 是 Processing 语言的一个JS移植版本，使其能在Web中工作。它完全使用JavaScript来实现Processing语言相同的功能

p5.js 是个 JavaScript 创意编程程式库，其专注在让编程更易于使用及更加广泛的包容艺术家、设计师、教育家、初学者以及任何其他人！p5.js 是个免费及开源的软件因为我们相信所有人都应该能自由使用软件及用于学习软件的工具。

- https://github.com/processing/p5.js
- https://editor.p5js.org/

```js
var x = 100; //Variables for location and speed of ball.
var y = 100;
var xspeed = 1;
var yspeed = 3.3;

function setup() {
  createCanvas(400, 400);
}

function draw() {
  smooth();
  background(220);

  x = x + xspeed; //Move the ball according to its speed.
  y = y + yspeed;
  if ((x > width) || (x < 0)) { //Check for bouncing.
  xspeed = xspeed * -1;
  }
  if ((y > height) || (y < 0)) {
  yspeed = yspeed * -1;
  }
  stroke(0);
  fill(175);
  ellipse(x,y,16,16); //Display the ball at the location (x,y).
}
```

## Class

```js
class PVector {
  constructor(x_, y_) {
    this.x = x_;
    this.y = y_;
  }

  Add(v) {
    this.y = this.y + v.y;
    this.x = this.x + v.x;
  }
}

let loc, velocity;

function setup() {
  createCanvas(400, 400);
  loc = new PVector(100,100);
  velocity = new PVector(1,3.3);
}

function draw() {
  background(220);

  loc.Add(velocity);
  
  if ((loc.x > width) || (loc.x < 0)) { //Check for bouncing.
    velocity.x = velocity.x * -1;
  }
  if ((loc.y > height) || (loc.y < 0)) {
    velocity.y = velocity.y * -1;
  }
  stroke(0);
  fill(175);
  ellipse(loc.x,loc.y,16,16); //Display the ball at the location (x,y).
}
```
