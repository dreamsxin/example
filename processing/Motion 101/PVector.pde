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

  void limit(float max) {
    float m = mag();
    if (m > max) {
      normalize();
      mult(max);
    }
  }

}
