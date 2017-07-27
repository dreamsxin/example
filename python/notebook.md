# IPython Notebook

```shell
ipython notebook --pylab=inline
```

## 创建自定义配置

我们可以修改默认的配置，也可以创建多个自定义配置方案，在不同项目中使用。

```shell
ipython profile create my_profile
```

将会生成以下配置文件：
- .ipython/profile_my_profile/ipython_config.py
- .ipython/profile_my_profile/ipython_notebook_config.py
- .ipython/profile_my_profile/ipython_nbconvert_config.py

## 使用指定自定义配置

```shell
ipython --profile=my_profile
```
