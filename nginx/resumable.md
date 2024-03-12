server {
	[...]
	location /resumable_upload {
	       upload_resumable on;
	       upload_state_store /usr/local/nginx/upload_temp ;
	       upload_pass @drivers_upload_handler;
	       upload_store /usr/local/nginx/upload_temp;
	       upload_set_form_field $upload_field_name.path "$upload_tmp_path";
	}

	location @resumable_upload_handler {
	       proxy_pass http://localhost:8002;
	}
	[...]
}
