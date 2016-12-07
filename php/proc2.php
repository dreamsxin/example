<?php
date_default_timezone_set('PRC');
$desc = array(
    0 => array("pipe", "r"), // stdin
    1 => array("pipe", "w"), // stdout
    2 => array("pipe", "w")  // stderr
);

$cmd = 'ps h -olstart= -p %d';
$pid = 797;
$comand = sprintf($cmd, $pid);
$process = proc_open($comand, $desc, $pipes);
if (!$process) {
    echo 'run fail'.PHP_EOL;
    exit;
}
$status = proc_get_status($process);
$running = $status['running'];
if (!$running || !$pipes || (isset($pipes[2]) && fgets($pipes[2]) !== false)) {
    echo 'run fail'.PHP_EOL;
    exit;
}
while($ret = fgets($pipes[1])) {
    $starttime = $ret;
}

var_dump($starttime);
