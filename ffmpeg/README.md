# 编译安装

```shell
git clone --depth 1 git://github.com/mstorsjo/fdk-aac.git
cd fdk-aac
autoreconf -fiv
./configure --disable-shared
```

```shell
sudo add-apt-repository ppa:kirillshkrogalev/ffmpeg-next
sudo apt-get update
sudo apt-get install ffmpeg

# or

sudo apt-get install libx264-dev x264 libfaac-dev libfaac0 yasm libmp3lame-dev libopencore-amrwb-dev libtheora-dev libogg-dev libvorbis-dev libvpx-dev libxvidcore-dev libopencore-amrnb-dev libsdl1.2-dev
#git clone https://git.ffmpeg.org/ffmpeg.git ffmpeg
wget http://ffmpeg.org/releases/ffmpeg-3.0.2.tar.bz2
cd ffmpeg
./configure --enable-gpl --enable-version3 --enable-nonfree --enable-postproc --enable-libfaac --enable-libmp3lame --enable-libopencore-amrnb --enable-libopencore-amrwb --enable-libtheora --enable-libvorbis --enable-libvpx --enable-libx264 --enable-libxvid --enable-x11grab --enable-pthreads --enable-libspeex --enable-libfdk_aac
make && sudo make install
```

# 查看支持的编码
```shell
ffmpeg -codecs
```

# 格式转换

`https://www.ffmpeg.org/ffmpeg.html#Video-and-Audio-file-format-conversion`

# 设置视频输出文件比特率为 64 kbit/s

```shell
ffmpeg -i input.avi -b:v 64k -bufsize 64k output.avi
```

# 强制视频为 24 帧

```shell
ffmpeg -i input.avi -r 24 output.avi
```

# 压缩jpg
```shell
sudo apt-get install jpegoptim
jpegoptim input.jpg
```

# 缩放图片
```shell
convert -sample 25%x25% input.jpg output.jpg
```

## 转换文件
```shell
ffmpeg -i 中国好声音抚州赛区.flv -vcodec libx264 -r 15 中国好声音抚州赛区.mp4
ffmpeg -i test.flv  -c copy output.mp4
ffmpeg -i test.flv  -c copy -copyts output.mp4
ffmpeg -i input.flv -vcodec libx264  -b 200k -r 24 -ab 64k -ar 32000 output.mp4
ffmpeg -threads 2 -i input.flv -f ogg  -acodec libvorbis -ac 2 -ab 128k -ar 44100 output.ogg
ffmpeg -i input.mp4 -f ogg  -acodec libvorbis -ac 2 -ab 128k -ar 44100 output.ogg
```

# webm
```shell
ffmpeg -y -i input.mp4 -f webm -vcodec libvpx -acodec libvorbis -vb 1600000 output.webm

# ogv
```shell
ffmpeg2theora -V 4000 -A 128 input.mp4 -o output.ogv
```

# ogv：
```shell
ffmpeg -i input.ogg -i input.ogv -acodec copy -vcodec copy mixed.ogg
```

# ffmpeg参数设定解说 
-bitexact 使用标准比特率 
-vcodec xvid 使用xvid压缩 
-s 320x240 指定分辨率 
-r 29.97 桢速率（可以改，确认非标准桢率会导致音画不同步，所以只能设定为15或者29.97）

画面部分
-b <比特率> 指定压缩比特率，似乎ffmpeg是自动VBR的，指定了就大概是平均比特率，比如768，1500这样的就是原来默认项目中有的 
-qscale <数值> 以<数值>质量为基础的VBR，取值0.01-255，越小质量越好 
-qmin <数值> 设定最小质量，与-qmax（设定最大质量）共用，比如-qmin 10 -qmax 31 
-sameq 使用和源同样的质量 

声音部分 
-acodec aac 设定声音编码 
-ac <数值> 设定声道数，1就是单声道，2就是立体声，转换单声道的TVrip可以用1（节省一半容量），高品质的DVDrip就可以用2 
-ar <采样率> 设定声音采样率，PSP只认24000 
-ab <比特率> 设定声音比特率，前面-ac设为立体声时要以一半比特率来设置，比如192kbps的就设成96，转换君默认比特率都较小，要听到较高品质声音的话建议设到160kbps（80）以上 
-vol <百分比> 设定音量，某些DVDrip的AC3轨音量极小，转换时可以用这个提高音量，比如200就是原来的2倍 

这样，要得到一个高画质音质低容量的MP4的话，首先画面最好不要用固定比特率，而用VBR参数让程序自己去 
判断，而音质参数可以在原来的基础上提升一点，听起来要舒服很多，也不会太大