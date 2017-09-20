
```php
echo readfile("php://filter/read=convert.base64-encode/resource=php://input");
echo readfile('php://filter/read=string.strip_tags/resource=php://input');
```