# SplFileObject 的使用

```php
<?php

$file = new SplFileObject("data.csv");
$file->setFlags(SplFileObject::READ_CSV);
foreach ($file as $fields) {
	var_dump($fields);
}

$file->seek(1);
while (!$file->eof()){
	var_dump($file->current());
	$file->next();
}
```
