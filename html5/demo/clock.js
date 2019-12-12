var dom = document.getElementById("clock");
var ctx = dom.getContext("2d");
var width = ctx.canvas.width;
var height = ctx.canvas.height;
var r=  width / 2;
var rem = width/200;
function drawAround(){
    ctx.save();
  
    ctx.beginPath();  ctx.translate(r, r);
    ctx.lineWidth = 10 * rem;
    ctx.arc(0, 0, r-5, 0,2*Math.PI, false);
    ctx.stroke();
    var housNumbers = [3,4,5,6,7,8,9,10,11,12,1,2];
    ctx.font="18px Arial";
    ctx.textAlign = "center";
    ctx.textBaseline = 'middle';
    housNumbers.forEach(function(number, i){
        var rad = 2 * Math.PI/12 *i;
        var x = Math.cos(rad) * (r - 30);
        var y = Math.sin(rad) * (r -30);
        
        ctx.fillText(number, x, y);
    });

    for(var i =0;i<60;i++){
        var rad = 2 * Math.PI/60 * i;
        var x = Math.cos(rad) * (r - 18);
        var y = Math.sin(rad) * (r -18);
        ctx.beginPath();
        if( i % 5 != 0){
            ctx.fillStyle = "#eee";
        }else {
            ctx.fillStyle = "#000";
        }
        ctx.arc(x, y, 2, 0,2*Math.PI, false);
        ctx.fill();
    }

}

function drawhHour(hour, minute){
    ctx.save()
    ctx.beginPath();
     var rad = 2 * Math.PI/12 *hour;
    var mrad = 2 * Math.PI / 12 /60 * minute;
    ctx.rotate(rad + mrad);
    ctx.lineWidth = 6;
    ctx.lineCap = "round"
    ctx.moveTo(0, 10);
    ctx.lineTo(0, -r/2);
    ctx.stroke();
    ctx.restore();

}
function drawMinute(minute){
    ctx.save()
    ctx.beginPath();
    var rad = 2 * Math.PI/60 *minute;
    ctx.rotate(rad);
    ctx.lineWidth = 3;
    ctx.lineCap = "round"
    ctx.moveTo(0, 10);
    ctx.lineTo(0, -r +30);
    ctx.stroke();
    ctx.restore();
}
function drawSecond(second){
    ctx.save()
    ctx.beginPath();
    var rad = 2 * Math.PI/60 *second;
    ctx.rotate(rad);
    ctx.fillStyle = "#f00";
    ctx.moveTo(-2, 20);
    ctx.lineTo(2, 20);
    ctx.lineTo(1, -r + 18);
    ctx.lineTo(-1, -r + 18);
    ctx.fill();
    ctx.restore();
}
function drawDot(){
    ctx.beginPath();
    ctx.fillStyle = "#fff";
    ctx.arc(0, 0,3,0,2*Math.PI,false);
    ctx.fill();
}
function draw(){
    ctx.clearRect(0, 0, width, height)
    var now = new Date();
    var hour = now.getHours();
    var minute = now.getMinutes();
    var second = now.getSeconds()

    drawAround();
    drawSecond(second);
    drawMinute(minute);
    drawhHour(hour, minute);
    drawDot();
    ctx.restore();
}
draw();
setInterval( draw,1000);


