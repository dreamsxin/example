static class PVector {
  float x;
  float y;
  
  PVector(float x_, float y_) {
    x = x_;
    y = y_;
  }

  void add(PVector v) {
    y = this.y + v.y;
    x = this.x + v.x;
  }

  static PVector add(PVector v1, PVector v2) {
    PVector v3 = new PVector(v1.x + v2.x, v1.y + v2.y);
    return v3;
  }

  void sub(PVector v) {
    x = x - v.x;
    y = y - v.y;
  }

  static PVector sub(PVector v1, PVector v2) {
    PVector v3 = new PVector(v1.x - v2.x, v1.y - v2.y);
    return v3;
  }

  void mult(float n) {
    //With multiplication, the components of the vector are multiplied by a number.
    x = x * n;
    y = y * n;
  }
  
  void div(float n) {
    x = x / n;
    y = y / n;
  }
  
  float mag() {
    return sqrt(x*x + y*y);
  }

  void normalize() {
    float m = mag();
    if (m != 0) {
      div(m);
    } else {
      if (x == 0) {
        y = 1;
      } else {
        x = 1;
      }
    }
  }

  void limit(float max) {
    float m = mag();
    if (m > max) {
      normalize();
      mult(max);
    }
  }

}

class Mover {
  PVector location;
  PVector velocity;
  PVector acceleration; //Acceleration is the key!
  float topspeed; //The variable topspeed will limit the magnitude of velocity.


  Mover() {
    //location = new PVector(random(width),random(height));
    location = new PVector(random(width),random(height));
    velocity = new PVector(0,0);
    topspeed = 10;
  }

  void update() {
    //Our algorithm for calculating acceleration:
    PVector mouse = new PVector(mouseX,mouseY);
    PVector dir = PVector.sub(mouse,location); //Find the vector pointing towards the mouse.
    
    dir.normalize(); //Normalize
    dir.mult(0.5); //Scale.

    acceleration = dir; //Set to acceleration.
    velocity.add(acceleration);
    velocity.limit(topspeed);
    location.add(velocity);
  }

  void display() {
    stroke(0);
    fill(175);
    ellipse(location.x,location.y,16,16); //The Mover is displayed.
  }
  
  void checkEdges() {
    //When it reaches one edge, set location to the other.
    if (location.x > width) {
      location.x = 0;
    } else if (location.x < 0) {
      location.x = width;
    }
    if (location.y > height) {
      location.y = 0;
    } else if (location.y < 0) {
      location.y = height;
    }
  }
}

Mover[] movers = new Mover[20];

void setup() {
  size(400,400);
  smooth();
  background(255);
  for (int i = 0; i < movers.length; i++) {
    movers[i] = new Mover(); //Initialize each object in the array.
  }
}

void draw() {
  background(255);
  for (int i = 0; i < movers.length; i++) {
    //Calling functions on all the objects in the array
    movers[i].update();
    movers[i].checkEdges();
    movers[i].display();
  }
}
