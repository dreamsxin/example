# Openstreet Map

## 安装配置

系统环境： ubuntu 16.04 64位

因为要在离线环境(局域网)下使用地图服务，所以需要搭建一个 OpenStreetMap 服务。

* 下载离线数据包

在 http://download.geofabrik.de/asia/china.html 下载离线数据包。
```shell
cd /var/www
wget http://download.geofabrik.de/asia/china-latest.osm.pbf
```

安装基础环境
```shell
apt update -y
apt upgrade -y
apt-get install -y software-properties-common python-software-properties
apt-get install -y libboost-dev libboost-filesystem-dev libboost-program-options-dev libboost-python-dev libboost-regex-dev libboost-system-dev libboost-thread-dev
apt-get install -y subversion git-core tar unzip wget bzip2 build-essential autoconf libtool libxml2-dev libgeos-dev libpq-dev libbz2-dev munin-node munin libprotobuf-c0-dev protobuf-c-compiler libfreetype6-dev libpng12-dev libtiff5-dev libicu-dev libgdal-dev libcairo-dev libcairomm-1.0-dev apache2 apache2-dev libagg-dev liblua5.2-dev ttf-unifont
apt-get install -y autoconf apache2-dev libtool libxml2-dev libbz2-dev libgeos-dev libgeos++-dev libproj-dev gdal-bin libgdal1-dev mapnik-utils python-mapnik libmapnik-dev
```

* 安装 postgresql
```shell
apt-get install -y postgresql-9.5-postgis-2.2 postgresql-contrib postgresql-server-dev-9.5
apt-get install -y osm2pgsql
```

* 安装 mod_tile
```shell
mkdir ~/src
cd src/
git clone https://github.com/openstreetmap/mod_tile/
cd mod_tile
./autogen.sh
./configure
make
make install
make install-mod_tile
ldconfig
```

* 安装 Mapnik 样式表
```shell
cd /usr/local/src && svn co http://svn.openstreetmap.org/applications/rendering/mapnik mapnik-style
cd /usr/local/src/mapnik-style && ./get-coastlines.sh /usr/local/share
```

* 配置服务
```shell
cd /usr/local/src/mapnik-style/inc && cp fontset-settings.xml.inc.template fontset-settings.xml.inc

wget https://raw.githubusercontent.com/ncareol/osm-tiles-docker/master/datasource-settings.sed -O /tmp/datasource-settings.sed

cd /usr/local/src/mapnik-style/inc && sed --file /tmp/datasource-settings.sed  datasource-settings.xml.inc.template > datasource-settings.xml.inc


wget https://raw.githubusercontent.com/ncareol/osm-tiles-docker/master/settings.sed -O /tmp/settings.sed
cd /usr/local/src/mapnik-style/inc && sed --file /tmp/settings.sed  settings.xml.inc.template > settings.xml.inc

wget https://raw.githubusercontent.com/ncareol/osm-tiles-docker/master/renderd.conf.sed -O /tmp/renderd.conf.sed
cd /usr/local/etc && sed --file /tmp/renderd.conf.sed --in-place renderd.conf

mkdir /var/run/renderd && chown www-data: /var/run/renderd
mkdir /var/lib/mod_tile && chown www-data /var/lib/mod_tile

wget https://raw.githubusercontent.com/ncareol/osm-tiles-docker/master/index.html -O /var/www/html/index.html

wget https://raw.githubusercontent.com/ncareol/osm-tiles-docker/master/mod_tile.load -O /etc/apache2/mods-available/mod_tile.load
wget https://raw.githubusercontent.com/ncareol/osm-tiles-docker/master/mod_tile.conf -O /etc/apache2/mods-available/mod_tile.conf
a2enmod mod_tile

sed -i -e 's/local   all             all                                     peer/local gis www-data peer/' /etc/postgresql/9.5/main/pg_hba.conf

wget https://raw.githubusercontent.com/ncareol/osm-tiles-docker/master/postgresql.conf.sed -O /tmp/postgresql.conf.sed
sed --file /tmp/postgresql.conf.sed --in-place /etc/postgresql/9.5/main/postgresql.conf

wget https://gist.githubusercontent.com/xdtianyu/ea9a57f4824eede676e6036c299f04e0/raw/renderd.service -O /etc/systemd/system/renderd.service
```

