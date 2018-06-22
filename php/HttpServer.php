<?php

class App extends Phalcon\Application {
	public function handle($data = NULL):Phalcon\Http\ResponseInterface{
		$response = new Phalcon\Http\Response('hello');
		$response->setHeader("Content-Type", "text/html");
		if ($data) {
			$response->setContent(json_encode($data));
		}
		return $response;
	}
}
// Phalcon\Debug::enable();
$app = new App;
$server = new Phalcon\Server\Http(array('host' => '127.0.0.1', 'port' => 8989));
$server->start($app);
