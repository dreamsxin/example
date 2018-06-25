<?php

$stdin = fopen('php://stdin', 'r');

$client = new Phalcon\Websocket\Client('127.0.0.1', 8080);
$client->on(Phalcon\Websocket\Client::ON_ACCEPT, function($client, $conn){
	echo 'Accept'.PHP_EOL;
	$conn->send('Hello Server!');
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
	echo 'Tick '.$data.PHP_EOL;
	$client->send($data);
});
$client->connect();