class PVector {
  float x;
  float y;
  PVector(float x_, float y_) {
    this.x = x_;
    this.y = y_;
  }

  void Add(PVector v) {
    this.y = this.y + v.y;
    this.x = this.x + v.x;
  }
}

//Instead of a bunch of floats, we now just have two PVector variables.
PVector location;
PVector velocity;

void setup() {
  size(400, 400);
  smooth();
  background(255);
  location = new PVector(100,100);
  velocity = new PVector(1,3.3);
}

void draw() {
  background(255);

  location.Add(velocity);
  
  if ((location.x > width) || (location.x < 0)) { //Check for bouncing.
    velocity.x = velocity.x * -1;
  }
  if ((location.y > height) || (location.y < 0)) {
    velocity.y = velocity.y * -1;
  }
  stroke(0);
  fill(175);
  ellipse(location.x,location.y,16,16); //Display the ball at the location (x,y).
}
