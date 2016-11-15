<?php

// telnet localhost 5000
$server = new Phalcon\Socket\Server('localhost', 5000);
$server->setOption(SOL_SOCKET, SO_REUSEADDR, 1);
$server->listen();
$server->setBlocking(FALSE);

$clients = $sockets = array();
$writes = array();
$except = array();

while(true){
	$reads = $sockets;
	$reads[] = $server->getSocket();
	$writes = $sockets;
	if (socket_select($reads, $writes, $except, NULL) !== FALSE) {
		if (in_array($server->getSocket(), $reads)) {
			$client = $server->accept();
			if($client){
				$client->write("Connecting...\r\n");
				$key = (int)$client->getSocket();
				$clients[$key] = $client;
				$sockets[$key] = $client->getSocket();
			}
		} else {
			// 可写的
			foreach($writes as $socket) {
				// socket_write($socket, "Hello\r\n").PHP_EOL;
			}
			// 可读的
			foreach($reads as $key => $socket) {
				if ($socket == $server->getSocket()) {
					continue;
				}
				$key = (int)$socket;
				$client = $clients[$key];
				$input = $client->read(1024);
				if ($input === '') { // 已关闭
					echo 'Client '.$key.' disconnected'.PHP_EOL;
					$client->close();
					unset($clients[$key]);
					unset($sockets[$key]);
				} else {
					echo $input.PHP_EOL;
					$client->write("Gets ".$input."\r\n");
				}
			}
		}
	}
}