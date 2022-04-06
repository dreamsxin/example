Mover[] movers = new Mover[20];

void setup() {
  size(800,600);
  smooth();
  background(255);
  for (int i = 0; i < movers.length; i++) {
    movers[i] = new Mover(random(0.1,5), 0, 0);
  }
}

void draw() {
  background(255);
  //Make up two forces.
  PVector wind = new PVector(0.01,0);
  PVector gravity = new PVector(0,0.1);
  //Loop through all objects and apply both forces to each object.
  for (int i = 0; i < movers.length; i++) {
    float c = 0.01; //coefficient of friction
    PVector friction = movers[i].velocity.get();
    friction.mult(-1);
    friction.normalize();
    friction.mult(c);
    movers[i].applyForce(friction); //Apply the friction force vector to the object

    movers[i].applyForce(wind);
    movers[i].applyForce(gravity);
    //Calling functions on all the objects in the array
    movers[i].update();
    movers[i].checkEdges();
    movers[i].display();
  }
}
