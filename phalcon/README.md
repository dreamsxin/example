# 系统环境

```shell
sudo apt-get install git vim build-essential libpcre3-dev
# 安装 add-apt-repository 命令
sudo apt-get install python-software-properties
```

## 查看系统版本号

```shell
uname -a
cat /etc/issue
```

## 修改编码

编辑文件 `/etc/default/locale`
```shell
LANG="zh_CN.UTF-8"
LANGUAGE="zh_CN:zh"
LC_ALL="zh_CN.UTF-8"
```

运行
```shell
locale-gen zh_CN.UTF-8
locale -a
```

## 安装 nginx

See https://github.com/dreamsxin/example/tree/master/nginx

```shell
sudo add-apt-repository ppa:nginx/stable
sudo apt-get update
sudo apt-get install nginx
```

## 安装 php-fpm

See https://github.com/dreamsxin/example/tree/master/nginx

```shell
sudo add-apt-repository ppa:ondrej/php
sudo apt-get update

sudo apt-get install php5.6-fpm php5.6-cli php5.6-dev php5.6-curl php5.6-pgsql php5.6-mcrypt php5.6-mbstring
// 或
sudo apt-get install php7.1-fpm php7.1-cli php7.1-dev php7.1-curl php7.1-pgsql php7.1-mcrypt php7.1-mbstring
```

### 配置 php-fpm

编辑 `/etc/php/5.6/fpm/pool.d/www.conf` 或 `/etc/php/7.1/fpm/pool.d/www.conf`

设置 listen，默认配置是使用 unix sock，可以更改为 ip:port。
```conf
listen = /var/run/php5-fpm.sock
```

### 开启php-fpm日志报告

编辑 `/etc/php/5.6/fpm/php.ini` 或 `/etc/php/7.1/fpm/php.ini`

```conf
error_reporting = E_ALL
display_errors = On
# 追加
php_flag[display_errors] = on
```

### 查看php运行进程

```shell
ps -waux | grep php
```

### 关闭重启php5进程
```shell
sudo service php5.6-fpm stop
# or
sudo /etc/init.d/php5.6-fpm stop

sudo service php5.6-fpm start
sudo service php5.6-fpm restart
sudo service php5.6-fpm status
```

## 安装 imagick 扩展

```shell
sudo apt-get install libmagickwand-dev imagemagick
```

### 编译安装
```shell
git clone https://github.com/mkoppanen/imagick.git
cd imagick
phpize
./configure
make -j4 && sudo make install
```
或者
```shell
sudo pecl install imagick
```

## 安装 phalcon 扩展

See https://github.com/dreamsxin/cphalcon/wiki/%E6%BA%90%E7%A0%81%E7%BC%96%E8%AF%91%E5%AE%89%E8%A3%85%EF%BC%88Linux%EF%BC%89

```shell
git clone https://github.com/dreamsxin/cphalcon.git
cd cphalcon/ext
# 如果是 PHP7 下载 Phalcon7
git clone https://github.com/dreamsxin/cphalcon7.git
cd cphalcon7/ext

phpize
./configure
make -j4 && sudo make install
```

编辑文件 `/etc/php/5.6/mods-available/pdo_pgsql.ini` 或 `/etc/php/7.1/mods-available/pdo_pgsql.ini`，增加一行
```ini
extension=phalcon.so
```

也可以在 `mods-available`，新增文件 `phalcon.ini`：
```ini
extension=phalcon.so
```

并使扩展生效：
```shell
sudo ln -s /etc/php/5.6/mods-available/phalcon.ini /etc/php/5.6/cli/conf.d/phalcon.ini
sudo ln -s /etc/php/5.6/mods-available/phalcon.ini /etc/php/5.6/fpm/conf.d/phalcon.ini
# 或
sudo ln -s /etc/php/7.1/mods-available/phalcon.ini /etc/php/7.1/cli/conf.d/phalcon.ini
sudo ln -s /etc/php/7.1/mods-available/phalcon.ini /etc/php/7.1/fpm/conf.d/phalcon.ini
```

## nginx 部署网站

