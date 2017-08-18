<?php

$db = new Phalcon\Storage\Leveldb('./testdb');
$total = 100000;
$start = time();
echo 'start insert '.date('Y-m-d H:i:s').PHP_EOL;
for ($i = 0; $i < $total; $i++) {
	$db->put('key_'.$i, 'value'.$i);
	printf("progress: [%-50s] %d%%\r", str_repeat('#', $i%100), ($i * 100)/$total);
}
echo PHP_EOL.'set '.$i.' '.(time() - $start).' seconds'.PHP_EOL;

$start = time();
for ($i = 0; $i < $total; $i++) {
	$db->get('key_'.$i);
	printf("progress: [%-50s] %d%%\r", str_repeat('#', $i%100), ($i * 100)/$total);
}
echo PHP_EOL.'get '.$i.' '.(time() - $start).' seconds'.PHP_EOL;
