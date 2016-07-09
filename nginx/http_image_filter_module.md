```conf
	location ~* /upload/img/(.+)_(\d+)x(\d+)\.(jpg|gif|png)$ {            
		set $h $2;
		set $w $3;
		if ($h = "0") {
			rewrite /upload/img/(.+)_(\d+)x(\d+)\.(jpg|gif|png)$ /upload/img/$1.$4 last;
		}
		if ($w = "0") {
			rewrite /upload/img/(.+)_(\d+)x(\d+)\.(jpg|gif|png)$ /upload/img/$1.$4 last;
		}
 
		image_filter resize $h $w;
		image_filter_buffer 2M;
		try_files /upload/img/$1.$4  /upload/img/404_$hx$w.jpg;	
	}
```