在目录 `/etc/nginx/sites-available/` 下，包含所有站点配置文件，站点开启之后，会软链接到目录 `/etc/nginx/sites-enabled/` 下。

### 创建站点目录结构

可以直接将示例将代码 `mvc` 目录拷贝到 `/var/www/html` 目录

### 启用站点

```shell
sudo ln -s /etc/nginx/sites-available/default /etc/nginx/sites-enabled/default
sudo service nginx restart
```

### 关闭站点

```shell
sudo rm /etc/nginx/sites-enabled/default
sudo service nginx restart
```

### 配置 nginx

修改 nginx 配置 `/etc/nginx/sites-available/default`

```shell
server {
    listen   80;
    server_name localhost;

    # 默认访问文件
    index index.php index.html index.htm;

    # 设置网站访问根目录，根据实际站点目录设置
    set $root_path '/var/www/html/mvc/public';
    root $root_path;
    # http 请求数据最大值
    client_max_body_size 50M;
    # 临时存放地址
    client_body_temp_path /tmp;

    # 按顺序检查文件是否存在，返回第一个找到的文件。结尾的斜线表示为文件夹。如果所有的文件都找不到，会进行一个内部重定向到最后一个参数。
    try_files $uri $uri/ @rewrite;

    location @rewrite {
        rewrite ^/(.*)$ /index.php?_url=/$1;
    }

    # 所有php结尾的请求都会，发送到php-fpm进行处理
    location ~ \.php {
	# 跟 php-fpm 配置文件里的 `listen` 项保持一致
        fastcgi_pass unix:/run/php-fpm/php-fpm.sock;
        fastcgi_index /index.php;

        include /etc/nginx/fastcgi_params;

        fastcgi_split_path_info    ^(.+\.php)(/.+)$;
        fastcgi_param PATH_INFO    $fastcgi_path_info;
        fastcgi_param PATH_TRANSLATED $document_root$fastcgi_path_info;
        fastcgi_param SCRIPT_FILENAME $document_root$fastcgi_script_name;
    }

    # 所有静态文件请求都会，从设定的root目录读取，这里跟之前的root文件一致，所以可以不设置，不过也可以用来限制带宽和并发数
    location ~* ^/(css|img|js|flv|swf|download)/(.+)$ {
        root $root_path;
    }

    location ~ /\.ht {
        deny all;
    }
}
```

重启

```shell
sudo service nginx restart
# or
sudo /etc/init.d/nginx restart
```

在浏览器中打开网址 `http://localhost`，会看到 `Hello world!` 字样

## 安装 PostgreSQL

See https://github.com/dreamsxin/example/tree/master/postgresql

配置文件 `/etc/postgresql/9.5/main/postgresql.conf`

```conf
listen_addresses = 'localhost,192.168.1.108'
port = 5432
max_connections = 100
```

配置文件 `/etc/postgresql/9.5/main/pg_hba.conf`

## 持久连接

```php
	$di->set('db', function () use ($config) {
		return new \Phalcon\Db\Adapter\Pdo\Postgresql(array(
			'host' => $config->database->host,
			'username' => $config->database->username,
			'password' => $config->database->password,
			'dbname' => $config->database->dbname,
			'port' => $config->database->port,
			'options' => array(
				PDO::ATTR_STRINGIFY_FETCHES => false,
				PDO::ATTR_PERSISTENT => true
			)
		));
	}, true);
```

## 读写分离

```php
	$di->set('modelsManager', function(){
		$manager = new Phalcon\Mvc\Model\Manager();
		$manager->setDefaultReadConnectionService('readdb');
		$manager->setDefaultWriteConnectionService('writedb');
		return $manager;
	}, TRUE);
```

## 分表与元数据缓存

```php
class Robots extends Phalcon\Mvc\Model {

	public function getSource() {
		return 'robots';
	}

	public function getMetadataCachekey() {
		return 'robots';
	}
}

class Robots2 extends Phalcon\Mvc\Model {

	public function getSource() {
		return 'robots_2';
	}

	public function getMetadataCachekey() {
		return 'robots';
	}
}
``

