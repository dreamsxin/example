<?php

$client = new Phalcon\Websocket\Client('127.0.0.1', 8080, NULL, Phalcon\Websocket\Server::WRITE_TEXT);
$client->on(Phalcon\Websocket\Client::ON_ACCEPT, function($client, $conn){
	echo 'Accept'.PHP_EOL;
	$conn->send('Hello Server!');
	$pid = pcntl_fork();

	$stdin = fopen('php://stdin', 'r');
	while($data = fgets($stdin,1024)) {
		print "You typed: $s";
		$conn->send($data);
	}
});
$client->on(Phalcon\Websocket\Client::ON_CLOSE, function(){
	echo 'Close'.PHP_EOL;
});
$client->on(Phalcon\Websocket\Client::ON_DATA, function($client, $conn, $data){
	echo 'Data '.$data.PHP_EOL;
	//$conn->send($data);
});
$client->on(Phalcon\Websocket\Client::ON_TICK, function($client){
	$data = 'Hello Tick!';
	echo $data.PHP_EOL;
	//$client->send($data, Phalcon\Websocket\Client::WRITE_BINARY);
});
$client->connect();