<?php

function line_print($v){
	echo str_pad($v, 32, " ", STR_PAD_LEFT).PHP_EOL;
}

// 数值转 ascii
$v = 20000000;
line_print('v='.$v);
line_print('0x7F='.decbin(0x7F));
line_print('0x80='.decbin(0x80));
echo str_pad('', 32, "-", STR_PAD_LEFT).PHP_EOL;
function writeVInt($value)
{
	$val = '';
	settype($value, 'integer');
	while ($value > 0x7F) {
		line_print(decbin($value));
		$value2 = ($value & 0x7F);
		line_print(decbin($value2));
		$val.=chr( $value2 | 0x80 );
		$value >>= 7;
	}
	$val.=chr($value);
	return $val;
}

$str = writeVInt($v);

// ascii 转数值
echo str_pad('', 32, "-", STR_PAD_LEFT).PHP_EOL;
function readVInt($str)
{
	$i = 0;
	$nextByte = ord($str[0]);
	line_print(decbin($nextByte));
	$val = $nextByte & 0x7F;
	line_print(decbin($val));
	for ($shift=7; ($nextByte & 0x80) != 0; $shift += 7) {
		$i++;
		$nextByte = ord($str[$i]);
		line_print(decbin($nextByte));
		$val |= ($nextByte & 0x7F) << $shift;
		line_print(decbin($val));
	}
	return $val;
}

$v = readVInt($str);
echo str_pad('', 32, "-", STR_PAD_LEFT).PHP_EOL;
line_print('v='.$v);