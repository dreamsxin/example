class Mover {
  float mass; //The object now has mass!
  PVector location;
  PVector velocity;
  PVector acceleration; //Acceleration is the key!

  Mover(float m, float x, float y) {
    mass = m; //Now setting these variables with arguments
    location = new PVector(x, y);
    velocity = new PVector(0,0);
    acceleration = new PVector(0,0);
  }
  
  
  void applyForce(PVector force) {
    //Newton’s second law
    PVector f = PVector.div(force, mass);//Receive a force, divide by mass, and add to acceleration.
    acceleration.add(f);
  }

  void update() {
    velocity.add(acceleration); //Motion 101 from Chapter 1
    location.add(velocity);
    //Now add clearing the acceleration each time!
    acceleration.mult(0);
  }

  void display() {
    stroke(0);
    fill(175);
    ellipse(location.x,location.y,mass*16,mass*16); //Scaling the size according to mass.
  }
  
  void checkEdges() {
    //When it reaches one edge, set location to the other.
    if (location.x > width) {
      location.x = width;
      velocity.x *= -1;
    } else if (location.x < 0) {
      //location.x = 0;
      //velocity.x *= -1;
    }
    if (location.y > height) {
      location.y = height;
      velocity.y *= -1;
    } else if (location.y < 0) {
      //location.y = 0;
      //velocity.y *= -1;
    }
  }
}
