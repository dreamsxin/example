
```shell
#下载免费的geo_city数据库
wget http://geolite.maxmind.com/download/geoip/database/GeoLiteCity.dat.gz
#下载免费的geo_coundty数据库
wget http://geolite.maxmind.com/download/geoip/database/GeoLiteCountry/GeoIP.dat.gz
#在debian中安装libgeoip:
sudo apt-get install libgeoip-dev
#其它系统，你可以下载并编译一个源文件
wget http://geolite.maxmind.com/download/geoip/api/c/GeoIP.tar.gz
```

# 配置 nginx.conf
```conf
geoip_country /etc/nginx/GeoIP.dat;
geoip_city /etc/nginx/GeoLiteCity.dat;
```

# 设置 header
```conf
add_header ip-country-code $geoip_city_country_code;
add_header ip-country $geoip_city_country_name
add_header ip-city $geoip_region;

fastcgi_param IP_COUNTRY_CODE $geoip_city_country_code;
fastcgi_param IP_COUNTRY $geoip_city_country_name
fastcgi_param IP_CITY $geoip_region;
```