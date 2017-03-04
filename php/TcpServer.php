<?php

class Server extends Phalcon\Server {
        public function onConnect(int $fd) {
                echo 'onConnect '.$fd.PHP_EOL;
        }
        public function onReceive(int $fd, string $data){
                return FALSE; // will close
        }
        public function onSend(int $fd) {
                echo 'onSend '.$fd.PHP_EOL;
        }
        public function onClose(int $fd) {
                echo 'onClose '.$fd.PHP_EOL;
        }
}
//Phalcon\Debug::enable();
$server = new Server(array('host' => '127.0.0.1', 'port' => 8989));
$server->start();
