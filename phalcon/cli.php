<?php

$cliopts = new \Phalcon\Cli\Options('Users Manager');
$cliopts->add([
	'type' => \Phalcon\Cli\Options::TYPE_STRING,
	'name' => 'type',
	'desc' => "操作类型",
	'help' => "允许的值[admin、reset]",
	'required' => true,
]);
$cliopts->add([
	'type' => \Phalcon\Cli\Options::TYPE_INT,
	'name' => 'uid',
	'desc' => "用户ID",
	'required' => true,
]);

$values = $cliopts->parse();

if (!$values) {
	return;
}

// 创建console应用

$di = new \Phalcon\DI\FactoryDefault\CLI();
$config = new Phalcon\Config\Adapter\Php(__DIR__.DIRECTORY_SEPARATOR.'config.local.php');

$loader = new \Phalcon\Loader();
$loader->registerDirs(array(
	__DIR__ . '/tasks',
	$config->modelsDir,
	$config->libraryDir,
))->register();

$di->set('db', function () use ($config) {
    return new Phalcon\Db\Adapter\Pdo\Postgresql(array(
		'host' => $config->database->host,
		'port' => $config->database->port,
		'username' => $config->database->username,
		'password' => $config->database->password,
		'dbname' => $config->database->dbname
	));
}, true);

$console = new \Phalcon\CLI\Console();
$console->setDI($di);

/**
 * 处理console应用参数
 */
$arguments = array(
    'task' => \Phalcon\Arr::get($values, 'type'),
    'action' => \Phalcon\Arr::get($values, 'action'),
    'uid' => \Phalcon\Arr::get($values, 'uid'),
);

try {
    // 处理参数
    $console->handle($arguments);
} catch (\Phalcon\Exception $e) {
	echo Phalcon\Cli\Color::error($e->getMessage());
    exit(255);
}
