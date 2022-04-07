Mover[] movers = new Mover[2];
Attractor disA;
void setup(){
  size(600, 600);
  for(int i = 0; i < movers.length; i++){
    movers[i] = new Mover(random(0.1,1), random(width/4),  random(height/4));
  }
  disA = new Attractor(20, width/2, height/2);
}
void draw(){
  background(255);
  disA.update();
  disA.display();
  for(int i = 0; i < movers.length; i++){
    for(int j = 0; j < movers.length; j++){
      if(i != j){
        PVector force = movers[j].disAttract(movers[i]);
        movers[i].applyForce(force);      
      }
    }
    PVector abs = disA.attract(movers[i]);
    movers[i].applyForce(abs);
    movers[i].update();
    movers[i].display();
  }
}
