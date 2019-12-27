// The color lerp function is from Ross Zurowski v
// https://gist.github.com/rosszurowski/67f04465c424a9bc0dae

function lerpColor(a, b, amount) {
  var ah = parseInt(a.replace(/#/g, ""), 16),
    ar = ah >> 16,
    ag = (ah >> 8) & 0xff,
    ab = ah & 0xff,
    bh = parseInt(b.replace(/#/g, ""), 16),
    br = bh >> 16,
    bg = (bh >> 8) & 0xff,
    bb = bh & 0xff,
    rr = ar + amount * (br - ar),
    rg = ag + amount * (bg - ag),
    rb = ab + amount * (bb - ab);

  return (
    "#" + (((1 << 24) + (rr << 16) + (rg << 8) + rb) | 0).toString(16).slice(1)
  );
}

const PI = Math.PI,
  TWO_PI = Math.PI * 2;

const Util = {};
Util.timeStamp = function() {
  return window.performance.now();
};
Util.random = function(min, max) {
  return min + Math.random() * (max - min);
};
Util.map = function(a, b, c, d, e) {
  return (a - b) / (c - b) * (e - d) + d;
};
Util.lerp = function(value1, value2, amount) {
  return value1 + (value2 - value1) * amount;
};
Util.clamp = function(value, min, max) {
  return Math.max(min, Math.min(max, value));
};

const Tween = {};
Tween.linear = function(currentTime, start, degreeOfChange, duration) {
  return degreeOfChange * currentTime / duration + start;
};
Tween.easeInOutQuad = function(t, b, c, d) {
  t /= d / 2;
  if (t < 1) return c / 2 * t * t + b;
  t--;
  return -c / 2 * (t * (t - 2) - 1) + b;
};
Tween.easeInOutExpo = function(t, b, c, d) {
  t /= d / 2;
  if (t < 1) return c / 2 * Math.pow(2, 10 * (t - 1)) + b;
  t--;
  return c / 2 * (-Math.pow(2, -10 * t) + 2) + b;
};
class v3 {
  constructor(x, y, z) {
    this.x = x;
    this.y = y;
    this.z = z;
  }
}

class Vector {
  constructor(x, y) {
    this.x = x || 0;
    this.y = y || 0;
  }
  set(x, y) {
    this.x = x;
    this.y = y;
  }
  reset() {
    this.x = 0;
    this.y = 0;
  }
  fromAngle(angle) {
    let x = Math.cos(angle),
      y = Math.sin(angle);
    return new Vector(x, y);
  }
  add(vector) {
    this.x += vector.x;
    this.y += vector.y;
  }
  sub(vector) {
    this.x -= vector.x;
    this.y -= vector.y;
  }
  mult(scalar) {
    this.x *= scalar;
    this.y *= scalar;
  }
  div(scalar) {
    this.x /= scalar;
    this.y /= scalar;
  }
  dot(vector) {
    return vector.x * this.x + vector.y * this.y;
  }
  limit(limit_value) {
    if (this.mag() > limit_value) this.setMag(limit_value);
  }
  mag() {
    return Math.hypot(this.x, this.y);
  }
  setMag(new_mag) {
    if (this.mag() > 0) {
      this.normalize();
    } else {
      this.x = 1;
      this.y = 0;
    }
    this.mult(new_mag);
  }
  normalize() {
    let mag = this.mag();
    if (mag > 0) {
      this.x /= mag;
      this.y /= mag;
    }
  }
  heading() {
    return Math.atan2(this.y, this.x);
  }
  setHeading(angle) {
    let mag = this.mag();
    this.x = Math.cos(angle) * mag;
    this.y = Math.sin(angle) * mag;
  }
  dist(vector) {
    return new Vector(this.x - vector.x, this.y - vector.y).mag();
  }
  angle(vector) {
    return Math.atan2(vector.y - this.y, vector.x - this.x);
  }
  copy() {
    return new Vector(this.x, this.y);
  }
}

// init canvas
let canvas = document.createElement("canvas"),
  ctx = canvas.getContext("2d"),
  W,
  H;

document.body.appendChild(canvas);
setSize();

document.body.onresize = function() {
  let old_width = W;
  setSize();
  populate();
};

function setSize() {
  W = canvas.width = window.innerWidth;
  H = canvas.height = window.innerHeight;
  ctx.lineCap = "round";
  ctx.lineJoin = "round";
}

let mouse = new Vector(W/2,H/2);
canvas.onmousemove = function(event) {
  mouse.x = event.clientX - canvas.offsetLeft;
  mouse.y = event.clientY - canvas.offsetTop;
};


//
class Point {
  constructor(x, y) {
    this.position = new Vector(x, y);
    this.old_position = this.position.copy();
    this.pinned = false;
    this.angle = undefined;
  }
  render() {
    ctx.beginPath();
    ctx.arc(this.position.x, this.position.y, 4, 0, 2 * Math.PI);
    ctx.stroke();
  }
}

class Leaf {
  constructor(parent, length) {
    this.parent = parent;
    this.end = new Point(parent.position.x, parent.position.y + (length * Util.random(0.9,1.1) ));
    this.end.angle = 0;
    this.z_value = 0;
    this.colors = ["#2c4c28", "#48792c", "#69982d"];
    this.true_color = this.colors[
      Math.floor(Math.random() * this.colors.length)
    ];
    this.size = (length * 2) * Util.random(0.8, 1.2);
    this.thickness = this.size * 0.5 * Util.random(0.6, 1.2);
  }
  setZ(z_value) {
    this.z_value = z_value;
    this.color = lerpColor(this.true_color, backgroundColor, this.z_value);
  }
  render() {
    ctx.strokeStyle = this.color;
    //ctx.lineWidth = this.thickness;
    ctx.beginPath();
    ctx.moveTo(this.parent.position.x, this.parent.position.y);
    ctx.lineTo(this.end.position.x, this.end.position.y);
    ctx.closePath();
    ctx.stroke();
    /*
    ctx.save();
    ctx.translate(this.parent.position.x, this.parent.position.y);
    ctx.rotate(angle + Math.PI / 2);
    ctx.beginPath();
    ctx.ellipse(0, -this.size, this.thickness, this.size, 0, 0, 2 * Math.PI);
    ctx.fill();
    ctx.restore();
  */
  }
}

class Link {
  constructor(p0, p1) {
    this.p0 = p0;
    this.p1 = p1;
    this.length = p0.position.dist(p1.position);
  }
}

class Plant {
  constructor(x, y, l) {
    this.position = new Vector(x, y);
    this.points = [];
    this.strand = [];
    this.leaves = [];
    this.links = [];
    this.length = l;
    this.resolution =
      Math.floor(Util.map(this.length, 0, H, 3, 8)) +
      Math.round(Util.random(0, 2));

    this.div = this.length / this.resolution;
    this.min_thickness = 10;
    this.max_thickness = 32;
    this.thickness =
      Util.map(this.length, H, 0, this.min_thickness, this.max_thickness) *
      Util.random(0.8, 1.2);
    this.z_value = 0;
    this.colors = ["#4a2525", "#562a3b"];
    this.true_color = this.colors[
      Math.floor(Math.random() * this.colors.length)
    ];
  }
  setZ(z_value) {
    this.z_value = z_value;
    this.color = lerpColor(this.true_color, backgroundColor, this.z_value);
  }
  init() {
    for (let i = 0; i < this.resolution; i++) {
      let x = this.position.x,
        y = this.position.y + (0, i * this.div);
      this.strand.push(new Point(x, y));
    }
    this.strand[0].pinned = true;
    for (let i = 0; i < this.resolution - 1; i++) {
      this.links.push(new Link(this.strand[i], this.strand[i + 1]));
    }

    for (let i = 1; i < this.resolution; i++) {
      let leaf_size = Util.map(
        this.thickness,
        this.min_thickness,
        this.max_thickness,
        10,
        60
      );
      leaf_size *= Util.map(i, 0, this.resolution, 1.2, 0.8);
      let leaf = new Leaf(this.strand[i], leaf_size);
      leaf.end.angle = Math.PI + Util.random(-0.2, 0.2);
      leaf.setZ(this.z_value);
      this.leaves.push(leaf);
      this.links.push(new Link(leaf.parent, leaf.end));

      leaf = new Leaf(this.strand[i], leaf_size);
      leaf.end.angle = 0 + Util.random(-0.2, 0.2);
      leaf.setZ(this.z_value);
      this.leaves.push(leaf);
      this.links.push(new Link(leaf.parent, leaf.end));
    }

    this.leaves.forEach(l => {
      this.points.push(l.end);
    });
    this.points = this.points.concat(this.strand);
  }
  translate(x, y) {
    this.position.set(x, y);
    this.strand[0].position.set(x, y);
  }
  render() {
    ctx.strokeStyle = this.color;
    ctx.lineWidth = this.thickness;
    ctx.beginPath();
    ctx.moveTo(this.strand[0].position.x, this.strand[0].position.y);
    for (let i = 2; i < this.strand.length; i++) {
      ctx.lineTo(this.strand[i - 1].position.x, this.strand[i - 1].position.y);
      ctx.lineTo(this.strand[i].position.x, this.strand[i].position.y);
    }
    ctx.stroke();
    this.leaves.forEach(l => {
      l.render();
    });
  }

  update() {
    for (let i = 0; i < this.points.length; i++) {
      let point = this.points[i];

      if (point.pinned) continue;
      let velocity = point.position.copy();
      velocity.sub(point.old_position);
      velocity.mult(0.98);
      point.old_position = point.position.copy();
      point.position.add(velocity);
      if (point.angle != undefined) {
        point.position.x += Math.cos(point.angle) * 1;
        point.position.y += Math.sin(point.angle) * 1;
      }
      let internal_wind = wind.value.copy();
      internal_wind.mult(this.z_value + 0.1);
      point.position.add(internal_wind);
      point.position.add(gravity);
      
      
      let distance = point.position.dist(mouse);

      if (distance < 200) {
        let force = new Vector();

        let angle = point.position.angle(mouse);
        force.setMag(Util.map(distance, 0, 200, 2, 0) * (-(this.z_value)+1));
        force.setHeading(angle);
        point.position.sub(force);
      }

      
      if (point.position.y > H) {
        point.position.y = H;
        point.position.y = point.position.y + velocity.y * 0.2;
      }
    }
    for (let i = 0; i < 4; i++) {
      this.links.forEach(link => {
        let distance = link.p0.position.dist(link.p1.position),
          difference = link.length - distance,
          percent = difference / distance / 2;
        let offset = new Vector(
          (link.p1.position.x - link.p0.position.x) * percent,
          (link.p1.position.y - link.p0.position.y) * percent
        );
        if (!link.p0.pinned && link.p1.angle == undefined) {
          link.p0.position.sub(offset);
        }
        if (!link.p1.pinned && link.p0.angle == undefined) {
          link.p1.position.add(offset);
        }
      });
    }
  }
}

let plants = [];
let gravity = new Vector(0, 0.6);
let wind = {
  value: new Vector(0, 0),
  reset: function() {
    this.time_start = new Date();
    this.start = this.value.x;
    this.duration = Util.random(200, 1000);
    this.goal = Util.random(-0.06, 0.06);
  },
  update: function() {
    let time = new Date() - this.time_start;
    if (time < this.duration) {
      this.value.x = Tween.linear(
        time,
        this.start,
        this.goal - this.start,
        this.duration
      );
    } else {
      setTimeout(() => {
        this.reset();
      }, Util.random(100, 3000));
    }
  }
};
wind.reset();

let backgroundColor = "#1d0d21";

function populate() {
  plants = [];
  let div_x = 4,
    div_y = 5;
  let total_div = div_x * div_y;

  let part_x = W / div_x;

  for (let i = 0; i < total_div; i++) {
    //offset_x = (i % div_x) * part_x,
    let offset_x = Util.random(0,W),
      offset_y = Math.floor(i / div_x),
      z_value = Util.map(offset_y, 0, div_y - 1, 0.8, 0),
      p_length = Util.map(offset_y, 0, div_y - 1, H * 0.4, H);
    
    let z_random = Util.random(0.8, 1.2);
    z_value *= z_random;
    z_value = Util.clamp(z_value, 0, 1);
    let plant = new Plant(offset_x, 0, p_length);
    plant.setZ(z_value);
    plant.init();
    plants.push(plant);
  }
}

populate();

function draw() {}

function loop(f) {
  ctx.fillStyle = backgroundColor;
  ctx.fillRect(0, 0, W, H);
  wind.update();
  plants.forEach(p => {
    p.update();
    p.render();
  });
  requestAnimationFrame(loop);
}

loop();