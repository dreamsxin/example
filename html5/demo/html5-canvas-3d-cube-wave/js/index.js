var renderer, scene, camera, cube;

var ww = window.innerWidth,
	wh = window.innerHeight;

function init(){

	renderer = new THREE.WebGLRenderer({
    canvas : document.querySelector('canvas'),
    antialias: true
  });
	renderer.setSize(ww,wh);
	renderer.setClearColor(0xffffff);

	scene = new THREE.Scene();
  

	camera = new THREE.PerspectiveCamera(50,ww/wh, 0.1, 10000 );
	camera.position.set(180,180,180);
	camera.lookAt(new THREE.Vector3(0,0,0));
	scene.add(camera);

  controls = new THREE.OrbitControls(camera);
  
	directionalLight = new THREE.DirectionalLight( 0xe6e3b0, 1.0 );
	directionalLight.position.set( 350, 0, 0  );
	directionalLight.lookAt(new THREE.Vector3(0,0,0));
	scene.add( directionalLight );
  
	directionalLight = new THREE.DirectionalLight( 0x3e5483, 1.0 );
	directionalLight.position.set( 0, 0, 350  );
	directionalLight.lookAt(new THREE.Vector3(0,0,0));
	scene.add( directionalLight );
  
	directionalLight = new THREE.DirectionalLight( 0x83b9b6, 1.0 );
	directionalLight.position.set( 0, 200, 0  );
	directionalLight.lookAt(new THREE.Vector3(0,0,0));
	scene.add( directionalLight );
	createCubes();
	render();
}

var cubes = new THREE.Object3D();
function createCubes(){
  
  var geometry = new THREE.BoxGeometry(10,30,10);
  var texture = new THREE.MeshLambertMaterial({color:0xffffff });
  
  for(var i=-7;i<7;i++){
    for(var j=-7;j<7;j++){
      cube = new THREE.Mesh(geometry, texture);
      cube.position.x = 10*i;
      cube.position.z = 10*j;
      cube.index = Math.sqrt( (cube.position.x)*(cube.position.x) + (cube.position.z)*(cube.position.z) )*0.1;
      TweenMax.to(cube.scale, 1.6,{
        y:4,
        repeat:-1,
        yoyo:true,
        delay:-cube.index*0.18,
        ease: Power2.easeInOut
      });
      cubes.add(cube);
    }
  }
  
  scene.add(cubes);
};


var render = function () {
	requestAnimationFrame(render);
  
	renderer.render(scene, camera);
};

init();