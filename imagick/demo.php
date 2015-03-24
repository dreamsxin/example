<?php
$image = new Phalcon\Image\Adapter\Imagick(NULL, 250, 250);
$image->background('#FFFFFF');
 // center
$image->text('phalcon', NULL, NULL, 100,  '#000000', 60);
// north
$image->text('hello', NULL, FALSE, 100,  '#000000', 60);
// south
$image->text('world', NULL, TRUE, 100,  '#000000', 60);
$image->save('text2.png');

//passthru('./pinbarrel "1.5,-0.5,0,0,0" text2.png text3.png');
$image = new Phalcon\Image\Adapter\Imagick('text2.png');
$sf = ($image->getWidth() > $image->getHeight() ? $image->getHeight() : $image->getWidth())/2;
$w = $image->getWidth() / 2;
$h = $image->getHeight() / 2;
$ax = 1.2;
$bx = 0;
$cx = 0;
$dx = -0.2;
Phalcon\Image\Adapter\Imagick::convert(array(
	'convert',
	'text2.png',
	'-virtual-pixel',
	'black',
	'-monitor',
	'-fx',
	"xd=(i-$w); yd=(j-$h); rd=hypot(xd,yd)/$sf; fdx=1.5 + -0.5*rd; xs=fdx*xd+$w; ys=ys=fdx*yd+$h; u.p{xs,ys}",
	'text3.png'
));

//passthru('./mottle -t b -a 5 -g 1 -c white -r b text3.png text4.png');
$image = new Phalcon\Image\Adapter\Imagick('text3.png');
$image->save('text3-1.png');
$ww = $image->getWidth();
$hh = $image->getHeight();
$granularity = 1;
$amount = 5;
$color = 'white';
Phalcon\Image\Adapter\Imagick::convert(array(
	'convert',

	'-size',
	"${ww}x${hh}",
	'xc:',
	'+noise',
	'Random',
	'-virtual-pixel',
	'tile',
	'-blur',
	"0x${granularity}",
	"-set",
	"colorspace",
	"RGB",
	"-contrast-stretch",
	"0%",
	"-channel",
	"G",
	"-separate",
	"text3-1.png",
));

Phalcon\Image\Adapter\Imagick::convert(array(
	'convert',
	"text3.png",
	"(",
	'-size',
	"${ww}x${hh}",
	"xc:${color}",
	")",
	"text3-1.png",
	"-set",
	"colorspace",
	"RGB",
	"-compose",
	"plus",
	"-composite",
	"text4.png",
));

//passthru('./mottle -t d -a 3 -g 1 text4.png text5.png');
$image = new Phalcon\Image\Adapter\Imagick('text4.png');
$image->save('text4-1.png');
$ww = $image->getWidth();
$hh = $image->getHeight();
$granularity = 1;
$amount = 3;
$color = 'black';
Phalcon\Image\Adapter\Imagick::convert(array(
	'convert',

	'-size',
	"${ww}x${hh}",
	'xc:',
	'+noise',
	'Random',
	'-virtual-pixel',
	'tile',
	'-blur',
	"0x${granularity}",
	"-set",
	"colorspace",
	"RGB",
	"-contrast-stretch",
	"0%",
	"-channel",
	"G",
	"-separate",
	"text4-1.png",
));

Phalcon\Image\Adapter\Imagick::convert(array(
	'convert',
	"text4.png",
	"(",
	'-size',
	"${ww}x${hh}",
	"xc:${color}",
	")",
	"text4-1.png",
	"-set",
	"colorspace",
	"RGB",
	"-compose",
	"plus",
	"-composite",
	"text5.png",
));

//passthru('./zoomblur -a 1.1 text5.png text6.png');
$image = new Phalcon\Image\Adapter\Imagick('text5.png');
$image->save('text5-1.png');
$ww = $image->getWidth()/2;
$hh = $image->getHeight()/2;
$rad= hypot($ww/2,$hh/2);
$zoomfrac= 1.1-1;
$iter= intval($zoomfrac*$rad);
$i=1;
while ($i <= $iter) {
	echo "iteration=$i".PHP_EOL;
	$j=$i + 1;
	#compute running average percentages
	$new=100/$j;
	$old=100-$new;
	$s=100*(1 + $zoomfrac*$i/$iter);

	Phalcon\Image\Adapter\Imagick::convert(array(
		'convert',
		"text5-1.png",
		'-resize',
		$s.'%',
		'-gravity',
		'center',
		'-crop',
		"${ww}x${hh}+0+0",
		"+repage",
		"text5-3.png",
	));

	Phalcon\Image\Adapter\Imagick::convert(array(
		'convert',
		"text5-3.png",
		"text5-1.png",
		'-gravity',
		'center',
		'-define',
		"compose:args=$old%x$new%",
		"-compose",
		"blend",
		"-composite",
		"text5-1.png",
	));

	$i++;
}
$image = new Phalcon\Image\Adapter\Imagick('text5-1.png');
$image->save('text6.png');
