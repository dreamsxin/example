class Mover {
  PVector location;
  PVector velocity;
  PVector acceleration; //Acceleration is the key!
  float topspeed; //The variable topspeed will limit the magnitude of velocity.


  Mover() {
    //location = new PVector(random(width),random(height));
    location = new PVector(width/2,height/2);
    velocity = new PVector(0,0);
    acceleration = new PVector(-0.001,0.01);
    topspeed = 10;
  }

  void update() {
    PVector mouse = new PVector(mouseX,mouseY);
    PVector dir = PVector.sub(mouse,location); //Step 1: Compute direction
    dir.normalize(); //Step 2: Normalize
    dir.mult(0.5); //Step 3: Scale
    acceleration = dir; //Step 4: Accelerate
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
