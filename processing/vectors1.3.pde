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
}

void setup() {
  size(200,200);
}

void draw() {
  background(255);

  //Two PVectors, one for the mouse location and one for the center of the window
  PVector mouse = new PVector(mouseX,mouseY);
  PVector center = new PVector(width/2,height/2);

  //line(center.x,center.y,mouse.x,mouse.y);
  
  mouse.sub(center); //PVector subtraction!
  translate(width/2,height/2); //转换坐标原点

  line(0,0,mouse.x,mouse.y); //Draw a line to represent the vector.
}
