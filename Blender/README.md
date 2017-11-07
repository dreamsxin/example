
https://docs.blender.org/manual/zh-hans/dev/

```shell
sudo add-apt-repository ppa:thomas-schiex/blender
sudo apt-get update
sudo apt-get install blender

# read-only access
git clone git://git.blender.org/blender.git

# read/write access
git clone git@git.blender.org:blender.git

cd blender
git submodule update --init --recursive
git submodule foreach git checkout master
git submodule foreach git pull --rebase origin master
```