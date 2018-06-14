#遍历压缩

for i in `ls *.mp4`; do zip -r ${i%\.*} $i; done

for i in `ls *.doc`; do zip -P 123456 -r ${i%\.*} $i; done

for i in `ls *.flv`; do ffmpeg -i ${i%\.*} -ar 16000 -ac 1 ${$i.ogv}; done