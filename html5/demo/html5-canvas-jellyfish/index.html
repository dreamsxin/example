<!doctype html>
<html>
<head>
<meta charset="utf-8">
<title>HTML5 Canvas 水母游动动画特效DEMO演示</title>
<script src="js/jquery.min.js"></script>
<style>
body {
  background: #333;
}

canvas {
  background: black;
}</style>
</head>
<body>
<script>
var App = {};

jQuery(document).ready(function() {
  // Setup canvas and app
  App.setup();
  // Launch animation loop
  App.frame = function() {
    App.update();
    window.requestAnimationFrame(App.frame);
  };
	App.frame();
  
  jQuery('canvas#ourCanvas').on('click', function(event) {
    App.hasUserClicked = !App.hasUserClicked;
  });
  
  jQuery('canvas#ourCanvas').on('mousemove', function(event) {
    App.target.x = event.pageX;
    App.target.y = event.pageY;
  });
});

App.setup = function() {
  // Setup canvas and get canvas context
  var canvas = document.createElement('canvas');
  canvas.height = window.innerHeight;
  canvas.width = window.innerWidth;
  canvas.id = 'ourCanvas';
  document.body.appendChild(canvas);
  this.ctx = canvas.getContext('2d');
  this.width = canvas.width;
  this.height = canvas.height;
  
  // Define a few useful elements
  this.stepCount = 0;
  this.hasUserClicked = false;
  this.xC = canvas.width / 2;
  this.yC = canvas.height / 2;
  this.target = {
    x: this.xC,
    y: this.yC,
    radius: 20
  };
  this.armsPop = 20;
  this.particlesPerArm = 15;
  
  // Create initial targets and arms
  this.arms = [];
  for (var i = 0; i < this.armsPop; i++) {
    this.arms.push([]);
  }
  // Fill initial arms
  this.initialBirth();
  
  // Some forces
  this.gravity = -1;
  this.springStiffness = 0.5;
  this.viscosity = 0.1;
  this.isElastic = false;
};
App.initialBirth = function() {
  for (var armIndex = 0; armIndex < this.arms.length; armIndex++) {
    var arm = this.arms[armIndex];
    var particlesNb = 20 + Math.ceil(20 * Math.random());
    for (var i = 0; i < particlesNb; i++) {
      var x = this.width * Math.random();
      var y = this.height * Math.random();
      var particle = {
        x: x,
        y: y,
        xLast: x,
        yLast: y,
        xSpeed: 0,
        ySpeed: 0,
        stickLength: 10,
        name: 'seed' + this.stepCount
      };

      arm.push(particle);
    }
  }
  
};
App.update = function() {
  // Evolve system
  this.evolve();
  // Move particles
  this.move();
  // Draw particles
  this.draw();
};
App.evolve = function() {
  this.stepCount++;
  this.target.radius = 50 + 30 * Math.sin(this.stepCount / 10);
};
App.move = function() {
  // This is inverse kinematics, the particles form an arm with N joints, and its shape adapts with a target contraint
  // Move target point
  if (!this.hasUserClicked) {
    this.target.x = this.xC + 150 * Math.cos(this.stepCount / 50);
    this.target.y = this.yC + 150 * Math.sin(this.stepCount / 20);
  }
  
  // Move particles accordingly on (each arm)
  for (var armIndex = 0; armIndex < this.arms.length; armIndex++) {
    var arm = this.arms[armIndex];
    var ownTargetAngle = 2 * Math.PI * armIndex / this.arms.length;
    var ownTarget = {
      x: this.target.x + this.target.radius * Math.cos(ownTargetAngle),
      y: this.target.y + this.target.radius * Math.sin(ownTargetAngle),
    }
    for (var i = 0; i < arm.length; i++) {
      var p = arm[i];
      var pLead = ( i == 0 ? ownTarget : arm[i-1] );
      var angle = segmentAngleRad(p.x, p.y, pLead.x, pLead.y, false);
      var dist = Math.sqrt(Math.pow(p.x - pLead.x, 2) + Math.pow(p.y - pLead.y, 2));
      var translationDist = dist - p.stickLength;
      if (translationDist < 0) {
        angle += Math.PI;
        translationDist = Math.abs(translationDist);
      }
      /* Kinetic binding */
      var dx = translationDist * Math.cos(angle);
      var dy = translationDist * Math.sin(angle);
      if (!this.isElastic) {
        p.x += dx;
        p.y -= dy;
      }
      /* Forces */
      var xAcc = this.springStiffness * dx - this.viscosity * p.xSpeed;
      var yAcc = this.springStiffness * dy + this.gravity - this.viscosity * p.ySpeed;
      p.xSpeed += xAcc;
      p.ySpeed += yAcc;
      p.x += 0.1 * p.xSpeed;
      p.y -= 0.1 * p.ySpeed;
    }
  }
  
}
App.draw = function() {
  // Add transparent layer for trace effect
  this.ctx.beginPath();
  this.ctx.rect(0, 0, this.width, this.height);
  this.ctx.fillStyle = 'rgba(0, 0, 0, 0.3)';
  this.ctx.fill();
  
  // Draw target
  this.ctx.beginPath();
    this.ctx.arc(this.target.x, this.target.y, 15, 0, 2 * Math.PI, false);
    this.ctx.fillStyle = 'rgba(255, 255, 255, 0.1)';
    this.ctx.fill();
  
  // Draw particles
  for (var armIndex = 0; armIndex < this.arms.length; armIndex++) {
    var arm = this.arms[armIndex];
    for (var i = 0; i < arm.length; i++) {
      var particle = arm[i];
      if (i != 0) { var particleLead = arm[i-1]; }

      // Draw particle
      this.ctx.beginPath();
      this.ctx.arc(particle.x, particle.y, 0.3 * (arm.length - i), 0, 2 * Math.PI, false);
      this.ctx.strokeStyle = 'hsla(' + (200 + i * 4) + ', 90%, 50%, 0.7)';
      this.ctx.stroke();
      // Draw its stick
      this.ctx.beginPath();
      this.ctx.lineWidth = 1;
      this.ctx.strokeStyle = 'hsla(' + (180 + i * 4) + ', 80%, 50%, 0.7)';
      if (i == 0) this.ctx.moveTo(this.target.x, this.target.y);
      else this.ctx.moveTo(particleLead.x, particleLead.y);
      this.ctx.lineTo(particle.x, particle.y);
      this.ctx.stroke();

    }
  }
};



