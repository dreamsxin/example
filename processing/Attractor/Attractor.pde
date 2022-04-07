class Attractor{
  float mass;
  PVector location;
  float G;
  Attractor(){
    location = new PVector(width/2, height/2);
    mass = 20;
    G = 1;
  }
  Attractor(float m_, float x_, float y_){
    location = new PVector(x_, y_);
    mass = m_;
    G = 1;
  }
  void update(){
    location.x = mouseX;
    location.y = mouseY;
  }
  void display(){
    stroke(0);
    fill(175, 0);
    ellipse(location.x, location.y, mass*2, mass*2);
  }
  PVector attract(Mover m){
    PVector force = PVector.sub(location, m.location);
    float distance = force.mag();
    distance = constrain(distance, 5, 25);
    force.normalize();
    float strength = (G*mass*m.mass)/(distance*distance);
    force.mult(strength);
    return force;
  }
  PVector disAttract(Mover m){
    PVector force = PVector.sub(location, m.location);
    float distance = force.mag();
    distance = constrain(distance, 5, 5);
    force.normalize();
    force.mult(-1);
    float strength = (G*mass*m.mass)/(distance*distance);
    force.mult(strength);
    return force;
  }
}
