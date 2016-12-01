<?php
$error_log = __DIR__."/error.log";
$desc = array(
    0 => array("pipe", "r"), // stdin
    1 => array("file", $error_log, "w"), // stdout
    2 => array("file", $error_log, "w")  // stderr
);
@unlink($error_log);
$pid_file = __DIR__.'/test.pid';
if (file_exists($pid_file)) {
  $signal = 15;
  $pid = file_get_contents($pid_file);
  $cmd = 'kill -%d %d';
  $comand = sprintf($cmd, $signal, $pid);
  $process = proc_open($comand, $desc, $pipes);
  if (!$process) {
    echo 'stop fail'.PHP_EOL;
    exit;
  }
  $status = proc_get_status($process);
  $running = $status['running'];
  if (!$running || !$pipes || (isset($pipes[2]) && fgets($pipes[2]) !== false)) {
    echo 'stop fail'.PHP_EOL;
    exit;
  }
  echo 'stop success';
  unlink($pid_file);
} else {
  $cmd = 'ping localhost';

  $process = proc_open($cmd, $desc, $pipes, NULL);
  if (!is_resource($process)) {
    echo 'start fail';
    exit;
  }
  $status = proc_get_status($process);
  $running = $status['running'];
  if (!$running || !$pipes) {
    echo 'start fail'.PHP_EOL;
    exit;
  }
  $pid = $status['pid'];
  var_dump($status);
  file_put_contents($pid_file, $pid);
  echo 'start success'.PHP_EOL;
  /*
  while ($running) {
      usleep(1000);
      $status = proc_get_status($process);
      var_dump($status);
      $running = $status['running'];
  }
  */
}
