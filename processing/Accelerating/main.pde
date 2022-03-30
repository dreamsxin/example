Mover mover; //Declare Mover object

void setup() {
  size(400,400);
  mover = new Mover(); //Create Mover object
}

void draw() {
  background(255);

  //Call functions on Mover object.
  mover.update();
  mover.checkEdges();
  mover.display();
}
