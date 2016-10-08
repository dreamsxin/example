<?php

class Iqiyi {

	private $auth = array(
			'client_id' => '',
			'client_secret' => ''
	);

	private function getAccessToken() {
		$client = new \Phalcon\Http\Client\Adapter\Stream();
		$response = $client->get('https://openapi.iqiyi.com/api/iqiyi/authorize?' . http_build_query($this->auth));
		if ($response->getStatusCode() == 200) {
			$ret = $file_response->getJsonBody(TRUE);
			if ($ret['code'] == 'A00000') {
				return $ret['data']['access_token'];
			}
		}

		return FALSE;
	}

	public function fileStatus($file_id) {
		$access_token = $this->getAccessToken();

		$params = array(
			'access_token' => $access_token,
			'file_id' => $file_id
		);

		$client = new \Phalcon\Http\Client\Adapter\Stream();
		$response = $client->get('http://openapi.iqiyi.com/api/file/fullStatus?' . http_build_query($params));
		if ($response->getStatusCode() == 200) {
			$ret = $response->getJsonBody(TRUE);
			if ($ret['code'] == 'A00000') {
				$params = array(
					'access_token' => $access_token,
					'file_ids' => $file_id
				);

				$client = new \Phalcon\Http\Client\Adapter\Stream();
				$file_response = $client->get('http://openapi.iqiyi.com/api/file/videoListForExternal?' . http_build_query($params));

				if ($file_response->getStatusCode() == 200) {
					$file_ret = json_decode($file_response->getBody(), true);
					$ret['duration'] = $file_ret['data'][0]['duration'];
				}
			}
			return $ret;
		}
		return FALSE;
	}

	public function fileUpload($filepath) {
		if (!file_exists($filepath)) {
			return FALSE;
        }

		$access_token = $this->getAccessToken();

		$filesize = filesize($filepath);
		$filetype = pathinfo($filepath)["extension"];

		$arr = array(
			'filetype' => $filetype,
			'filesize' => $filesize,
			'access_token' => $access_token
		);
	
		$client = new \Phalcon\Http\Client\Adapter\Stream();
		$client->setHeaders($arr);
		$response = $client->get('http://upload.iqiyi.com/openupload');
		if ($response->getStatusCode() == 200) {
			$ret = $response->getJsonBody(TRUE);
			if ($ret['code'] == 'A00000') {
				$upload_url = $ret['data']['upload_url'];

				$data = [];
				$data['file_id'] = $ret['data']['file_id'];
				$data['file_size'] = $filesize;
				$data['range'] = 0 . '-' . ($filesize - 1);

				$client = new \Phalcon\Http\Client\Adapter\Stream();
				$client->setFiles(array('file' => $filepath));
				$file_response = $client->post($upload_url, $data);

				if ($file_response->getStatusCode() != 200) {
					return FALSE;
				}

				$file_ret = $file_response->getJsonBody(TRUE);
				$params = array(
					'range_finished' => TRUE,
					'file_id' => $file_ret['file_id'],
					'access_token' => $access_token
				);
				$client = new \Phalcon\Http\Client\Adapter\Stream();
				$response = $client->post('http://upload.iqiyi.com/uploadfinish?' . http_build_query($params));
				if ($response->getStatusCode() == 200) {
					$finish_ret = $response->getJsonBody(TRUE);
					if ($finish_ret['code'] == 'A00000') {
						$params = array(
							'file_id' => $file_ret['file_id'],
							'file_name' => 'your filename',
							'description' => 'your description',
							'access_token' => $access_token
						);
						$client = new \Phalcon\Http\Client\Adapter\Stream();
						$response = $client->get('http://openapi.iqiyi.com/api/file/info?' . http_build_query($params));
						if ($response->getStatusCode() == 200) {
							$meat_ret = $response->getJsonBody(TRUE);
							if ($meat_ret['code'] == 'A00000') {
								return TRUE;
							}
						}
					}
				}
			}
		}

		return FALSE;
	}

	public function fileDelete($file_id) {
		$access_token = $this->getAccessToken();

		$arr = array(
			'delete_type' => 0,
			'access_token' => $access_token,
			'file_ids' => $file_id
		);

		$client = new \Phalcon\Http\Client\Adapter\Stream();
		$response = $client->get('https://openapi.iqiyi.com/api/file/delete?' . http_build_query($arr));
		if ($response->getStatusCode() == 200) {
			$ret = json_decode($response->getBody(), true);
			if ($ret['code'] == 'A00000') {
				return TRUE;
			}
		}
		return FALSE;
	}
}
