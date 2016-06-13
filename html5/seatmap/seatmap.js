jQuery.fn.extend({
	seatSelected: null,
	showSeats : function(options){
		var options = jQuery.extend({ //默认参数选项列表
			rows: 10,
			columns: 30,
			labels: null,
			radius: 20,
			selected: null
		}, options);

		var line = {
			stroke: "blue",
			fill: "blue",
			"stroke-width": 1,
			"stroke-linecap": "round",
			"stroke-linejoin": "round"
		};

		this.seatSelected = options.click;

		var paper = Raphael($(this).attr("id"), options.radius * options.columns * 2.5, options.radius * options.rows * 2.5);
		var text = paper.text((options.radius * options.columns * 2.5)/2, 15, "舞台屏幕").attr({font:"20px Arial", opacity: 0.5, fill: "#0f0"});
		var c = paper.rect((options.radius * options.columns * 2.5)/2 - 25, 0, 50, 5).attr(line);

		for (var i = 1; i < options.rows; i++) {
			for (var j = 1; j < options.columns; j++) {
				var index = (i - 1)*options.columns + j;
				var y = i * 2.5 * options.radius,
					x = j * 2.5 * options.radius;
				
				var label = i + "排" + j + "座";
				if (options.labels[index]) {
					label = options.labels[index];
				}
				var seat = paper.circle(x, y, options.radius).attr({"title": label, stroke: "none", fill: "#f00", opacity: .4});
				seat.data("label", label);
				seat.data("value", index);
				seat.mouseover(function(){
					this.attr("opacity", 1);
				});

				seat.mouseout(function(){
					this.attr("opacity", .4);
				});

				seat.click(this.seatSelected);
			}
		}
 
	}
}); 