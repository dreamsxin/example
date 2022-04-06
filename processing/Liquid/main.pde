Mover[] movers = new Mover[5];
Liquid liquid;
boolean run;
void setup(){
  size(360, 640);
  smooth();
  for(int i = 0; i < movers.length; i++){
    movers[i] = new Mover(random(0.1, 5), width/2, 0);
  }
  liquid = new Liquid(0, height/2, width, height/2, 0.1); 
}

void mousePressed() {
  if (run) {
    run = false;
  } else {
    run = true;
  }
}

void draw(){
  if (run) {
    background(255);
    liquid.display();
    
    for(int i = 0; i < movers.length; i++){
      if(movers[i].isInside(liquid)){
        movers[i].drag(liquid);
      }
      
      float m = 0.1*movers[i].mass;
      PVector gravity = new PVector(0, m);
      
      movers[i].applyForce(gravity);
      movers[i].update();
      movers[i].display();
      movers[i].checkEdges();
    }
  }
}
