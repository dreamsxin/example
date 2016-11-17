<?php

$server = new Phalcon\Socket\Server('127.0.0.1', 5000);
$server->setEvent(Phalcon\Socket\Server::USE_SELECT);
$server->run(
	function(Phalcon\Socket\Client $client){
		echo "Connecting...".count($this->getClients()).PHP_EOL;
	},
	function(Phalcon\Socket\Client $client, $mssage){
		// Received
		echo '接收到消息：'.$mssage.PHP_EOL;
		// 向所有连接转发数据
		echo 'Client write'.PHP_EOL;
		foreach($this->getClients() as $otherclient) {
			if ($client != $otherclient) {
				$otherclient->write($mssage);
			}
		}
	},
	function(Phalcon\Socket\Client $client){
		// Write
	},
	function(Phalcon\Socket\Client $client){
		// Close
		echo 'Client '.$client->getSocketId().' disconnected'.PHP_EOL;
		foreach($this->getClients() as $otherclient) {
			if ($client != $otherclient) {
				$otherclient->write($client->getSocketID().'断开连接');
			}
		}
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
