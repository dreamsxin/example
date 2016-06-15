jQuery.fn.extend({
	paper: null,
	options: null,
	mode: 0,
	cuurentSeat: null,
	seatChart : function(options){
		this.options = jQuery.extend({ //默认参数选项列表
			stage: 'top',
			rows: 10,
			columns: 30,
			seats: null,
			stages: null,
			radius: 20,
			selectSeat: null,
			unSelectSeat: null,
			selectStage: null,
			unSelectStage: null
		}, options);
		this.paper = Raphael($(this).attr("id"), 0, 0);
		console.log(this.paper);
		return this.drawSeats(this.options);
	},
	setMode: function(mode){
		console.log("mode", mode);
		this.mode = mode;
	},
	getSelectSeat: function(){
		return this.cuurentSeat;
	},
	clear: function(){
		console.log("clear", this);
		this.paper.clear();
	},
	drawSeats : function(options){
		if (!this.paper) {
			return false;
		}
		this.options = jQuery.extend(this.options, options);
		var seatChart = this;
		var line = {
			stroke: "blue",
			fill: "blue",
			"stroke-width": 1,
			"stroke-linecap": "round",
			"stroke-linejoin": "round",
			title: "舞台屏幕"
		};

		console.log(this.options);

		var width = this.options.radius * this.options.columns * 2.5;
		var height = this.options.radius * this.options.rows * 2.5;
		this.paper.setSize(width , height);
		console.log(this.paper);

		for (var i = 1; i < this.options.rows; i++) {
			for (var j = 1; j < this.options.columns; j++) {
				var index = (i - 1)*this.options.columns + j;
				var y = i * 2.5 * this.options.radius,
					x = j * 2.5 * this.options.radius;
				
				var label = i + "排" + j + "列";
				var isSeat = 0, isStage = 0;
				if (this.options.seats[index]) {
					label = this.options.seats[index];
					isSeat = 1;
					this.attr("fill", "#009900");
					this.attr("opacity", 1);
				} else if (this.options.stages[index]) {
					label = this.options.stages[index];
					isStage = 1;
				}
				var seat = this.paper.circle(x, y, this.options.radius).attr({"title": "编号:" + index + "，位置:" + label, stroke: "#fff", "stroke-width": 5, fill: "#333", opacity: .6});
				
				seat.data("pos",  i + "排" + j + "列");
				seat.data("label", label);
				seat.data("index", index);
				seat.mouseover(function(){
					if (this.attrs.fill == "#333") {
						this.attr("opacity", 1);
					}
					this.tooltip({ msg: this.attrs.title});
				});

				seat.mouseout(function(){
					if (this.attrs.fill == "#333") {
						this.attr("opacity", .6);
					}
					this.untooltip();
				});

				if (isSeat) {
					seat.attr("fill", "#009900");
					seat.attr("opacity", 1);
					if (seatChart.options.selectSeat && $.isFunction(seatChart.options.selectSeat))
					{
						seatChart.options.selectSeat(seat);
					}
				} else if (isStage) {
					seat.attr("fill", "#bf852f");
					seat.attr("opacity", 1);
					this.attr("title", "舞台编号:" + seat.data('index') + "，位置:" + this.data("pos"));
					if (seatChart.options.selectStage && $.isFunction(seatChart.options.selectStage))
					{
						seatChart.options.selectStage(seat);
					}
				}

				seat.click(function(){
					console.log(this.attrs);
					if (seatChart.mode == 0) { // 选择座位
						if (this.attrs.fill != "#009900") {
							this.attr("fill", "#009900");
							this.attr("opacity", 1);
							seatChart.cuurentSeat = this;
							if (seatChart.options.selectSeat && $.isFunction(seatChart.options.selectSeat))
							{
								seatChart.options.selectSeat(this);
							}
						} else {
							this.attr("fill", "#333");
							
							if (seatChart.options.unSelectSeat && $.isFunction(seatChart.options.unSelectSeat))
							{
								seatChart.options.unSelectSeat(this);
							}
						}
					} else if (seatChart.mode == 1) { // 选择舞台
						if (this.attrs.fill != "#bf852f") {
							this.attr("fill", "#bf852f");
							this.attr("opacity", 1);
							this.attr("title", "舞台编号:" + seat.data('index') + "，位置:" + this.data("pos"));
							if (seatChart.options.selectStage && $.isFunction(seatChart.options.selectStage))
							{
								seatChart.options.selectStage(this);
							}
						} else {
							this.attr("fill", "#333");
							this.attr("title", "编号:" + seat.data('index') + "，位置:" + this.data("label"));
							if (seatChart.options.unSelectStage && $.isFunction(seatChart.options.unSelectStage))
							{
								seatChart.options.unSelectStage(this);
							}
						}
					}
				});
			}
		}
		return this;
	}
}); 