<?php
$total = 100000;
$start = time();
echo 'start insert '.date('Y-m-d H:i:s').EOL;
$btree = new Phalcon\Storage\Btree(__DIR__.'/cache/tree.db');
for ($i = 0; $i < $total; $i++) {
	$btree->set('key_'.$i, 'value'.$i);
	printf("progress: [%-50s] %d%%\r", str_repeat('#', $i%100), ($i * 100)/$total);
}

echo 'insert '.$i.' '.(time() - $start).' seconds';
