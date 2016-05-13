# RTMP 处理流程

RTMPSession::onData ->
RTMPSession::buildPacket && RTMPSession::receive -> RTMPSession::kill(REJECTED_DEATH);
FlashStream::process -> 
FlashMainStream::messageHandler: {"connect", "setPeerInfo", "createStream", "deleteStream"} ->
