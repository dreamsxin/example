# PGO

See http://www.laruence.com/2015/06/19/3063.html

下载PHP7的源代码，第一遍编译PHP7，让它生成会产生profile数据的可执行文件：
```shell
make prof-gen
```

这时候 php 目录下会生成 `sapi/cli/php`、`sapi/cgi/php-cgi` 和 `sapi/fpm/php-fpm` 这个三个文件，供我们进行针对性的训练, 让我们开始训练 GCC：
```shell
sapi/cgi/php-cgi -T 100 /var/www/html/wordpress/index.php >/dev/null
```

让 php-cgi 跑 100 遍 wordpress 的首页，从而生成一些在这个过程中的profile信息。
一般来说网站访问量最大的就是首页了，所以我们只要对首页文件进行训练即可。 使用 LNMP 的环境还可以用 `sapi/fpm/php-fpm` 代替 `sapi/cgi/php-cgi`，获得更好的反馈。

值得说明的是，训练只能对一个网站程序有效，例如训练的 WordPress 只会对 WP 的性能进行适应性优化。如果服务器有多种网站程序，则可以自己写脚本用来训练从而获得通用性的 PGO 优化。

然后, 我们开始第二次编译PHP7：
```shell
make prof-clean
make prof-use && make install
```

好的, 就这么简单, PGO 编译完成了。