* 初始化数据库
```shell
chown -R postgres /var/lib/postgresql/
sudo -u postgres -i /usr/lib/postgresql/9.5/bin/initdb --pgdata /var/lib/postgresql/9.5/main
ln -s /etc/ssl/certs/ssl-cert-snakeoil.pem /var/lib/postgresql/9.5/main/server.crt
ln -s /etc/ssl/private/ssl-cert-snakeoil.key /var/lib/postgresql/9.5/main/server.key
systemctl start postgresql
sudo -H -u postgres bash -c "createuser -s www-data"

dbname=gis

sudo -H -u postgres bash -c "createdb -O www-data $dbname"
sudo -H -u www-data bash -c "psql -d $dbname -f /usr/share/postgresql/9.5/contrib/postgis-2.2/postgis.sql"
sudo -H -u www-data bash -c "psql -d $dbname -c 'CREATE EXTENSION HSTORE;'"
sudo -H -u www-data bash -c "psql -d $dbname -c 'ALTER TABLE geometry_columns OWNER TO "www-data"; ALTER TABLE spatial_ref_sys OWNER TO "www-data";'"
sudo -H -u www-data bash -c "psql -d $dbname -f /usr/share/postgresql/9.5/contrib/postgis-2.2/spatial_ref_sys.sql"
```

* 导入数据库
注意这个过程可能需要至少20分钟时间，产生的数据库约占 8G 空间。
```shell
sudo -H -u www-data bash -c "osm2pgsql --slim --hstore --cache 2000 --database gis --number-processes 8 /var/www/china-latest.osm.pbf"
```

修改 `/usr/local/etc/renderd.conf` 配置
```conf
[renderd]
socketname=/var/run/renderd/renderd.sock
num_threads=4
tile_dir=/var/lib/mod_tile
stats_file=/var/run/renderd/renderd.stats

[mapnik]
plugins_dir=/usr/lib/mapnik/3.0/input
font_dir=/usr/share/fonts
font_dir_recurse=true

[default]
font_dir=/usr/share/fonts
font_dir_recurse=true
URI=/osm_tiles/
TILEDIR=/var/lib/mod_tile
XML=/opt/carto/style.xml
HOST=localhost
TILESIZE=256
```

* 启动服务

添加`/etc/systemd/system/renderd.service`

文件，内容如下
```conf
[Service]
ExecStart=/usr/local/bin/renderd --config /usr/local/etc/renderd.conf --foreground yes
Restart=always
StandardOutput=syslog
StandardError=syslog
SyslogIdentifier=renderd
User=www-data
Group=www-data
Environment=SYSTEM_FONTS=/usr/share/fonts
[Install]
WantedBy=multi-user.target
systemctl restart renderd
systemctl restart apache2
```

可以使用如下命令测试 renderd 服务
```shell
sudo -H -u www-data bash -c "export SYSTEM_FONTS=/usr/share/fonts && /usr/local/bin/renderd --config /usr/local/etc/renderd.conf --foreground yes"
```
字体问题
```shell
apt install fonts-wqy-microhei fonts-noto-cjk fonts-noto-hinted fonts-noto-unhinted ttf-unifont
ls /usr/share/fonts/truetype/
export SYSTEM_FONTS=/usr/share/fonts
```

预加载数据

注意这个过程可能需要几个或几十个小时，产生的文件可能会占用 10G-30G 空间。
```shell
sudo -H -u www-data bash -c "export SYSTEM_FONTS=/usr/share/fonts && render_list --all --min-zoom 1 --max-zoom 18 --num-threads 4"
```

