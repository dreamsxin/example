<?php

$client = new Phalcon\Socket\Client('localhost', 6000);
if ($client->connect()) {
	$client->write('Hello world!');
	while($ret = $client->read(1024)) {
		echo $ret;
	}
} else {
	echo 'connect fail'.PHP_EOL;
}