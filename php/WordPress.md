# WordPress

## 安装环境

```shell
sudo apt-get install apache2 libapache2-mod-php7.1
sudo apt-get install mariadb-server mariadb-client
sudo apt-get install php7.1 php7.1-mbstring php7.1-gd php7.1-intl php7.1-mysql php7.1-zip php7.1-curl
```

## 设置数据库

```shell
sudo mysql_secure_installation
```

```shell
sudo mysql -u root -p
```

```sql
CREATE DATABASE coin;
CREATE USER 'coin'@'localhost' IDENTIFIED BY 'user_password_here';
GRANT ALL ON coin.* TO 'coin'@'localhost' IDENTIFIED BY 'user_password_here' WITH GRANT OPTION;
FLUSH PRIVILEGES;
EXIT;
```

## 设置 PHP 上传文件参数

```shell
sudo vi /etc/php/7.1/apache2/php.ini
```

```ini
file_uploads = On
allow_url_fopen = On
memory_limit = 256M
upload_max_filesize = 100M
max_execution_time = 120
```

## 下载安装 WordPress

```shell
https://wordpress.org/latest.tar.gz
tar -zxvf latest.tar.gz
sudo chown -R www-data:www-data /var/www/html/wordpress/
sudo chmod -R 755 /var/www/html/wordpress/
```

## 配置 Apache2

```shell
sudo vi /etc/apache2/sites-available/wordpress.conf
```

```conf
<VirtualHost *:80>
     ServerAdmin admin@example.com
     DocumentRoot /var/www/html/wordpress/
     ServerName example.com
     ServerAlias www.example.com

     <Directory /var/www/html/wordpress/>
        Options +FollowSymlinks
        AllowOverride All
        Require all granted
     </Directory>

     ErrorLog ${APACHE_LOG_DIR}/error.log
     CustomLog ${APACHE_LOG_DIR}/access.log combined

</VirtualHost>
```

```shell
sudo a2ensite wordpress.conf
sudo a2enmod rewrite
sudo systemctl restart apache2.service
```