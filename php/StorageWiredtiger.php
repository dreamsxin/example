<?php
$total = 100000;
$start = time();
echo 'start insert '.date('Y-m-d H:i:s').PHP_EOL;
$db = new Phalcon\Storage\Wiredtiger(__DIR__.'/cache/wiredtiger');
$db->create('table:test');
$cursor = $db->open('table:test');
for ($i = 0; $i < $total; $i++) {
	$cursor->set('key_'.$i, 'value'.$i);
	printf("progress: [%-50s] %d%%\r", str_repeat('#', $i%100), ($i * 100)/$total);
}

echo PHP_EOL.'set '.$i.' '.(time() - $start).' seconds'.PHP_EOL;
$start = time();
for ($i = 0; $i < $total; $i++) {
	$cursor->get('key_'.$i);
	printf("progress: [%-50s] %d%%\r", str_repeat('#', $i%100), ($i * 100)/$total);
}
echo PHP_EOL.'get '.$i.' '.(time() - $start).' seconds'.PHP_EOL;
