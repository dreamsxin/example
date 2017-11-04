
https://docs.blender.org/manual/zh-hans/dev/

```shell
# read-only access
git clone git://git.blender.org/blender.git

# read/write access
git clone git@git.blender.org:blender.git

cd blender
git submodule update --init --recursive
git submodule foreach git checkout master
git submodule foreach git pull --rebase origin master
```