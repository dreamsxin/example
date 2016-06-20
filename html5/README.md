//页面等比缩放
function zoomin () {
	var orBkgWidth = 1920;
	var orBkgHeight = 1080;
	var scaleSize = 1.0;
	var width = 0;
	var height = 0;
	if (window.innerWidth)
	{
	    width = window.innerWidth;
	}
	else if ((document.body) && (document.body.clientWidth))
	{
	    width = document.body.clientWidth;
	}
	//获取窗口高度 
	if (window.innerHeight)
	{
	    height = window.innerHeight;
	}
	else if ((document.body) && (document.body.clientHeight))
	{
	    height = document.body.clientHeight;
	}
	//通过深入Document内部对body进行检测，获取窗口大小 
	if (document.documentElement && document.documentElement.clientHeight && document.documentElement.clientWidth)
	{
	    height = document.documentElement.clientHeight;
	    width = document.documentElement.clientWidth;
	}

	var tempWidth = orBkgWidth;
	var tempHeight = orBkgHeight;
	var b1 = 0;
	var b2 = 0;
	var transformText = "";
	b1 = orBkgWidth / width;
	b2 = orBkgHeight / height;
	transformText = 'rotate(0deg)';

	var b = b1;
	if (b1 < b2)
	{
	    b = b2;
	}

	if (b > 1)
	{
	    scaleSize = 1 / b;
	}
	else
	{
	    scaleSize = 1 / b;
	}

	var t = transformText + ' scale(' + scaleSize + ',' + scaleSize + ')';

	$("#bkg").css('-webkit-transform', t);
	$("#bkg").css('-moz-transform', t);
}

// 监听窗口大小变动事件
var evt = "onorientationchanged" in window ? "orientationchanged" : "resize";
window.addEventListener(evt, function ()
{
	zoomin();
});