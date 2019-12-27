/**
 * @author Ryan Liu
 * www.ryan-liu.com
 */
 
 var canvas
 var context
 var img
 var imgArr = new Array();
 var ox = 350
 var oy = 50
 var w = 100
 var h = 100;
 var th = 1;
 var bottom;
 var posData = new Array();
 var oldPos = new Array();
 var currentData = new Array();
 var targetData = posData

 var reversed = false;
 
 for(var i=0;i<h;i++){
	 posData[i] = new Array();
	 oldPos[i] = new Array();
	 currentData[i] = new Array();
	 for(var j=0;j<w;j++){
		 posData[i][j] = {x:ox+j,y:oy+i};
		 oldPos[i][j] = {x:Math.random()*800,y:Math.random()*600};
		  currentData[i][j] = {x:Math.random()*800,y:Math.random()*600,vx:0,vy:0,ax:.16-Math.random()*.08,ay:.16-Math.random()*.08,nx:.4+Math.random()*.3,ny:.3+Math.random()*.2};
		 
	 }
 }
 
 window.onload = init;
 window.onclick = onClick;
function init(){
	canvas = $('canvas');
	context = canvas.getContext('2d');
	bottom = canvas.height - 1;
	img = new Image();
	img.src = "ryan.gif"
	img.onload = function () {
		//
		var interval = setInterval(run,10);
		context.drawImage(img,0,0);
		for(var i=0;i<h;i++){
			imgArr[i] = [];
			for(var j=0;j<w;j++){
				imgArr[i][j] = context.getImageData(j,i,1,1);
			}
		}
		context.clearRect(0,0,canvas.width,canvas.height);
	}
 }
 
 function run(){
	 context.clearRect(0,0,canvas.width,canvas.height);
	 if(!reversed){
		 if(th<h) th++;
	 	for(var i=0;i<h;i++){
			for(var j=0;j<w;j++){
				var target = targetData[i][j];
				var current = currentData[i][j];
				if(i<th){
					var xdiff = target.x - current.x;
					var ydiff = target.y - current.y;
					if(Math.abs(xdiff)<.5) {
						current.x = target.x;
					} else {
						current.x+= (target.x - current.x)*current.ax;
					}
					if(Math.abs(ydiff)<.5) {
						current.y = target.y;
					} else {
						current.y+= (target.y - current.y)*current.ay;
					}
				}
				context.putImageData(imgArr[i][j],current.x,current.y);
			 }
		 }
	 } else {
	 	for(var i=0;i<h;i++){
			for(var j=0;j<w;j++){
				var current = currentData[i][j];
				current.x+=current.vx;
				current.y+=current.vy;
				
				if(current.y>=bottom){
					current.y = bottom;
					current.vy = -current.ny*current.vy;
					if(Math.abs(current.vy)<=1) {
						current.vy = 0;
					}
					current.vx*=current.nx;
					if(Math.abs(current.vx)<=1) current.vx = 0;
				} else {
					current.vy+=1;
				}
				context.putImageData(imgArr[i][j],current.x,current.y);
			}
		}
	 }
	 
 }
 
 function onClick(){
	 reversed = !reversed;
	 if(reversed){
		 for(var i=0;i<h;i++){
			for(var j=0;j<w;j++){
				var current = currentData[i][j];
				current.vx = (Math.random()-Math.random())*3;
				current.vy = -Math.random()*10;
			}
		 }
	 } else {
		 th=1;
		 targetData = posData
	 }
 }
 
 
 function $(id){
	 return document.getElementById(id);
 }