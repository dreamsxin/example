var xmlns = "http://www.w3.org/2000/svg",
  xlinkns = "http://www.w3.org/1999/xlink",
  select = function(s) {
    return document.querySelector(s);
  },
  selectAll = function(s) {
    return document.querySelectorAll(s);
  },
    flameContainer = select('.flameContainer'),
    sparksContainer = select('.sparksContainer'),
    flameSVG = select('.flameSVG'),
    flamePosXArr = [10, -10],
    flameOffset = 0.34,
    sparkOffset = 0.72,
    numFlames = 50

  

 CustomEase.create("return", "M0,0 C0,0 0.162,1 0.4,1 0.918,1 1,0 1,0");
 CustomEase.create("sparkFlicker", "M0,0 C0.126,0.382 0.216,0.572 0.414,0.482 0.821,0.296 0.984,0.94 1,1");
 CustomEase.create("flameJump", "M0,0 C0.126,0.382 0.256,0.248 0.406,0.23 0.85,0.176 0.984,0.94 1,1");
TweenMax.set('svg', {
  visibility: 'visible'
})
TweenMax.set('.whole', {
 scale:1.52,
 transformOrigin:'50% -500%'
})
var mainTl = new TimelineMax({});
var flameTl = new TimelineMax({repeat:0});
function createFlames(){
 for(var i = 0; i < numFlames; i++){
  
  var f = select('.flame').cloneNode(true);
  
  flameContainer.appendChild(f);
  
  TweenMax.set(f, {
   x:(i%2) ? flamePosXArr[0] : flamePosXArr[1],
   transformOrigin:'50% 50%',
   rotation:-45
  })
  var fTl = new TimelineMax({repeat:-1, repeatDelay:((numFlames-1) * flameOffset)-2});
  fTl.to(f, 2, {
    x:(i%2)? '-=22' : '+=22',
   scale:10,
   ease:'return'
  })
  .to(f, 2, {
   y:-145,
   ease:'flameJump'
  },'-=2')
  .to(f, 2, {
   fill:'#F73B01',
   ease:Sine.easeOut
  },'-=2')
  .to(f, 2, {

   alpha:0,
   ease:Expo.easeIn
  },'-=2')  

  flameTl.add(fTl, i*flameOffset);
  
  var s = select('.spark').cloneNode(true);
  
  sparksContainer.appendChild(s);
  
  TweenMax.set(s, {
   x:(i%3) ? flamePosXArr[1] : flamePosXArr[0],
   transformOrigin:'50% 50%'
  })  
 }

}
createFlames();


var sparkTl = new TimelineMax({repeat:-1});

sparkTl.staggerTo('.spark', 2, {
 cycle:{
  x:['-=25', '+=15', 0, '+=23', '-=5', '+=71', '-=54'],
  scale:function(){
   return Math.random() * 23
  }
 },
 ease:'return'
},sparkOffset)
.staggerTo('.spark', 3, {
 cycle:{
  ease:[SlowMo.ease.config(0.2,0.2),'sparkFlicker',SlowMo.ease.config(0.42,0.52) ],
  y:function(){
   return -(Math.random() * 200)-200
  }
 }
 
},sparkOffset,'-='+sparkTl.duration())
.staggerTo('.spark', 3, {
 cycle:{
  fill:['#F36B01', '#FDBB01', '#ededed']
 },
 ease:Sine.easeIn
},sparkOffset,'-='+sparkTl.duration())
.staggerTo('.spark', 3, {

 alpha:0,
 ease:Expo.easeIn
},sparkOffset,'-='+sparkTl.duration())

sparkTl.timeScale(1)
mainTl.add(flameTl, 0);
mainTl.add(sparkTl, 0);
mainTl.timeScale(1.2).seek(97);

function pokeFire(){
resetSparks();  
 
 
 var pokeTl = new TimelineMax({onComplete:function(){
  resetSparks();
  sparkTl.play(99)
  
 }}).timeScale(2)
pokeTl.staggerTo('.logs path', 0.7, {
  cycle:{
   rotation:[3, -3],
  transformOrigin:['2% 100%', '98% 100%']
  },
  ease:'return'
 },0.02)
 .to(flameContainer, 0.7, {
 scaleY:0.8,
 transformOrigin:'50% 100%',
 ease:'return'
},'-=' + pokeTl.duration())
 .staggerTo('.spark', 3, {
 cycle:{
  x:['-=25', '+=15', 0, '+=23', '-=5', '+=71', '-=54'],
  scale:function(){
   return (Math.random() * 30) 
  }
 },
 ease:'return'
},0.07,'-=1.2')
.staggerTo('.spark', 3, {
 cycle:{
  ease:[SlowMo.ease.config(0.2,0.2),'sparkFlicker',SlowMo.ease.config(0.42,0.52) ],
  y:function(){
   return -(Math.random() * 200)-200
  }
 }
 
},0.07,'-='+pokeTl.duration())
.staggerTo('.spark', 3, {
 cycle:{
  fill:['#F36B01', '#FDBB01']
 },
 ease:Sine.easeIn
},0.07,'-='+pokeTl.duration())
.staggerTo('.spark', 3, {

 alpha:0,
 ease:Expo.easeIn
},0.07,'-='+pokeTl.duration())
 
}

function resetSparks(){
   TweenMax.staggerTo('.spark', 0, {
   cycle:{
    x:function(i){
     return (i%3) ? flamePosXArr[1] : flamePosXArr[0]
    } 
   },   
   y:0,
   alpha:1,
   scale:1
  },0)  
}

flameSVG.onclick = pokeFire;