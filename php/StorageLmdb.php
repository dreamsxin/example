<?php

$lmdb = new Phalcon\Storage\Lmdb('./testdb', NULL, NULL, 1099511627776);
$total = 1000000;
$start = time();
$lmdb->begin();
echo 'start insert '.date('Y-m-d H:i:s').PHP_EOL;
for ($i = 0; $i < $total; $i++) {
	$lmdb->put('key_'.$i, 'value'.$i);
	//printf("progress: [%-50s] %d%%\r", str_repeat('#', $i%100), ($i * 100)/$total);
}
$lmdb->commit();
echo PHP_EOL.'set '.$i.' '.(time() - $start).' seconds'.PHP_EOL;

$start = time();
$lmdb->begin();
for ($i = 0; $i < $total; $i++) {
	$lmdb->get('key_'.$i);
	//printf("progress: [%-50s] %d%%\r", str_repeat('#', $i%100), ($i * 100)/$total);
}
$lmdb->commit();
echo PHP_EOL.'get '.$i.' '.(time() - $start).' seconds'.PHP_EOL;