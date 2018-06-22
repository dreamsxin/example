<?php

$server = new Phalcon\Socket\Server('localhost', 6000);
$server->setMaxChildren(2);
$server->setOption(Phalcon\Socket::SOL_SOCKET, SO_REUSEADDR, 1);
$server->setOption(Phalcon\Socket::SOL_SOCKET, SO_REUSEPORT, 1);
$server->setOption(Phalcon\Socket::SOL_TCP, Phalcon\Socket::TCP_NODELAY, 1);
$server->setOption(Phalcon\Socket::SOL_TCP, Phalcon\Socket::TCP_QUICKACK, 1);

$server->run(
	function(Phalcon\Socket\Client $client){
		//echo "Connecting...".count($this->getClients()).PHP_EOL;
	},
	function(Phalcon\Socket\Client $client, $mssage){
		// Received
		echo 'Client Received'.PHP_EOL;
		//echo $mssage.PHP_EOL;
	},
	function(Phalcon\Socket\Client $client){
		// Allow write
		echo 'Client write'.PHP_EOL;
		$client->write("HTTP/1.0 200 OK\r\nServer: webserver\r\nContent-Type: text/html\r\nConnection: close\r\n\r\nHello World");
		return FALSE;
	},
	function(Phalcon\Socket\Client $client){
		// Close
		echo 'Client Close'.PHP_EOL;
		//echo 'Client '.$client->getSocket().' disconnected'.PHP_EOL;
	},
	function(Phalcon\Socket\Client $client){
		// Error
		echo 'Client '.$client->getSocket().' error'.PHP_EOL;
	},
	function(){
		// Timeout
		echo 'Timeout'.PHP_EOL;
	}
);