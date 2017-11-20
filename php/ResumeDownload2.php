<?php
    
    $file_name_not_secure = $_GET["file_name1"]; //remember to remove '/'
    
    $file_name = str_replace("\r", "", $file_name_not_secure); //replace '\r' for security issue
    $file_name = str_replace("\n", "", $file_name_not_secure); //replace '\n' for security issue
    $file_name = str_replace("/", "", $file_name_not_secure);  //replace '/' for security issue
    
    $file_name_urlencoded = rawurlencode($file_name);
    
    $file_path = "/download/" . $file_name;
    
    if (!file_exists($file_path)) {
        header("HTTP/1.1 404 Not Found");
        return;
    }
    
    $file_size = filesize($file_path);
    
    $start = 0;
    $end = $file_size-1; //because the range spec is zero based.
    
    $dlFile = @fopen($file_path, 'rb');
    if (!$dlFile) {
        header ("HTTP/1.1 500 Internal server error");
        return;
    }
    
    $partialContent = false;
    if (isset($_SERVER['HTTP_RANGE'])) {
        $partialContent = true;
    
        preg_match('/bytes=(\d+)-(\d+)?/', $_SERVER['HTTP_RANGE'], $matches);  //note: bytes=-500 are not supported
        $start = intval($matches[1]);
        if (!empty($matches[2])) {
            $end = intval($matches[2]);
        }
    }
    
    if ($partialContent) {
        header('HTTP/1.1 206 Partial Content');
    } else {
        header('HTTP/1.1 200 OK');
    }
    
    //control proxy not cache this file
    header('Pragma public');
    header('Expires: 0');
    header('Cache-Control: must-revalidate, post-check=0, pre-check=0');
    header('Cache-Control: private', false);
    
    $time  = date('r', filemtime($file_path));  //use RFC 2822 format, Example: Thu, 21 Dec 2000 16:01:07 +0200
    
    header("Last-Modified: $time");
    header('Content-Type: audio/x-wav');
    header('Accept-Ranges: bytes');  //tell http client that the site support Range header
    
    header("Content-Range: bytes {$start}-{$end}/{$file_size}");
    
    if ($partialContent) {
        header('Content-Length: ' . ($end-$start) + 1);
    } else {
        header('Content-Length: ' . $file_size);
    }
    
    //if the filename contains UTF-8 characters
    $user_agent = $_SERVER['HTTP_USER_AGENT'];
    $pos_IE60 = strpos($user_agent, "MSIE 6.0");
    $pos_IE70 = strpos($user_agent, "MSIE 7.0");
    $pos_safari = strpos($user_agent, "Safari");
    if ( $pos_IE60 > 0 || $pos_IE70 > 0 ) {
        header('Content-Disposition: attachment; filename=' . $file_name_urlencoded . '');
    } else if ( $pos_safari > 0 ) {
        header('Content-Disposition: attachment; filename=' . $file_name . '');
    } else {
        header('Content-Disposition: attachment; filename*=UTF-8\'\'' . $file_name_urlencoded . '');
    }
    header('Content-Transfer-Encoding: binary');
    
    //output file content
    $cur = $start;
    fseek($dlFile, $start, 0);  //skip to the right position
    while(!feof($dlFile) && $cur <= $end && (connection_status() == 0)) {
        print fread($dlFile, min(1024 * 16, ($end - $cur) + 1));
        $cur += 1024 * 16;
    }
