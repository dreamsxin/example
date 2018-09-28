# 安装 tofrodos
```shell
sudo apt-get install tofrodos
```
它安装了两个工具：todos（相当于unix2dos），和fromdos（相当于dos2unix）

比如：
```shell
todos Hello.txt (即unix2dos Hello.txt) 
fromdos Hello.txt (即dos2unix Hello.txt) 
```
```shell
ln -s /usr/bin/todos /usr/bin/unix2dos 
ln -s /usr/bin/fromdos /usr/bin/dos2unix 
```
或者在`~/.bashrc`
添加 `alias unix2dos=todos alias dos2unix=fromdos`
