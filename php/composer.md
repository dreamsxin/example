```shell
composer config -g repo.packagist composer https://packagist.phpcomposer.com  
composer global require "squizlabs/php_codesniffer=*"
phpcs --config-set installed_paths /xxx/PHPCompatibility
```

```shell
ln -s ~/.config/composer/vendor/bin/phpcs /usr/local/bin/phpcs
ln -s ~/.config/composer/vendor/bin/phpcbf /usr/local/bin/phpcbf
```

```shell
phpcs --standard=PHPCompatibility
```