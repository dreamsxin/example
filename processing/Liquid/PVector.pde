static class PVector {
  float x;
  float y;
  
  PVector(float x_, float y_) {
    x = x_;
    y = y_;
  }

  PVector get() {
    PVector v = new PVector(x, y);
    return v;
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

  static PVector div(PVector v1, float mass) {
    PVector v3 = new PVector(v1.x/mass, v1.y/mass);
    return v3;
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
