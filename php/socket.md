# 超时时间设置

<?php

$socket  = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);

$timeout = array('sec'=>0,'usec'=>300);
// 影响 send, sendmsg, connect
$ret = socket_set_option($socket, SOL_SOCKET, SO_SNDTIMEO,$timeout);
// 影响 recv, recvmsg, accept
$ret = socket_set_option($socket, SOL_SOCKET, SO_RCVTIMEO,$timeout);

var_dump(socket_get_option($socket, SOL_SOCKET, SO_RCVTIMEO));
$ret = socket_connect($socket, 'live.eotu.com', '80');