/**
 * @param {Number} Xstart X value of the segment starting point
 * @param {Number} Ystart Y value of the segment starting point
 * @param {Number} Xtarget X value of the segment target point
 * @param {Number} Ytarget Y value of the segment target point
 * @param {Boolean} realOrWeb true if Real (Y towards top), false if Web (Y towards bottom)
 * @returns {Number} Angle between 0 and 2PI
 */
segmentAngleRad = function(Xstart, Ystart, Xtarget, Ytarget, realOrWeb) {
	var result;// Will range between 0 and 2PI
	if (Xstart == Xtarget) {
		if(Ystart == Ytarget) {
			result = 0; 
		} else if (Ystart < Ytarget) {
			result = Math.PI/2;
		} else if (Ystart > Ytarget) {
			result = 3*Math.PI/2;
		} else {}
	} else if (Xstart < Xtarget) {
		result = Math.atan((Ytarget - Ystart)/(Xtarget - Xstart));
	} else if (Xstart > Xtarget) {
		result = Math.PI + Math.atan((Ytarget - Ystart)/(Xtarget - Xstart));
	}
	
	result = (result + 2*Math.PI)%(2*Math.PI);
	
	if (!realOrWeb) {
		result = 2*Math.PI - result;
	}
	
	return result;
}</script>

<div style="text-align:center;clear:both;">
<script src="/gg_bd_ad_720x90.js" type="text/javascript"></script>
<script src="/follow.js" type="text/javascript"></script>
</div>

</body>
</html>

