<?php

$server = new Phalcon\Websocket\Server(8080, Phalcon\Websocket\Server::WRITE_TEXT);
$server->on(Phalcon\Websocket\Server::ON_ACCEPT, function($server, $conn){
	echo 'Accept'.PHP_EOL;
	$conn->send('Hello world!');
});
$server->on(Phalcon\Websocket\Server::ON_CLOSE, function($server){
	echo 'Close'.PHP_EOL;
});
$server->on(Phalcon\Websocket\Server::ON_DATA, function($server, $conn, $data){
	echo 'Data '.$data.PHP_EOL;
	$conn->send('Hello world2!');
});
$server->on(Phalcon\Websocket\Server::ON_TICK, function($server){
	$data = 'Server Tick!';
	//echo 'Tick '.$data.PHP_EOL;
});
$server->run();