参考 https://github.com/ncareol/osm-tiles-docker

安装编译依赖
```shell
apt install -y autoconf libtool build-essential apache2-dev libmapnik-dev
```
安装 mod_tile
```shell
mkdir ~/src
cd src/
git clone https://github.com/openstreetmap/mod_tile/
cd mod_tile
./autogen.sh
./configure
make
export DESTDIR="$HOME/out" && make install
export DESTDIR="$HOME/out" && make install-mod_tile
ldconfig
```

打包生成的文件
```shell
tar czf out.tar.gz "$HOME/out"
```

安装部署依赖
```shell
apt install libmapnik3.0
```
将生成的 out.tar.gz 文件复制到目标服务器
```shell
tar xf out.tar.gz
cd out
cp -r usr /
ldconfig
```
运行
```shell
renderd --help
```
测试二进制执行环境是否正常。

```shell
apt-get update 
apt-get install -y sudo

// 安装基本依赖
sudo apt install -y libboost-all-dev git-core tar unzip wget bzip2 build-essential autoconf libtool libxml2-dev libgeos-dev libgeos++-dev libpq-dev libbz2-dev libproj-dev munin-node munin libprotobuf-c0-dev protobuf-c-compiler libfreetype6-dev libtiff5-dev libicu-dev libgdal-dev libcairo-dev libcairomm-1.0-dev apache2 apache2-dev libagg-dev liblua5.2-dev ttf-unifont lua5.1 liblua5.1-dev libgeotiff-epsg curl

// 安装postgresql 及 postgis （安装过程需要选择地区）
sudo apt-get install -y postgresql postgresql-contrib postgis postgresql-10-postgis-2.4 postgresql-10-postgis-scripts

// 开启postgresql服务
sudo service postgresql start

// 使用用户名 devsum 创建数据库，你可以修改为别的名字
sudo -u postgres -i
createuser devsum  # 如果询问是否设置为superuser，请选择是
createdb -E UTF8 -O devsum gis  # 创建名为gis的库

// 配置数据库
psql
\c gis
CREATE EXTENSION postgis;
CREATE EXTENSION hstore;
ALTER TABLE geometry_columns OWNER TO devsum;
ALTER TABLE spatial_ref_sys OWNER TO devsum;
\q
exit

// 添加账户
sudo useradd -m devsum

// 安装 osm2pgsql，用于将osm数据存入数据库
mkdir /home/devsum/src
cd /home/devsum/src
git clone git://github.com/openstreetmap/osm2pgsql.git
cd osm2pgsql

// 安装一些构建osm2pgsql所需的依赖
sudo apt install -y make cmake g++ libboost-dev libboost-system-dev libboost-filesystem-dev libexpat1-dev zlib1g-dev libbz2-dev libpq-dev libgeos-dev libgeos++-dev libproj-dev lua5.2 liblua5.2-dev

// 构建并安装 osm2pgsql
mkdir build && cd build
cmake ..
sudo make install

// 安装Mapnik 
sudo apt-get install -y autoconf apache2-dev libtool libxml2-dev libbz2-dev libgeos-dev libgeos++-dev libproj-dev gdal-bin libmapnik-dev mapnik-utils python-mapnik

// 安装 mod_tile 和 renderd
// 编译 mod_tile 源代码
cd /home/devsum/src
git clone -b switch2osm git://github.com/SomeoneElseOSM/mod_tile.git
cd mod_tile
./autogen.sh
// 构建并安装
./configure
make
sudo make install
sudo make install-mod_tile
sudo ldconfig

// 配置样式表
cd /home/devsum/src
git clone git://github.com/gravitystorm/openstreetmap-carto.git
cd openstreetmap-carto
// 安装carto
sudo apt install -y npm nodejs
sudo npm install -g carto
carto -v  # 检查carto版本高于1.1.0

//将git下来的项目转换成mapnik可读的文件
carto project.mml > mapnik.xml

// 下载地图（为测试，选用一个小的地图数据）
mkdir /home/devsum/data
cd /home/devsum/data
wget http://download.geofabrik.de/asia/azerbaijan-latest.osm.pbf

// 导入地图数据到数据库 （此处需要修改-c参数的值（node cache）当你的机器内存为1G时，推荐设置为小于800）
sudo su devsum osm2pgsql -d gis --create --slim  -G --hstore --tag-transform-script /home/devsum/src/openstreetmap-carto/openstreetmap-carto.lua -C 2500 --number-processes 1 -S /home/devsum/src/openstreetmap-carto/openstreetmap-carto.style /home/devsum/data/azerbaijan-latest.osm.pbf
e
exit  # 回到root用户
// 下载shapefile 
cd /home/devsum/src/openstreetmap-carto/
scripts/get-shapefiles.py  # 会花费较长时间，并可能失败注1// 下载font
sudo apt-get install -y fonts-noto-cjk fonts-noto-hinted fonts-noto-unhinted ttf-unifont
```
注1：如果下载耗时过长，你可以自己下载（https://pan.baidu.com/s/14SLismMSLlXm1Db_JOgIEQ 提取码：mbci ）后解压至到容器的/home/devsum/src/openstreetmap-carto/data/目录下。（每个压缩包文件解压到一个文件夹）
配置Web服务器

