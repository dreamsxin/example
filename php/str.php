<?php

$string = '{\x22os\x22:1,\x22model\x22:\x22L39u\x22,\x22phone\x22:\x22+8618684039527\x22,\x22osv\x22:17,\x22imei\x22:\x22351759060143829\x22,\x22vers\x22:{\x22sdk\x22:{\x22mCode\x22:86,\x22vName\x22:\x223.2.20811a\x22,\x22vCode\x22:26}},\x22brand\x22:\x22Sony\x22,\x22devicetype\x22:0,\x22imsi\x22:\x22460014018608490\x22,\x22lang\x22:\x22zh\x22,\x22release\x22:\x224.2.2\x22,\x22country\x22:\x22CN\x22}';

$out = preg_replace_callback(
	"(\\\\x([0-9a-f]{2}))i",
	function($a) {
		return chr(hexdec($a[1]));
	},
	$string
);

echo $out;
var_dump(chr(hexdec('\x22')));