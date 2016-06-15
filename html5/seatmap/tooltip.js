Raphael.el.tooltip = function (obj, w, h) {
    this.tt = this.paper.set();
	var paperSize = this.paper.getSize();
    var pos = this.getBBox();

	var text = this.paper.text(pos.x + 5, pos.y - 20, obj.msg);
    var box = text.getBBox();
    if (w === undefined) w = box.width + 40;
    if (h === undefined) h = box.height + 10;

	var x = pos.x;
	var y = pos.y;

	if (x + w > this.paper.width) {
		x = pos.x - w;
	}
	if (y + h < this.paper.height) {
		y = pos.y - h;
	}

    //text tooltip
    this.tt.push(this.paper.rect(x, y, w, h, 5).attr({ "stroke-width": 2, "stroke": "#fff", "fill": "#333" }));
    this.tt.push(text.attr({x:x + w/2, y:y + h/2, "stroke-width": 1, "text-anchor": "middle", "stroke": "#fff", "font-size": 13, "fill": "#000" }));
   
	text.toFront();

    return this.tt;
};

Raphael.el.untooltip = function () {
    this.tt && this.tt.remove();
    return this;
};