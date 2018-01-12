# PGO

See http://www.laruence.com/2015/06/19/3063.html

下载PHP7的源代码，第一遍编译PHP7，让它生成会产生profile数据的可执行文件：
```shell
make prof-gen
```

然后, 让我们开始训练 GCC：
```shell
sapi/cgi/php-cgi -T 100 /var/www/html/wordpress/index.php >/dev/null
```

也就是让 php-cgi 跑 100 遍 wordpress 的首页，从而生成一些在这个过程中的profile信息。

然后, 我们开始第二次编译PHP7：
```shell
 make prof-clean
make prof-use && make install
```

好的, 就这么简单, PGO 编译完成了。