<?php
date_default_timezone_set('PRC');
$desc = array(
    0 => array("pipe", "r"), // stdin
    1 => array("pipe", "w"), // stdout
    2 => array("pipe", "w")  // stderr
);

echo '输入命令，例如 ping localhost：'.PHP_EOL;
$cmd = stream_get_line(STDIN, 1024, "\n");
var_dump($cmd);
$process = proc_open($cmd, $desc, $pipes);
if (!$process) {
    echo 'run fail'.PHP_EOL;
    exit;
}
$status = proc_get_status($process);
$running = $status['running'];

if (!$running || !$pipes) {
    echo 'run fail'.PHP_EOL;
    exit;
}
if (isset($pipes[2])) {
    stream_set_blocking($pipes[2], 0);
    if (($ret = fgets($pipes[2])) !== false) {
        echo $ret.PHP_EOL;
        exit;
    }
}
if (isset($pipes[1]) && $ret = fgets($pipes[1])) {
    echo $ret.PHP_EOL;
}
//get the parent pid of the process we want to kill
$ppid = $status['pid'];
//use ps to get all the children of this process, and kill them
$pids = preg_split('/\s+/', `ps -o pid --no-heading --ppid $ppid`);
foreach($pids as $pid) {
    if(is_numeric($pid)) {
        echo "Killing $pid\n";
        posix_kill($pid, 9);
    }
}
proc_terminate($process);
proc_close($process);
exit;
while($status['running']) {
    echo '输出结果：'.PHP_EOL;
    if (isset($pipes[1]) && $ret = fgets($pipes[1])) {
        echo $ret.PHP_EOL;
    }
    $status = proc_get_status($process);
}
