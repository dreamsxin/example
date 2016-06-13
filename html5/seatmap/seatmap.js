jQuery.fn.extend({
	paper: null,
	options: null,
	seatSelected: null,
	initSeats : function(options){
		this.options = jQuery.extend({ //默认参数选项列表
			stage: 'top',
			rows: 10,
			columns: 30,
			labels: null,
			radius: 20,
			selected: null
		}, options);
		this.paper = Raphael($(this).attr("id"), 0, 0);
		console.log(this.paper);
		return this.drawSeats(this.options);
	},
	clear: function(){
		console.log(this);
		this.paper.clear();
	},
	drawSeats : function(options){
		if (!this.paper) {
			return false;
		}
		this.options = jQuery.extend(this.options, options);

		var line = {
			stroke: "blue",
			fill: "blue",
			"stroke-width": 1,
			"stroke-linecap": "round",
			"stroke-linejoin": "round",
			title: "舞台屏幕"
		};

		console.log(this.options);

		this.seatSelected = this.options.click;

		var width = this.options.radius * this.options.columns * 2.5;
		var height = this.options.radius * this.options.rows * 2.5;
		this.paper.setSize(width , height);
		console.log(this.paper);
		if (this.options.stage == 'top') {
			var stage = this.paper.rect(width/2 - 25, 0, 50, 20).attr(line);
		} else if (this.options.stage == 'center') {
			var stage = this.paper.rect(width/2 - 25,  height/2 - 10, 50, 20).attr(line);
		} else if (this.options.stage == 'bottom') {
			var stage = this.paper.rect( width/2 - 25, height - 20, 50, 20).attr(line);
		} else if (this.options.stage == 'left') {
			var stage = this.paper.rect(0, height/2 - 10, 50, 20).attr(line);
		} else if (this.options.stage == 'right') {
			var stage = this.paper.rect(width - 50, height/2 - 10, 50, 20).attr(line);
		}

		for (var i = 1; i < this.options.rows; i++) {
			for (var j = 1; j < this.options.columns; j++) {
				var index = (i - 1)*this.options.columns + j;
				var y = i * 2.5 * this.options.radius,
					x = j * 2.5 * this.options.radius;
				
				var label = i + "排" + j + "座";
				if (this.options.labels[index]) {
					label = this.options.labels[index];
				}
				var seat = this.paper.circle(x, y, this.options.radius).attr({"title": label, stroke: "none", fill: "#f00", opacity: .4});
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
		return this;
	}
}); 