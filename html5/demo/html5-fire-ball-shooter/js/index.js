bgColor = '#111';
gravity = 0.03;
particleColor = '#f73';

canvas = document.querySelector('canvas');
ctx = canvas.getContext('2d');

(onresize = function(){
  width = canvas.width = canvas.clientWidth;
  height = canvas.height = canvas.clientHeight;
  o = {x:Math.floor(width/2),y:Math.floor(height/2)};
  edge = {top:-o.y,right:width-o.x,bottom:height-o.y,left:-o.x}
})();

particles = {};
newParticle = (function(){
  var nextIndex = 0;
  return function(x,y,r,o,c,xv,yv,rv,ov){
    particles[++nextIndex] = {
      index: nextIndex,
      x: x,
      y: y,
      r: r,
      o: o,
      c: c,
      xv: xv,
      yv: yv,
      rv: rv,
      ov: ov
    };
  };
})();

fireballs = {};
newFireball = (function(){
  var nextIndex = 0;
  return function(x,y,xv,yv,life){
    fireballs[++nextIndex] = {
      index: nextIndex,
      x: x,
      y: y,
      xv: xv,
      yv: yv,
      life: life
    };
  };
})();

mouse = {x:0,y:0,d:0};
onmousemove = function(e){
  mouse.x = e.clientX-o.x;
  mouse.y = e.clientY-o.y;
  var dx = mouse.x - pos1.x,
      dy = mouse.y - pos1.y;
  mouse.d = Math.sqrt(dx*dx+dy*dy);
};

charging = false;
pos1 = {x:0,y:0};
showInstructions = true;
onmousedown = function(e){
  pos1.x = mouse.x;
  pos1.y = mouse.y;
  charging = true;
  showInstructions = false;
};

onmouseup = function(){
  if(charging){
    newFireball(
      mouse.x,
      mouse.y,
      (pos1.x-mouse.x)*0.03,
      (pos1.y-mouse.y)*0.03,
      600
    );
    charging = false;
  }
};

time = 0;
requestAnimationFrame(loop = function(){
  ctx.setTransform(1,0,0,1,0,0);
  ctx.globalCompositeOperation = 'source-over';
  ctx.globalAlpha = 1;
  ctx.fillStyle = bgColor;
  ctx.fillRect(0,0,width,height);
  
  ctx.translate(o.x,o.y);
  
  if(charging){
    var c = Math.floor(30+mouse.d/2);
    ctx.strokeStyle = 'rgba('+c+','+c+','+c+',1)';
    ctx.lineWidth = 4;
    ctx.beginPath();
    ctx.moveTo(pos1.x,pos1.y);
    ctx.lineTo(mouse.x,mouse.y);
    ctx.lineCap = 'round';
    ctx.stroke();
  }
  
  if(showInstructions){
    pos1.x = -70;
    pos1.y = -35;
    
    if(time<10){
      var x = -70,
          y = -35,
          r = 30-time*2,
          a = time/10;
    }else if(time<80){
      var x = (time-10)*2-70,
          y = (time-10)-35,
          r = 10,
          a = 1;
    }else if(time<90){
      var x = 70,
          y = 35,
          r = 10+(time-80)*2,
          a = 1-(time-80)/10;
    }else if(time<140){
      var x = 70,
          y = 35,
          r = 30,
          a = 0;
    }
    var dx = pos1.x-x,
        dy = pos1.y-y,
        d = Math.sqrt(dx*dx+dy*dy);
    if(time<80&&time>10){
      ctx.globalCompositeOperation = 'source-over';
      ctx.globalAlpha = 1;
      var c = Math.floor(30+d/2);
      ctx.strokeStyle = 'rgba('+c+','+c+','+c+',1)';
      ctx.lineWidth = 4;
      ctx.beginPath();
      ctx.moveTo(pos1.x,pos1.y);
      ctx.lineTo(x,y);
      ctx.lineCap = 'round';
      ctx.stroke();
    }
    if(time<140){
      ctx.globalCompositeOperation = 'source-over';
      ctx.globalAlpha = a;
      ctx.beginPath();
      ctx.arc(x,y,r,0,Math.PI*2);
      ctx.lineWidth = 2;
      ctx.strokeStyle = '#aaa';
      ctx.stroke();
    }
    if(time==80){
      newFireball(
        x,
        y,
        dx*0.03,
        dy*0.03,
        240
      );
    }
    time = (time+1)%180;
  }
  
  ctx.globalCompositeOperation = 'lighter';
  for(var i in particles){
    var p = particles[i];
    ctx.beginPath();
    ctx.arc(p.x,p.y,p.r,0,Math.PI*2);
    ctx.globalAlpha = p.o;
    ctx.fillStyle = p.c;
    ctx.fill();
  }
  
  for(var i in particles){
    var p = particles[i];
    p.x += p.xv;
    p.y += p.yv;
    p.r += p.rv;
    p.o += p.ov;
    if(p.r<0)delete particles[p.index];
    if(p.o<0)delete particles[p.index];
  }
  
  for(var i in fireballs){
    f = fireballs[i];
    var numParticles = Math.sqrt(f.xv*f.xv+f.yv*f.yv)/5;
    if(numParticles<1)numParticles=1;
    var numParticlesInt = Math.ceil(numParticles),
        numParticlesDif = numParticles/numParticlesInt;
    for(var j=0;j<numParticlesInt;j++){
      newParticle(
        f.x-f.xv*j/numParticlesInt,
        f.y-f.yv*j/numParticlesInt,
        7,
        numParticlesDif,
        particleColor,
        Math.random()*0.6-0.3,
        Math.random()*0.6-0.3,
        -0.3,
        -0.05*numParticlesDif
      );
    }
    f.x += f.xv;
    f.y += f.yv;
    f.yv += gravity;
    var boundary;
    if(f.y<(boundary = edge.top+7)){
      f.y = boundary;
      f.yv *= -1;
    }else if(f.y>(boundary = edge.bottom-7)){
      f.y = boundary;
      f.yv *= -1;
    }
    if(f.x>(boundary = edge.right-7)){
      f.x = boundary;
      f.xv *= -1;
    }else if(f.x<(boundary = edge.left+7)){
      f.x = boundary;
      f.xv *= -1;
    }
    if(--f.life<0)delete fireballs[f.index];
  }
  
  requestAnimationFrame(loop);
});