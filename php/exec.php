<?php

$error_log = __DIR__.'/error.log';
$pid_file = __DIR__.'/test.pid';
if (file_exists($pid_file)) {
  $cmd = 'kill -QUIT `cat '.$pid_file.'`';
  $comand = sprintf("%s > %s 2>&1", $cmd, $error_log);
  exec($comand);
  $command = 'ps -p `cat '.$pid_file.'`';
  exec($command, $output);
  if (!$output || !isset($output[1])) {
    echo 'stop success';
    unlink($pid_file);
  } else {
    echo 'stop fail';
  }
} else {
  // /usr/local/bin/ffmpeg -i rtmp://localhost/live/$name -i rtmp://localhost/sound/$name -c:v copy -c:a copy -f flv rtmp://localhost/single/$name;
  $cmd = 'ping localhost';
  $comand = sprintf("%s > %s 2>&1 & echo $! > %s", $cmd, $error_log, $pid_file);
  exec($comand);

  $command = 'ps -p `cat '.$pid_file.'`';
  exec($command, $output);
  if (!$output || !isset($output[1])) {
    echo 'start fail';
    unlink($pid_file);
  } else {
    echo 'start success';
  }
}
