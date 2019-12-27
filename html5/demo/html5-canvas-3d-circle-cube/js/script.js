const radiansToDegrees = (_val) => {  
  return _val * (Math.PI/180);
}

const componentToHex = (c) => {
  var hex = c.toString(16);
  return hex.length == 1 ? "0" + hex : hex;
}

const rgbToHex = (r, g, b) => {
  return "#" + componentToHex(r) + componentToHex(g) + componentToHex(b);
}

const hslToRgb = (_h, s, l) => {	
	var h = Math.min(_h, 359)/60;
	
	var c = (1-Math.abs((2*l)-1))*s;
	var x = c*(1-Math.abs((h % 2)-1));
	var m = l - (0.5*c);
	
	var r = m, g = m, b = m;
	
	if (h < 1) {
		r += c, g = +x, b += 0;
	} else if (h < 2) {
		r += x, g += c, b += 0;
	} else if (h < 3) {
		r += 0, g += c, b += x;
	} else if (h < 4) {
		r += 0, g += x, b += c;
	} else if (h < 5) {
		r += x, g += 0, b += c;
	} else if (h < 6) {
		r += c, g += 0, b += x;
	} else {
		r = 0, g = 0, b = 0;
	}
	
	//return 'rgb(' + Math.floor(r*255) + ', ' + Math.floor(g*255) + ', ' + Math.floor(b*255) + ')';
	return rgbToHex(Math.floor(r*255), Math.floor(g*255) , Math.floor(b*255) );
}


const createSpectrum = (length) => {
	var colors = [];
	// 270 if we don't want the spectrum to loop
	// 360 if we do want the spectrum to loop
	var step = 360/length;
	for (var i = 1; i <= length; i++) {
		var color = hslToRgb((i)*step, 0.5, 0.5);
		colors.push(color);
	}
	
	return colors;
}
//48 because we have 4 faces to populate x 12 cubes
const spectrum = createSpectrum(48).reverse();

let illo = new Zdog.Illustration({
  element: '.zdog-canvas',
	dragRotate: true
});

var mainGroup = new Zdog.Anchor({
  addTo: illo,
  translate: {x: -0, y: 0, z: 0 }
});

var boxGroup = new Zdog.Group({
  addTo: mainGroup,
  translate: {x: -400, y: 0, z: 0 }
});

const makeBoxes = () => {
	let leftFaceCount = 0, topFaceCount = 11, rightFaceCount = 23,  bottomFaceCount = 35;
	for(let i = 0; i < 12; i++){
		let boxAnchor = new Zdog.Anchor({
			addTo: mainGroup,
			rotate: {  y: radiansToDegrees(360 - (i * 30))},
			translate: {x: 0, y: 0, z: 0 }
		});		

		let box = new Zdog.Box({
			addTo: boxAnchor,
			translate: {x: -200},
			width: 60,
			height: 60,
			depth: 60,
			stroke: 2,
			color: '#000',
			// remove left & right faces
			leftFace: spectrum[leftFaceCount],
			rightFace: spectrum[rightFaceCount],
			rearFace: '#000',
			topFace: spectrum[topFaceCount],
			bottomFace: spectrum[bottomFaceCount],
			backface: true,
			rotate: { z: radiansToDegrees((90/12) * i)}
		})
		
		leftFaceCount ++;
		rightFaceCount ++;
		topFaceCount ++;
		bottomFaceCount ++;
		
	}
}

mainGroup.rotate.x = radiansToDegrees(-45);

function render () {
	
	for(let i = 1; i < mainGroup.children.length; i++) {
		mainGroup.children[i].children[0].rotate.z -= (0.031)
	}
	illo.updateRenderGraph();
	requestAnimationFrame(render)
}

makeBoxes();
render();