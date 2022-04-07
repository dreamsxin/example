class Mover {
  PVector location;
  PVector velocity;
  PVector acceleration;
  float mass;
  float G;
  Mover(float m_, float x_, float y_){
    location = new PVector(x_, y_);
    velocity = new PVector(0, 0);
    acceleration = new PVector(0, 0);
    mass = m_;
    G = 1;
  }
  Mover(){
    location = new PVector(random(width), random(height));
    velocity = new PVector(0, 0);
    acceleration = new PVector(0, 0);
    mass = 1;
    G = 1;
  }

  void applyForce(PVector force){
    PVector f = PVector.div(force, mass);
    acceleration.add(f);
  }

  void update(){
    velocity.add(acceleration);
    location.add(velocity);
    
    acceleration.mult(0);
  }
  void display(){
    stroke(0);
    fill(175);
    ellipse(location.x, location.y, mass*16, mass*16);
  }

  void checkEdges(){
    if(location.x > width){
      location.x = width;
      velocity.x *= -1;
    }else if(location.x < 0){
      location.x = 0;
      velocity.x *= -1;
    }
    
    if(location.y > height){
      location.y = height;
      velocity.y *= -1;
    }else if(location.y < 0){
      location.y = 0;
      velocity.y *= -1;
    }
  }

  boolean isInside(Liquid l){
    if(location.x > l.x && location.x < l.x+l.w && location.y > l.y && location.y < l.y+l.h){
      return true;
    }else{
      return false;
    }
  }

  void drag(Liquid l){
    float speed = velocity.mag();
    float dragMagnitude = 0.5*l.c * speed *speed;
    
    PVector drag = velocity.get();
    drag.mult(-1);
    drag.normalize();
    
    drag.mult(dragMagnitude);
    applyForce(drag);
  }

  PVector disAttract(Mover m){
    PVector force = PVector.sub(location, m.location);
    float distance = force.mag();
    distance = constrain(distance, 5, 10);
    force.mult(-1);
    force.normalize();
    
    float strength = (G * mass * m.mass)/(distance * distance);
    force.mult(strength);
    return force;
  }

  PVector attract(Mover m){
    PVector force = PVector.sub(location, m.location);
    float distance = force.mag();
    distance = constrain(distance, 5, 10);
    force.normalize();
    
    float strength = (G * mass * mass)/(distance * distance);
    force.mult(strength);
    return force;
  }
}
