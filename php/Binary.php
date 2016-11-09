<?php

class Binary {

	private $data;
	private $index;

	public function __construct($_data) {
		$this->data = $_data;
		$this->index = 0;
	}

	public function getIndex() {
		return $this->index;
	}

	public function setIndex($_index) {
		$this->index = $_index;
	}

	private function peekBool() {
		return (unpack('@' . $this->index . '/C', $this->data)[1] ? true : false);
	}

	private function peekByte() {
		return unpack('@' . $this->index . '/C', $this->data)[1];
	}

	private function peekMShort() {
		return unpack('@' . $this->index . '/S', $this->data)[1];
	}

	private function peekLShort() {
		return unpack('@' . $this->index . '/v', $this->data)[1];
	}

	private function peekBShort() {
		return unpack('@' . $this->index . '/n', $this->data)[1];
	}

	public function peekMInt() {
		return unpack('@' . $this->index . '/L', $this->data)[1];
	}

	public function peekLInt() {
		return unpack('@' . $this->index . '/V', $this->data)[1];
	}

	public function peekBInt() {
		return unpack('@' . $this->index . '/N', $this->data)[1];
	}

	public function peekFloat() {
		return unpack('@' . $this->index . '/f', $this->data)[1];
	}

	public function peekDouble() {
		return unpack('@' . $this->index . '/d', $this->data)[1];
	}

	public function peekString() {
		return unpack('@' . $this->index . '/Z*', $this->data)[1];
	}

	public function readBool() {
		$byte = $this->peekBool();
		$this->index += 1;
		return $byte;
	}

	public function readByte() {
		$byte = $this->peekByte();
		$this->index += 1;
		return $byte;
	}

	public function readMShort() {
		$short = $this->peekMShort();
		$this->index += 2;
		return $short;
	}

	public function readLShort() {
		$short = $this->peekLShort();
		$this->index += 2;
		return $short;
	}

	public function readBShort() {
		$short = $this->peekBShort();
		$this->index += 2;
		return $short;
	}

	public function readMInt() {
		$int = $this->peekMInt();
		$this->index += 4;
		return $int;
	}

	public function readLInt() {
		$int = $this->peekLInt();
		$this->index += 4;
		return $int;
	}

	public function readBInt() {
		$int = $this->peekBInt();
		$this->index += 4;
		return $int;
	}

	public function readFloat() {
		$float = $this->peekFloat();
		$this->data = substr($this->data, 8);
		$this->index += 8;
		return $float;
	}

	public function readDouble() {
		$double = $this->peekDouble();
		$this->index += 16;
		return $double;
	}

	public function readString() {
		$string = $this->peekString();
		$this->index += strlen($string) + 1;
		return $string;
	}

}
