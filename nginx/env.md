# 读取系统环境变量

## With Lua

```conf
http {
  ...
  server {
    location / {
      set_by_lua $api_key 'return os.getenv("API_KEY")';
      ...
    }
  }
}
```

In this example we are assigning the environment variable to one of Nginx variables; we can use $api_key as a regular nginx.conf variable.

## With Perl


```conf
http {
  ...
  server {
    location / {
      perl_set $api_key 'sub { return $ENV{"API_KEY"}; }';
      ...
    }
  }
}
```