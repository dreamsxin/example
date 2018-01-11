<?php

function tree() {
	echo __FUNCTION__.PHP_EOL;
	sleep(1);
}

function two() {
	echo __FUNCTION__.PHP_EOL;
	tree();
}

function one() {
	echo __FUNCTION__.PHP_EOL;
	two();
}

Phalcon\Xhprof::enable(Phalcon\Xhprof::FLAG_MEMORY | Phalcon\Xhprof::FLAG_CPU);

one();

$data = Phalcon\Xhprof::disable();

$times = Phalcon\Arr::path($data, '*.time');
array_multisort($times, SORT_DESC, SORT_NUMERIC, $data);
var_dump($data);