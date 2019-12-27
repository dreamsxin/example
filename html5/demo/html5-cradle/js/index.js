(function() {
  
  this.Cradle = {};
  
  var Engine = Matter.Engine,
      Gui = Matter.Gui,
      World = Matter.World,
      Bodies = Matter.Bodies,
      Body = Matter.Body,
      Composite = Matter.Composite,
      Composites = Matter.Composites,
      Common = Matter.Common,
      Constraint = Matter.Constraint;
  
  var _engine;
  
  Cradle.init = function() {
    var container = document.getElementById('stage');
    
    // engine options - these are the defaults
    var options = {
      positionIterations: 5,
      velocityIterations: 3,
      enableSleeping: false,
      timeScale: 1
    };
    
    _engine = Engine.create(container, options);
    Engine.run(_engine);
    
    var _world = _engine.world;
    World.clear(_world);
    Engine.clear(_engine);
    _engine.enabled = true;
    _engine.enableSleeping = false;
    _engine.world.gravity.y = 1;
    
    var renderOptions = _engine.render.options;
        renderOptions.wireframes = true;
        renderOptions.showAxes = true;
        renderOptions.showPositions = true;
    
    var cradle = Composites.newtonsCradle(240, 10, 6, 30, 200);
    World.addComposite(_world, cradle);
    Body.translate(cradle.bodies[0], { x: -180, y: -100 });
  };

  if (window.addEventListener) {
    window.addEventListener('load', Cradle.init);
  } 
  else if (window.attachEvent) {
    window.attachEvent('load', Cradle.init);
  }
  
})();