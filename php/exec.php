<?php

$error_log = __DIR__.'/error.log';
$pid_file = __DIR__.'/test.pid';
if (file_exists($pid_file)) {
  $cmd = 'kill -QUIT `cat '.$pid_file.'`';
  $comand = sprintf("%s > %s 2>&1", $cmd, $error_log);
  exec($comand);
  unlink($pid_file);
} else {
  $cmd = 'ping localhost';
  $comand = sprintf("%s > %s 2>&1 & echo $! > %s", $cmd, $error_log, $pid_file);
  exec($comand);
}
