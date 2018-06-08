

```php
$data_string = json_encode($data);
$ch = curl_init();
curl_setopt($ch, CURLOPT_URL, $url);
curl_setopt($ch, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1);
curl_setopt($ch, CURLOPT_CUSTOMREQUEST, "POST");
curl_setopt($ch, CURLOPT_POSTFIELDS, $data_string);
curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
curl_setopt($ch, CURLOPT_HTTPHEADER, array(
	'Content-Type: application/x-www-form-urlencoded',
	'Content-Length: ' . strlen($data_string))
);
$res = curl_exec($ch);
$res = file_get_contents($url, null, stream_context_create(array(
	'http' => array(
		'method' => 'POST',
		'header' => 'Content-Type: application/json' . "\r\n"
		. 'Content-Length: ' . strlen($data_string) . "\r\n",
		'content' => $data_string,
	),
)));
```