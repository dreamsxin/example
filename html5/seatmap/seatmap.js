window.onload = function () {
	
	var seats = [
		{"x":1,"y":1}, {"x":2,"y":1}, {"x":3,"y":1}, {"x":4,"y":1}, {"x":5,"y":1}, {"x":6,"y":1}, {"x":7,"y":1},
		{"x":1,"y":2}, {"x":2,"y":2}, {"x":3,"y":2}, {"x":4,"y":2}, {"x":5,"y":2}, {"x":6,"y":2}, {"x":7,"y":2},
		{"x":1,"y":3}, {"x":2,"y":3}, {"x":3,"y":3}, {"x":4,"y":3}, {"x":5,"y":3}, {"x":6,"y":3}, {"x":7,"y":3},
		{"x":1,"y":4}, {"x":2,"y":4}, {"x":3,"y":4}, {"x":4,"y":4}, {"x":5,"y":4}, {"x":6,"y":4}, {"x":7,"y":4}
	];
	var radius = 20;

	var line = {
		stroke: "blue",
		fill: "blue",
		"stroke-width": 1,
		"stroke-linecap": "round",
		"stroke-linejoin": "round"
	};
	var paper = Raphael(0, 30, 400, 230);
	var c = paper.rect(175, 0, 50, 5).attr(line);
	var text = paper.text(200, 15, "舞台屏幕").attr({font:"20px Arial", opacity: 0.5, fill: "#0f0"});

	for (var i = 0; i < seats.length; i++)
	{
		var seatObj = seats[i];
		var x = seatObj.x * 2.5 * radius,
			y = seatObj.y * 2.5 * radius;
		
		var seat = paper.circle(x, y, radius).attr({stroke: "none", fill: "#f00", opacity: .4});
		
		seat.mouseover(function(){
			this.attr("opacity", 1);
		});
		
		seat.mouseout(function(){
			this.attr("opacity", .4);
		});
		
		seat.click(function(){
			if (this.attrs.fill == "#f00")
			{
				this.attr("fill", "#000");
			}
			else
			{
				this.attr("fill", "#f00");
			}
			document.getElementById('selectedSeat').innerHTML = '选择了位置： ' + this.attrs.cy/(2.5 * radius) + '排,' + this.attrs.cx/(2.5 * radius) + '列';
		});
		
	}
	
}