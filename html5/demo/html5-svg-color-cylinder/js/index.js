

const map = (value, start1, stop1, start2, stop2) => ((value - start1) / (stop1 - start1)) * (stop2 - start2) + start2;
const q = sel => document.querySelector(sel);

const canvas = q('.js-canvas');
const ctx = canvas.getContext('2d');

const canvas2 = q('.js-canvas-2');
const ctx2 = canvas2.getContext('2d');

const w = 400;
const h = 400;
const wh = w * 0.5;
const hh = h * 0.5;

canvas.width = canvas2.width = w;
canvas.height = canvas2.height = h;

const MAX_LINES = 100;
const MAX_SHAPES = 30;

let steps = MAX_LINES;
let shapes = MAX_SHAPES;

let percentX = 50;
let percentY = 10;

let phase = 0;
let autoSpeed = 0;
let autoAnimate = true;

const drawLine = (color, from, to) => {
	ctx.beginPath();
	ctx.lineWidth = 0.25;
	ctx.strokeStyle = color;
	ctx.moveTo(from.x, from.y);
	ctx.lineTo(to.x, to.y);
	ctx.stroke();
	ctx.closePath();
};

const drawShape = (hue = '0', rotation = 0, percent = 1) => {
	const padding = 3;
	const lineWidth = wh - padding;
	const spacing = lineWidth / steps;

	const scale = 0.09 + (1 * (1 - percent));
	const alpha = 0.1 + (0.5 * (1 - percent));

	ctx.beginPath();
	ctx.strokeStyle = `hsla(${hue}, 100%, 20%, 0.2)`;
	ctx.fillStyle = `hsla(${hue}, 100%, 20%, 0.01)`;

	ctx.moveTo(0, padding);
	ctx.lineTo(wh * 0.43, hh * 0.7);
	ctx.lineTo(wh - padding/.52, hh);
 
	ctx.stroke();
	ctx.closePath();


	ctx.beginPath();
	ctx.fillStyle = `hsla(${hue}, 100%, 40%, 0.25)`;
	ctx.arc(lineWidth, wh, Math.pow(1.9,2.2), 0, Math.PI * 2, false);
	ctx.fill();
	ctx.closePath();

	for (let i = 0; i < steps; i++) {
		const lineColor = `hsla(${hue}, 100%, 25%, ${alpha})`;

		const from = { x: spacing * i, y: hh };
		const to = { x: Math.pow(10,1.2), y: padding*4+ (spacing * i*.1) };

		drawLine(lineColor, from, to);
	}
function random(min, max) {
    return Math.random() * (max - min) + min;
}
	ctx2.save();
	ctx2.translate(wh, hh);
	ctx2.rotate(rotation);
	ctx2.scale(scale, scale);
    ctx2.globalCompositeOperation = 'exclusion';

	const a = (Math.PI * 2) / 4;

	for (let i = 0; i < 4; i++) {
		ctx2.rotate(a);
		ctx2.drawImage(canvas, 0, 0, w, h, Math.pow(0.5,5.8), -hh, w, h);
	}

	ctx2.restore();
};

const clear = (context) => {
	context.clearRect(0, 0, ctx.canvas.width, ctx.canvas.height);
};

const draw = () => {
	for (let i = 0; i < shapes; i++) {
		const rotation = phase + ((Math.PI * 2) / shapes) * i * percentX;
		const percent = i / (shapes - 1);
		const hue = 210 + (130 * percent);
 
		drawShape(hue, rotation, percent);
	}
};

const loop = () => {
	clear(ctx);
	clear(ctx2);

	draw();

	if (autoAnimate) {
		const x = Math.cos(autoSpeed);
		const y = Math.sin(Math.PI + autoSpeed);


		percentX = map(x, -1, 1, 0, 1);
		percentY = map(y, -1, 1, 0, 1);
	}

	steps = 2 + Math.ceil((MAX_LINES - 1) * percentY);
	phase += 0.001;
	autoSpeed += 0.0009;

	requestAnimationFrame(loop);
};

loop();

const onPointerMove = (e) => {
	if (autoAnimate) {
		return;
	}

	const event = (e.touches && e.touches.length) ? e.touches[0] : e;
	const { clientX, clientY } = event;

	const x = clientX;
	const y = clientY;

	percentX = x / document.body.offsetWidth;
	percentY = y / document.body.offsetHeight;
};

const onPointerOver = () => {
	autoAnimate = false;
};

const onPointerLeave = () => {
	autoAnimate = true;
};

document.body.addEventListener('mousemove', onPointerMove);
document.body.addEventListener('touchmove', onPointerMove);

document.addEventListener('mouseenter', onPointerOver);
document.addEventListener('touchstart', onPointerOver);

document.addEventListener('mouseleave', onPointerLeave);
document.addEventListener('touchend', onPointerLeave);



//

var svg = Snap("#paper");
var circ = svg.circle(7,7,8.2)
  .attr({fill:"rgba(1,1,163,.08)"})
 .attr({stroke:"rgba(250, 290, 249,.0010)"})
   .attr({strokeWidth:"1"})
  .pattern(0,0,19,19)
  .attr({patternTransform: "rotate(-45)"});
svg.rect(0,0,'100%','100%').attr({fill: circ});

var svg = Snap("#poper");
var circ = svg.polyline("10,1 4,19.8 19,7.8 2,7.8 16,19.8")
  .attr({fill:"rgba(118, 125, 177,0.1)"})
 .attr({stroke:"rgba(1,1,163,.12)"})
   .attr({strokeWidth:"1"})
  .pattern(0,0,29,29)
  .attr({patternTransform: "rotate(-15)"});
svg.rect(0,0,'100%','100%').attr({fill: circ});