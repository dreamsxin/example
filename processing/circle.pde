float radius1 = 150;
float radius2 = 75;

void setup(){
  size(500, 500);
  noStroke();
}

void draw(){
  fill(0, 100);
  rect(0, 0, width, height);
  fill(255);
  translate(width / 2, height / 2);
  for(float ang1 = 0; ang1 < 360; ang1 += 5){
    float radian1 = radians(ang1);
    pushMatrix();
    translate(radius1 * cos(radian1), radius1 * sin(radian1));
    for(float ang2 = 0; ang2 < 90; ang2 += 10){
      float radian2 = radians(ang2) - frameCount * 0.02 + radian1;
      ellipse(radius2 * cos(radian2), radius2 * sin(radian2), 2, 2);
    }
    popMatrix();
    //saveFrame("line-######.png"); //每一帧保存成png
  }
}
