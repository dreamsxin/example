## Demo

```php
$request = json_encode($request);
$ch = curl_init($this->_url);
curl_setopt_array($ch, [
	CURLOPT_HEADER         => false,
	CURLOPT_POST           => true,
	CURLOPT_HTTPHEADER     => ['Expect:', 'Content-Type: text/json', 'Content-Length:'],
	CURLOPT_POSTFIELDS     => dechex(strlen($request)) . "\r\n" . $request . "\r\n0\r\n\r\n",
	CURLOPT_RETURNTRANSFER => true,
]);
$response = curl_exec($ch);
$code     = curl_getinfo($ch, CURLINFO_HTTP_CODE);
curl_close($ch);
return (200 === $code) ? json_decode($response) : $response;
```