# ngx_http_qrcode_module

```conf
server {
	listen 80;
	server_name  localhost;

	#set with constant
	location /qr_with_con {
		qrcode_fg_color FF0000;
		qrcode_bg_color FFFFFF;
		qrcode_level 2;
		qrcode_hint 2;
		qrcode_size 80;
		qrcode_margin 2;
		qrcode_version 2;
		qrcode_txt "http://wwww.dcshi.com";
		qrcode_casesensitive 1;
		qrcode_gen;
	}

	large_client_header_buffers  8  512k;
	#set with variables
	location /qr_with_var {
		if ($arg_fg_color ~ ^$){
			set $arg_fg_color 000000;
		}
		if ($arg_bg_color ~ ^$){
			set $arg_bg_color FFFFFF;
		}
		if ($arg_level ~ ^$){
			set $arg_level 2;
		}
		if ($arg_hint ~ ^$){
			set $arg_hint 2;
		}
		if ($arg_size ~ ^$){
			set $arg_size 80;
		}
		if ($arg_margin ~ ^$){
			set $arg_margin 5;
		}
		if ($arg_ver ~ ^$){
			set $arg_ver 2;
		}
		if ($arg_txt ~ ^$){
			set $arg_txt "http://wwww.myleft.org";
		}

		qrcode_fg_color $arg_fg_color;
		qrcode_bg_color $arg_bg_color;
		qrcode_level $arg_level;
		qrcode_hint $arg_hint;
		qrcode_size $arg_size;
		qrcode_margin $arg_margin;
		qrcode_version $arg_ver;
		qrcode_txt $arg_txt;
		#qrcode_urlencode_txt $arg_txt;

		qrcode_gen;
	}
}
```

访问地址：`http://localhost/qr?size=160&fg_color=00FF00&bg_color=fff700&case=1&txt=12a&margin=2&level=0&hint=2&ver=2`

# 参数说明

## 前景色: qrcode_fg_color color
- Default: qrcode_fg_color 000000
- Context: http, server, location
- Description: set the color of QRcode.


## 背景色: qrcode_bg_color color
- Default: qrcode_bg_color FFFFFF
- Context: http, server, location
- Description: set the background color of QRcode.


## 二维码的纠错级别: qrcode_level level
- Default: qrcode_level 0
- Context: http, server, location
- Description: level of error correction, [0:3]. Refer to http://fukuchi.org/works/qrencode/manual/qrencode_8h.html#a35d70229ba985c61bbdab27b0f65740e

0 L: 最大 7% 的错误能够被纠正；
1 M: 最大 15% 的错误能够被纠正；
2 Q: 最大 25% 的错误能够被纠正；
3 H: 最大 30% 的错误能够被纠正；

## Syntax: qrcode_hint hint
- Default: qrcode_hint 2
- Context: http, server, location
- Description: encoding mode. Refer to http://fukuchi.org/works/qrencode/manual/qrencode_8h.html#ab7ec78b96e63c8f019bb328a8d4f55db 

- QR_MODE_NUL -1
- QR_MODE_NUM 0
- QR_MODE_AN 1
- QR_MODE_8 2
- QR_MODE_KANJI 3
- QR_MODE_STRUCTURE 4

## Syntax: qrcode_size size
- Default: qrcode_size 4
- Context: http, server, location
- Description: size of qrcode image.(pixel)


## Syntax: qrcode_margin margin
- Default: qrcode_margin 4
- Context: http, server, location
- Description: margin of qrcode image.(pixel)


## 二维码的规格: qrcode_version version
- Default: qrcode_version 1
- Context: http, server, location
- Description: version of the symbol.it should less 10.

QR码符号共有40种规格的矩阵（一般为黑白色），从21x21（版本1），到177x177（版本40），每一版本符号比前一版本 每边增加4个模块。

## Syntax: qrcode_casesensitive [1 | 0]
- Default: qrcode_casesensitive off
- Context: http, server, location
- Description: case-sensitive(1) or not(0)


## Syntax: qrcode_txt txt
- Default: none
- Context: http, server, location
- Description: the txt you want to encode.


## Syntax: qrcode_urlencode_txt txt
- Default: none
- Context: http, server, location
- Description: the txt you want to encode. Different from qrcode_txt, the directive will urldecode first. Just http%3a%2f%2fdcshi.com%3fa%3db%26c%3dd => http://dcshi.com?a=b&c=d


## Syntax: qrcode_cp txt
- Default: none
- Context: http, server, location
- Description: the center picture encoding by base64


## Syntax: qrcode_gen
- Default: none
- Context: http, server, location
- Description: generate QRcode.