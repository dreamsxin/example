{
	"api" : {
		"connection" : "postgresql:host=localhost;dbname=ads;user=postgres;password=123456;@pool_size=10;@use_prepared=on",
	},
    "service" : {
        "api" : "http",
        "port" : 8080
    },
    "http" : {
        "script_names" : [ "/api/ad", "/api/setting" ]
    },
	"file_server" : {
		"enable": true,
		"document_root" : "/var/www/html",
	}
}