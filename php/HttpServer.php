<?php

class App extends Phalcon\Application {
        public function handle($uri = NULL):Phalcon\Http\ResponseInterface{
                return new Phalcon\Http\Response('hello');
        }
}
// Phalcon\Debug::enable();
$app = new App;
$server = new Phalcon\Server\Http(array('host' => '127.0.0.1', 'port' => 8989));
$server->start($app);
