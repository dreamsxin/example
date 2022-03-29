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

}

void setup() {
  size(200,200);
}

void draw() {
  background(255);

  PVector mouse = new PVector(mouseX,mouseY);
  PVector center = new PVector(width/2,height/2);

  mouse.sub(center);

  //In this example, after the vector is normalized, it is multiplied by 50 so that it is viewable onscreen. Note that no matter where the mouse is, the vector will have the same length (50) due to the normalization process.
  mouse.normalize();
  mouse.mult(50);

  translate(width/2,height/2);
  line(0,0,mouse.x,mouse.y);
}
