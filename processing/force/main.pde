Mover[] movers = new Mover[1];

void setup() {
  size(600,600);
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
    movers[i].applyForce(wind);
    movers[i].applyForce(gravity);
    //Calling functions on all the objects in the array
    movers[i].update();
    movers[i].checkEdges();
    movers[i].display();
  }
}
