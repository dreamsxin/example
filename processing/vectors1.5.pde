class PVector {
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
  
  void sub(PVector v) {
    x = x - v.x;
    y = y - v.y;
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

}

void setup() {
  size(200,200);
}

void draw() {
  background(255);

  //Two PVectors, one for the mouse location and one for the center of the window
  PVector mouse = new PVector(mouseX,mouseY);
  PVector center = new PVector(width/2,height/2);

  mouse.sub(center);
  mouse.mult(0.5);

  float m = mouse.mag();
  fill(0);
  rect(0,0,m,10);
  
  translate(width/2,height/2);
  line(0,0,mouse.x,mouse.y);
}
