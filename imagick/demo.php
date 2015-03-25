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
$ww = $image->getWidth();
$hh = $image->getHeight();
$rad= hypot($ww,$hh);
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

// passthru('./stainedglass birdofparadise.gif glass.gif')
$kind = "random";
$size = 16;
$offset = 6;
$ncolors = "";
$bright = 100;
$ecolor = "black";
$thick = 1;
$rseed = "";
$average = "no";

$tmpA = 'a.gif';

$image = new Phalcon\Image\Adapter\Imagick('birdofparadise.gif');
$image->save($tmpA);
$colorspace = $image->getInternalImInstance()->getColorspace();

$reduce = "";
$cspace1 = "sRGB";
$cspace2 = "sRGBA";

$modulation = "";

$ww = $image->getWidth();
$hh = $image->getHeight();
$ww1=$ww-1;
$hh1=$hh-1;
$ww2= $ww1 + round($size/2);
$hh2= $hh1 + round($size/2);
$qrange = $image->getInternalImInstance()->getQuantumRange();

$offset = $offset+1;


$tmpC = "tmpC.txt"; 
$tmpG = "tmpG.txt"; 
unlink($tmpC);
unlink($tmpG);
$fhC = fopen($tmpC, 'w+'); 
$fhG = fopen($tmpG, 'w+'); 
fwrite($fhC, "# ImageMagick pixel enumeration: $ww,$hh,255,rgb\n");
$k=0;
$y=0;
$n = 0;
while ( $y < $hh2 ) {
	$x=0;
	while ($x < $ww2 ) { 
		if ($x>$ww1) {
			$x=$ww1;
		}

		if ($y>$hh1) {
			$y=$hh1;
		}

		$rx=mt_rand() / mt_getrandmax();

		if ($rx<0.5) {
			$signx=-1;
		} else {
			$signx=1;
		}

		$offx=intval($signx*(mt_rand() / mt_getrandmax())*$offset); 
		$xx=$x+$offx;

		if ($xx<0) {
			$xx=0;
		}

		if ($xx>$ww1) {
			$xx=$ww1;
		}

		$ry=mt_rand() / mt_getrandmax();

		if ($ry<0.5) {
			$signy=-1;
		} else {
			$signy=1;
		}

		$offy=intval($signy*(mt_rand() / mt_getrandmax())*$offset);	
		$yy=$y+$offy;
		if ($yy<0) {
			$yy=0;
		}

		if ($yy>$hh1) {
			$yy=$hh1;
		}

		fwrite($fhC, "${xx},${yy}: (255,255,255)\n");
		fflush($fhC);

		$g=100*$k/$qrange['quantumRangeLong'];
		fwrite($fhG, "${xx} ${yy} gray(${g}%)\n");
		fflush($fhG);

		$x=$x+$size;
		$k++;
	}
	$y=$y+$size;
}

fclose($fhC);
fclose($fhG);

$txtA = "a.txt";


Phalcon\Image\Adapter\Imagick::convert(array(
	'convert',
	$tmpA,
	'(',
	'-background',
	'black',
	$tmpC,
	')',
	'-alpha',
	'off',
	'-compose',
	'copy_opacity',
	'-composite',
	$txtA,
));

$fh = fopen($txtA, 'r');
fgets($fh);

$txtA2 = "a2.txt";
unlink($txtA2);
$fh2 = fopen($txtA2, 'w+');
$i = 0;
while (($buf = fgets($fh)) !== false) {
	if (strpos($buf, " 0)") === false) {

		$buf = substr($buf, 0, strpos($buf, ":")) .",". substr($buf, strrpos($buf, " ")+1);
		fwrite($fh2, $buf);
		var_dump($buf);
	}
}
fclose($fh);
fclose($fh2);

$txtA2 = "a2.txt";
$tmpA2 = "a2.gif";

Phalcon\Image\Adapter\Imagick::convert(array(
	'convert',
	'-size',
	"${ww}x${hh}",
	"xc:",
	"-sparse-color",
	"voronoi",
	"@${txtA2}",
	$tmpA2,
));

$tmpA3 = 'a3.gif';
Phalcon\Image\Adapter\Imagick::convert(array(
	'convert',
	'-size',
	"${ww}x${hh}",
	"xc:",
	"-sparse-color",
	"voronoi",
	"@${tmpG}",
	"-auto-level",
	"-morphology",
	"edge",
	"diamond:${thick}",
	"-threshold",
	"0",
	"-negate",
	$tmpA3,
));
Phalcon\Image\Adapter\Imagick::convert(array(
	'convert',
	$tmpA2,
	$tmpA3,
	"-alpha",
	"off",
	"-compose",
	"copy_opacity",
	"-composite",
	"-compose",
	"over",
	"-background",
	$ecolor,
	"-flatten",
	"glass.gif"
));
