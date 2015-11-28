#遍历压缩

for i in `ls *.mp4`; do zip -r ${i%\.*} $i; done