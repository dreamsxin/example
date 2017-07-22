## 安装

```shell
sudo apt-get install ipython ipython-notebook
sudo -H pip install jupyter
```

## 启动

```shell
jupyter notebook
```

访问 http://localhost:8888/

## 生成配置文件

```shell
jupyter notebook  --generate-config
```
```text
Writing default config to: /home/snail/.jupyter/jupyter_notebook_config.py
```

## 生成自签名SSL证书
```shell
cd ~/.jupyter
openssl req -x509 -nodes -days 365 -newkey rsa:1024 -keyout notebook_cert.key -out notebook_cert.pem
```

## 生成一个密码hash
```shell
python -c "import IPython;print(IPython.lib.passwd())"
```

## 编辑配置文件

`jupyter_notebook_config.py`
```python
c = get_config()

c.NotebookApp.certfile = u'/home/snail/.jupyter/notebook_cert.pem'
c.NotebookApp.keyfile = u'/home/snail/.jupyter/notebook_cert.key'

c.NotebookApp.password = u'sha1:cac0ff2f9346:e3823d119d732a1286421d676d62be60d9d423c5'

c.NotebookApp.ip = '*'
c.NotebookApp.port = 8080
c.NotebookApp.open_browser = False
```