3.1 配置renderd

```shell
sudo nano /usr/local/etc/renderd.conf  # 打开renderd配置文件
```
修改以下内容:

```conf
num_threads=4  # 当机器内存为2GB左右时，修改为2
XML=/home/renderaccount/src/openstreetmap-carto/mapnik.xml  # 将renderaccount修改为选择的用户名，如devsum
```
3.2 配置Apache

```shell
sudo mkdir /var/lib/mod_tile
sudo chown devsum /var/lib/mod_tile

sudo mkdir /var/run/renderd
sudo chown devsum /var/run/renderd
```

3.3 在Apache中配置mod_tile

```shell
sudo nano /etc/apache2/conf-available/mod_tile.conf  # 修改该文件
LoadModule tile_module /usr/lib/apache2/modules/mod_tile.so  # 添加此行
```
保存退出后，运行:

```shell
sudo a2enconf mod_tile
```


3.4 在Apache中配置renderd

```shell
sudo nano /etc/apache2/sites-available/000-default.conf # 修改该文件
```

在“ServerAdmin”与“DocumentRoot”行间添加以下内容：

```shell
LoadTileConfigFile /usr/local/etc/renderd.conf
ModTileRenderdSocketName /var/run/renderd/renderd.sock
# Timeout before giving up for a tile to be rendered
ModTileRequestTimeout 0
# Timeout before giving up for a tile to be rendered that is otherwise missing
ModTileMissingRequestTimeout 30
```
完成后，启动Apache服务

```shell
sudo service apache2 start
```

3.5 前台运行renderd

接下来，我们尝试在前台运行renderd以检查配置是否成功。

```shell
sudo service postgresql start  # 首先运行postgresql服务
sudo su devsum  # 需要切换用户
renderd -f -c /usr/local/etc/renderd.conf  # 开启渲染
```

此时，打开浏览器，访问`127.0.0.1/hot/0/0/0.png`，正常显示一张图片，至此，配置基本完成。

3.6 配置后台renderd

```shell
nano /home/devsum/src/mod_tile/debian/renderd.init  # 修改renderaccount为devsum（或其他在上面设置的用户名）
sudo cp /home/devsum/src/mod_tile/debian/renderd.init /etc/init.d/renderd
sudo chmod u+x /etc/init.d/renderd
sudo cp /home/devsum/src/mod_tile/debian/renderd.service /lib/systemd/system/
```