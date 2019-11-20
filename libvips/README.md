# libvips

https://github.com/libvips/libvips
https://github.com/libvips/php-vips-ext

```shlel
sudo apt-get install gtk-doc-tools
sudo apt-get install gobject-introspection
```

## 编译

```shell
./autogen.sh
./configure
make -j4 && sudo make install
```

如果出现错误 `config.status: error: cannot find input file: `Makefile.in'` 执行下列命令生成：
```shell
aclocal
automake --add-missing
```

如果出现错误 `configure.ac:353: error: required file './ltmain.sh' not found` 执行下列命令生成：
```shell
libtoolize --version
libtoolize --copy --force --automake
```

```shell
php -d extension=vips.so vips.php
```
```php
#!/usr/bin/env php
<?php

$x = vips_image_new_from_file($argv[1])["out"];
$x = vips_call("invert", $x)["out"];
vips_image_write_to_file($x, $argv[2]);
EOF;
```