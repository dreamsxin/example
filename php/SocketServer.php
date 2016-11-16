<?php

$server = new Phalcon\Socket\Server('localhost', 5000);
$server->setOption(SOL_SOCKET, SO_REUSEADDR, 1);
$server->setEvent(Phalcon\Socket\Server::USE_SELECT);
$server->listen();
$server->run(
	function(Phalcon\Socket\Client $client){
		$client->write("Connecting...\r\n");
	},
	function(Phalcon\Socket\Client $client, $mssage){
		// Received
		echo $mssage.PHP_EOL;
		$client->write("Gets ".$mssage."\r\n");
	},
	function(Phalcon\Socket\Client $client){
		// Allow write
		// $client->write("---- Allow write ----\r\n");
	},
	function(Phalcon\Socket\Client $client){
		// Close
		echo 'Client '.$client->getSocketId().' disconnected'.PHP_EOL;
	},
	function(Phalcon\Socket\Client $client){
		// Error
		echo 'Client '.$client->getSocketId().' error'.PHP_EOL;
	},
	function(){
		// Timeout
		echo 'Timeout'.PHP_EOL;
	}
);
