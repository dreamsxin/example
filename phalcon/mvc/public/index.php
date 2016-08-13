<?php

try {
	$loader = new \Phalcon\Loader();
	$loader->registerDirs(array(
		__DIR__.'/../app/controllers/'
	))->register();

	$di = new \Phalcon\DI\FactoryDefault();

	$di->set('view', function () {
		$view = new \Phalcon\Mvc\View();
		$view->setBasePath(__DIR__.'/../app/views/');
		return $view;
	}, true);

	$application = new \Phalcon\Mvc\Application($di);
	echo $application->handle()->getContent();
} catch (\Exception $e) {
	echo $e->getMessage();
}
