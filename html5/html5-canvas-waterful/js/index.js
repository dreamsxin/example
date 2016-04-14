		var waterfallCanvas = function(c, cw, ch){
			
			var _this = this;
			this.c = c;
			this.ctx = c.getContext('2d');
			this.cw = cw;
			this.ch = ch;			
			
			this.particles = [];
			this.particleRate = 6;
			this.gravity = .15;
							

			this.init = function(){				
				this.loop();
			};
			
			this.reset = function(){				
				this.ctx.clearRect(0,0,this.cw,this.ch);
				this.particles = [];
			};
						
			this.rand = function(rMi, rMa){return ~~((Math.random()*(rMa-rMi+1))+rMi);};
			

			this.Particle = function(){
				var newWidth = _this.rand(1,20);
				var newHeight = _this.rand(1, 45);
				this.x = _this.rand(10+(newWidth/2), _this.cw-10-(newWidth/2));
				this.y = -newHeight;
				this.vx = 0;
				this.vy = 0;
				this.width = newWidth;
				this.height = newHeight;
				this.hue = _this.rand(200, 220);
				this.saturation = _this.rand(30, 60);
				this.lightness = _this.rand(30, 60);
			};
			
			this.Particle.prototype.update = function(i){
				this.vx += this.vx; 
				this.vy += _this.gravity;
				this.x += this.vx;
				this.y += this.vy;							
			};
			
			this.Particle.prototype.render = function(){			
				_this.ctx.strokeStyle = 'hsla('+this.hue+', '+this.saturation+'%, '+this.lightness+'%, .05)';
				_this.ctx.beginPath();
				_this.ctx.moveTo(this.x, this.y);
				_this.ctx.lineTo(this.x, this.y + this.height);
				_this.ctx.lineWidth = this.width/2;
				_this.ctx.lineCap = 'round';
				_this.ctx.stroke();
			};
			
			this.Particle.prototype.renderBubble = function(){				
				_this.ctx.fillStyle = 'hsla('+this.hue+', 40%, 40%, 1)';
				_this.ctx.fillStyle = 'hsla('+this.hue+', '+this.saturation+'%, '+this.lightness+'%, .3)';
				_this.ctx.beginPath();
				_this.ctx.arc(this.x+this.width/2, _this.ch-20-_this.rand(0,10), _this.rand(1,8), 0, Math.PI*2, false);
				_this.ctx.fill();
			};
						
			this.createParticles = function(){
				var i = this.particleRate;
				while(i--){
					this.particles.push(new this.Particle());
				}
			};
			
			this.removeParticles = function(){
				var i = this.particleRate;
				while(i--){
					var p = this.particles[i];
					if(p.y > _this.ch-20-p.height){
						p.renderBubble();
						_this.particles.splice(i, 1);
					}	
				}
			};
							
			this.updateParticles = function(){					
				var i = this.particles.length;						
				while(i--){
					var p = this.particles[i];
					p.update(i);											
				};						
			};
			
			this.renderParticles = function(){
				var i = this.particles.length;						
				while(i--){
					var p = this.particles[i];
					p.render();											
				};					
			};
			
			this.clearCanvas = function(){				
				this.ctx.globalCompositeOperation = 'destination-out';
    			this.ctx.fillStyle = 'rgba(255,255,255,.06)';
    			this.ctx.fillRect(0,0,this.cw,this.ch);
				this.ctx.globalCompositeOperation = 'lighter';
			};
			
			this.loop = function(){
				var loopIt = function(){					
					requestAnimationFrame(loopIt, _this.c);					
						_this.clearCanvas();					
						_this.createParticles();					
						_this.updateParticles();					
						_this.renderParticles();	
						_this.removeParticles();
				};
				loopIt();					
			};
		
		};
		
	var isCanvasSupported = function(){
		var elem = document.createElement('canvas');
		return !!(elem.getContext && elem.getContext('2d'));
	};
	
	var setupRAF = function(){
		var lastTime = 0;
		var vendors = ['ms', 'moz', 'webkit', 'o'];
		for(var x = 0; x < vendors.length && !window.requestAnimationFrame; ++x){
			window.requestAnimationFrame = window[vendors[x]+'RequestAnimationFrame'];
			window.cancelAnimationFrame = window[vendors[x]+'CancelAnimationFrame'] || window[vendors[x]+'CancelRequestAnimationFrame'];
		};
		
		if(!window.requestAnimationFrame){
			window.requestAnimationFrame = function(callback, element){
				var currTime = new Date().getTime();
				var timeToCall = Math.max(0, 16 - (currTime - lastTime));
				var id = window.setTimeout(function() { callback(currTime + timeToCall); }, timeToCall);
				lastTime = currTime + timeToCall;
				return id;
			};
		};
		
		if (!window.cancelAnimationFrame){
			window.cancelAnimationFrame = function(id){
				clearTimeout(id);
			};
		};
	};			
	
	if(isCanvasSupported()){
			var c = document.getElementById('waterfall');
			var cw = c.width = 100;
			var ch = c.height = 140;	
			var waterfall = new waterfallCanvas(c, cw, ch);			  
			setupRAF();
			waterfall.init();
	}