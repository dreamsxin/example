<?php

/**
 * A download resume class
 * 
 * Client request header example:
 * Range: bytes=0-199
 * 
 * Sever usage:
 * $url = 'http://www.xxx.com/xxx.zip';
 * $rd = new ResumeDownload($url);
 * $rd->download();
 *
 * @author aiddroid
 * @date 2013-11-22
 * @encoding UTF-8
 */
class ResumeDownload {

    //file path
    private $filePath;
    //file name
    private $fileName;
    //file handler
    private $fileHandler;
    //the start of download
    private $rangeStart;
    //the end of download
    private $rangeEnd;
    //file size
    private $fileSize;
    private $log;

    /**
     * construction
     * @param type $filePath file path
     * @return type
     */
    function __construct($filePath, $log = false) {
		$this->log = $log;
		$this->filePath = $filePath;
		//check if file exists and readable
		if (!is_file($this->filePath) || !($this->fileHandler = fopen($this->filePath, 'rb'))) {
			return $this->sendError();
		}
		$this->fileName = basename($this->filePath);
		$this->fileSize = filesize($this->filePath);

		//get range value from http header
		$this->initDownloadRange();
    }

    /**
     * resume download
     */
    public function download() {
		$this->sendHeader();
		$this->sendContent();
    }

    /*
     * get range start and end from http header
     */

    private function initDownloadRange() {
		preg_match("/^bytes=(\d?)-(\d*?)$/i", $_SERVER['HTTP_RANGE'], $match);
		$this->rangeStart = intval($match[1]);
		$this->rangeEnd = intval($match[2]);
		//log
		$this->log("$this->rangeStart $this->rangeEnd $this->fileSize", __FUNCTION__);

		//check range value
		if ($this->rangeStart < 0 || $this->rangeStart >= $this->fileSize || (!empty($this->rangeEnd) && $this->rangeEnd < $this->rangeStart) || $this->rangeEnd >= $this->fileSize) {
			return $this->sendError('HTTP/1.1 416 Requested Range Not Satisfiable');
		}
    }

    /*
     * send output header
     */

    private function sendHeader() {
		//clear php errors or warnings
		ob_clean();
		header('Content-Length: ' . ($this->rangeEnd - $this->rangeStart));

		if ($this->rangeStart > 0) {
			header('HTTP/1.1 206 Partial Content');
			//Content-Range: bytes 21010-47021/47022
			header('Content-Range: bytes ' . $this->rangeStart . '-' . $this->rangeEnd . '/' . $this->fileSize);
		} else {
			header('Accept-Ranges: bytes');
		}

		header('Content-Type: application/octet-stream');
		//for filename
		header('Content-Disposition: attachment;filename=' . $this->fileName);
    }

    /**
     * send real data
     */
    private function sendContent() {
	//seek file point to read
	fseek($this->fileHandler, $this->rangeStart);
	//read file as range
	if (!empty($this->rangeEnd)) {
	    //Range: bytes=0-0 -> read the first byte
	    $content = fread($this->fileHandler, ($this->rangeEnd - $this->rangeStart + 1));
	    echo $content;
	    return;
	}
	//file passthrough?
	fpassthru($this->fileHandler);
    }

    /**
     * send error
     */
    private function sendError($error) {
	header($error ? $error : 'HTTP/1.0 404 Not Found');
    }

    /**
     * log
     * @param type $content
     * @param type $logFile
     */
    private function log($content, $logFile = '') {
	if ($this->log) {
	    $logFile = empty($logFile) ? __FUNCTION__ : $logFile;
	    $logFile = __CLASS__ . '.' . $logFile;
	    error_log(date('Y-m-d H:i:s') . " $content \n", 3, $logFile . '.' . date('Ymd') . '.log');
	}
    }

}
