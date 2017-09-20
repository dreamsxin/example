## PHP 刷新缓存立即输出

以下面这段代码并不会逐个输出，而是当浏览器筹够一定字节数进行统一输出，结果显而易见，10秒后一次性输出所有内容
```php
for($i=0;$i<10;$i++){
	echo $i.'</br>';
	sleep(1);
}
```

Apache：
```php
for($i=0;$i<10;$i++){
	echo $i.'</br>';
	sleep(1);
	ob_flush(); //关闭缓存
	flush();    //刷新缓存即立即输出了
}
```

Nginx：

`header('X-Accel-Buffering: no');`
 
实例代码

```php
header('Content-Type: text/event-stream'); // 以事件流的形式告知浏览器进行显示
header('Cache-Control: no-cache');         // 告知浏览器不进行缓存
header('X-Accel-Buffering: no');           // 关闭加速缓冲
for($i=0;$i<10;$i++){
    echo $i.'</br>';
    sleep(1);
    ob_flush();
    flush();
}
```