//===================================================== canvas
    var container = document.getElementById('paint');
    var renderer = new THREE.WebGLRenderer({antialiase:true});
    container.appendChild(renderer.domElement);
    renderer.setSize(container.getBoundingClientRect().width, container.getBoundingClientRect().height);
    renderer.setClearColor(0x151515, 1);



    //===================================================== camera
    var scene = new THREE.Scene();
    var camera = new THREE.PerspectiveCamera(45, container.getBoundingClientRect().width / container.getBoundingClientRect().height, 1, 10000);



    //===================================================== resize
    window.addEventListener("resize", function() {
      let width = container.getBoundingClientRect().width;
      let height = container.getBoundingClientRect().height;
      renderer.setSize(width, height);
      camera.aspect = width / height;
      camera.updateProjectionMatrix();
    });


    //===================================================== camera
    var plane = new THREE.GridHelper(150, 50);
    plane.material.color = new THREE.Color('gray');
    scene.add(plane);




    //===================================================== controls
    var controls = new THREE.OrbitControls(camera, renderer.domElement);
    controls.enableDamping = true;
    controls.dampingFactor = 0.25;
    controls.enableZoom = true;
    controls.maxPolarAngle = Math.PI / 2.1;



    //===================================================== add Light
    var light = new THREE.DirectionalLight(0xefefff, 1.5);
    light.position.set(1, 1, 1).normalize();
    scene.add(light);

    var light = new THREE.DirectionalLight(0xffefef, 1.5);
    light.position.set(-1, -1, -1).normalize();
    scene.add(light);


    //===================================================== add blue light on the seats
    var light = new THREE.DirectionalLight(new THREE.Color("blue"), 1.5);
    light.position.set(0, 200, 100);
    light.castShadow = true;
    light.shadow.camera.top = 280;
    light.shadow.camera.bottom = -200;
    light.shadow.camera.left = -200;
    light.shadow.camera.right = 200;
    scene.add(light);




    //==================================================== Add obj
    loader = new THREE.LegacyJSONLoader();
    loader.load('https://raw.githubusercontent.com/baronwatts/models/master/airplane2.js', function(geometry, materials) {

      var matt = new THREE.MeshPhongMaterial({
        vertexColors: THREE.FaceColors,
        side: THREE.DoubleSide,
        wireframe: true,
        transparent: true,
        opacity: .085
      });
      var obj = new THREE.Mesh(geometry, matt);
      obj.scale.set(.5, .5, .5);
      obj.rotateY(-Math.PI);
      obj.receiveShadow = true;
      obj.castShadow = true;
      scene.add(obj);

    });


    //===================================================== seating rows
    var row = new THREE.Group();
    row.rotateY(Math.PI);
    row.position.y = 5;
    row.position.z = -3.75;
    row.translateX(20);
    scene.add(row);


    loader = new THREE.LegacyJSONLoader();
    loader.load('https://raw.githubusercontent.com/baronwatts/models/master/seats.js', function(geometry, materials) {

      var matt = new THREE.MeshLambertMaterial({
        vertexColors: THREE.FaceColors,
        color: new THREE.Color('#888'),
        side: THREE.DoubleSide
      });

      for (var i = 0; i < 30; i++) {
        for (var j = 0; j < 2; j++) {

          var obj2 = new THREE.Mesh(geometry, matt);
          obj2.scale.set(3, 3, 3);
          obj2.rotateY(Math.PI / 2);
          obj2.position.z = -3 * j;
          obj2.position.x = -1.25 * i;
          obj2.receiveShadow = true;
          obj2.castShadow = true;
          row.add(obj2);

        }
      }

    });



    //==================================================== number
    var canvas = document.getElementById("number");
    var ctx = canvas.getContext("2d");
    var x = 32;
    var y = 32;
    var radius = 30;
    var startAngle = 0;
    var endAngle = Math.PI * 2;

    ctx.fillStyle = "rgba(0, 0, 0, 0)";
    ctx.beginPath();
    ctx.arc(x, y, radius, startAngle, endAngle);
    ctx.fill();

    ctx.fillStyle = "#ccc";
    ctx.font = "bold 32px Roboto Condensed";
    ctx.textAlign = "center";
    ctx.textBaseline = "middle";
    ctx.fillText("56%", x, y);

    var numberTexture = new THREE.CanvasTexture(document.querySelector("#number"));
    var spriteMaterial = new THREE.SpriteMaterial({
      map: numberTexture,
      alphaTest: 0.5,
      transparent: true,
      depthTest: false,
      depthWrite: false
    });

    sprite = new THREE.Sprite(spriteMaterial);
    sprite.position.set(0, 10, 0);
    sprite.scale.set(2, 2, 2);
    scene.add(sprite);



    //==================================================== number
    var canvas2 = document.getElementById("number2");
    var ctx = canvas2.getContext("2d");
    ctx.fillStyle = "#ccc";
    ctx.font = "bold 32px Roboto Condensed";
    ctx.textAlign = "center";
    ctx.textBaseline = "middle";
    ctx.fillText("56%", x, y);

    var numberTexture = new THREE.CanvasTexture(document.querySelector("#number2"));
    var spriteMaterial = new THREE.SpriteMaterial({
      map: numberTexture,
      alphaTest: 0.5,
      transparent: true,
      depthTest: false,
      depthWrite: false
    });

    sprite = new THREE.Sprite(spriteMaterial);
    sprite.position.set(-10, 10, 0);
    sprite.scale.set(2, 2, 2);
    scene.add(sprite);




    //==================================================== number
    var canvas2 = document.getElementById("number4");
    var ctx = canvas2.getContext("2d");
    ctx.fillStyle = "gray";
    ctx.font = "bold 32px Roboto Condensed";
    ctx.textAlign = "center";
    ctx.textBaseline = "middle";
    ctx.fillText("49%", x, y);

    var numberTexture = new THREE.CanvasTexture(document.querySelector("#number4"));
    var spriteMaterial = new THREE.SpriteMaterial({
      map: numberTexture,
      alphaTest: 0.5,
      transparent: true,
      depthTest: false,
      depthWrite: false
    });

    sprite = new THREE.Sprite(spriteMaterial);
    sprite.position.set(-18, 10, 0);
    sprite.scale.set(2, 2, 2);
    scene.add(sprite);




    //==================================================== number
    var canvas2 = document.getElementById("number3");
    var ctx = canvas2.getContext("2d");
    ctx.fillStyle = "#3fe687";
    ctx.font = "bold 32px Roboto Condensed";
    ctx.textAlign = "center";
    ctx.textBaseline = "middle";
    ctx.fillText("69%", x, y);

    var numberTexture = new THREE.CanvasTexture(document.querySelector("#number3"));
    var spriteMaterial = new THREE.SpriteMaterial({
      map: numberTexture,
      alphaTest: 0.5,
      transparent: true,
      depthTest: false,
      depthWrite: false
    });

    sprite = new THREE.Sprite(spriteMaterial);
    sprite.position.set(15, 10, 0);
    sprite.scale.set(2, 2, 2);
    scene.add(sprite);




    //===================================================== Circle path for the camera
    var segmentCount = 10;
    var radius = 50;
    var xyzArray = new Array(segmentCount)
      .fill(null)
      .map(
        (d, i) =>
        new THREE.Vector3(
          /*x*/
          Math.cos(i / segmentCount * Math.PI * 2) * radius,
          /*y*/
          5,
          /*z*/
          Math.sin(i / segmentCount * Math.PI * 2) * radius
        )
      );

    var curve = new THREE.CatmullRomCurve3(xyzArray);
    curve.closed = true;
    var g2 = new THREE.Geometry();
    g2.vertices = curve.getPoints(50);
    var m2 = new THREE.LineBasicMaterial({
      color: new THREE.Color("skyblue")
    });
    var curveObject = new THREE.Line(g2, m2);
    //scene.add(curveObject);



    //===================================================== buttons

    let button1 = document.querySelector('.one');
    button1.addEventListener('click', function() {
      zoom(-30, 10, 5);
    });

    let button2 = document.querySelector('.two');
    button2.addEventListener('click', function() {
      zoom(-20, 10, 20)
    });

    let button3 = document.querySelector('.three');
    button3.addEventListener('click', function() {
      zoom(5, 10, 30);
    });

    let button4 = document.querySelector('.four');
    button4.addEventListener('click', function() {
      zoom(30, 10, 5);
    });



    function zoom(x, y, z) {
      var from = {
        x: camera.position.x,
        y: camera.position.y,
        z: camera.position.z
      };
      var to = {
        x: x,
        y: y,
        z: z
      };
      var tween = new TWEEN.Tween(from)
        .to(to, 900)
        .easing(TWEEN.Easing.Quadratic.Out)
        .onUpdate(function() {
          camera.position.set(this.x, this.y, this.z);
          camera.lookAt(new THREE.Vector3(0, 0, 0));
        })
        .onComplete(function() {
          camera.lookAt(new THREE.Vector3(0, 0, 0));
        })
        .start();
    };




    //===================================================== animate

    spline = curve;
    camPosIndex = 0;
    clock = new THREE.Clock();

    function animate() {

      TWEEN.update();

      camPosIndex++;
      if (camPosIndex > 1000) {
        camPosIndex = 0;
      }


      let camPos = spline.getPoint(camPosIndex / 1000); //move camera x,y,z
      let camRot = spline.getTangent(camPosIndex / 1000); //rotates camera x,y,z


      //move camera on path
      if (document.getElementById("myCheck").checked == true) {
        camera.position.set(camPos.x, camPos.y, camPos.z + 5);
        camera.lookAt(new THREE.Vector3(0, 0, 0));
      } else {
        camera.position.set(camera.position.x, camera.position.y, camera.position.z);
        camera.lookAt(new THREE.Vector3(0, 0, 0));
      }


      renderer.render(scene, camera);
      requestAnimationFrame(animate);

    }

    animate();
    zoom(1, 10, 40);