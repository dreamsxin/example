# 安装

```shell
pip install web.py
# Or to get the latest development version from git:
git clone git://github.com/webpy/webpy.git
ln -s `pwd`/webpy/web .
python setup.py install
```

# 实例


```python
import web

urls = (
    '/', 'index'
)

class index:
    def GET(self):
        return "Hello, world!"

if __name__ == "__main__":
    app = web.application(urls, globals())
    app.run()
```