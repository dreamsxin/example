<?php

$lmdb = new Phalcon\Storage\Libmdbx('./testdb');
$total = 999999;
$start = time();
$lmdb->begin();
echo 'start insert '.date('Y-m-d H:i:s').PHP_EOL;
for ($i = 0; $i < $total; $i++) {
	$lmdb->put('key_'.$i, 'value'.$i);
	printf("progress: [%-50s] %d%%\r", str_repeat('#', $i%100), ($i * 100)/$total);
}
echo PHP_EOL.'set '.$i.' '.(time() - $start).' seconds'.PHP_EOL;
$lmdb->commit();

$start = time();
$lmdb->begin();
echo 'start get '.date('Y-m-d H:i:s').PHP_EOL;
for ($i = 0; $i < $total; $i++) {
	$lmdb->get('key_'.$i);
	printf("progress: [%-50s] %d%%\r", str_repeat('#', $i%100), ($i * 100)/$total);
}
echo PHP_EOL.'get '.$i.' '.(time() - $start).' seconds'.PHP_EOL;
$lmdb->commit();