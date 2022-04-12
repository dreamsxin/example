boolean run;

void mousePressed() {
  if (run) {
    run = false;
  } else {
    run = true;
  }
}

float angle = 0; //Location
float aVelocity = 0; //Velocity
float aAcceleration = 0.001; //Acceleration

void setup() {
  size(200,200);
}

void draw() {
  if (!run) {
    return;
  }
  background(255);
  fill(175);
  stroke(0);
  rectMode(CENTER);
  translate(width/2,height/2);
  rotate(angle);
  line(-50,0,50,0);
  ellipse(50,0,8,8);
  ellipse(-50,0,8,8);
  //Angular equivalent of velocity.add(acceleration);
  aVelocity += aAcceleration;
  angle += aVelocity; //Angular equivalent of location.add(velocity);
}
