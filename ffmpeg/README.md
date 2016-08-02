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

./configure --enable-gpl --enable-version3 --enable-nonfree --enable-postproc --enable-libfaac --enable-libmp3lame --enable-libopencore-amrnb --enable-libopencore-amrwb --enable-libtheora --enable-libvorbis --enable-libvpx --enable-libx264 --enable-libxvid --enable-x11grab --enable-pthreads --enable-libspeex --enable-libfdk_aac --enable-decoder=h264

make && sudo make install
```

# 查看支持的编码
```shell
ffmpeg -codecs
```

# 读取摄像头和麦克风推送到rtmp

```shell
ffmpeg -f alsa -ac 1 -i pulse -acodec aac -f video4linux2  -s 1280x720 -i /dev/video0 -acodec libfdk_aac -vcodec libx264 -preset ultrafast -vprofile baseline -vlevel 1.0  -s 640x480 -b:v 800k -r 25  -pix_fmt yuv420p -f flv rtmp://192.168.1.108/live/test
```

# 屏幕录像，推送到rtmp
```shell
ffmpeg -f alsa -ac 1 -i pulse -acodec aac -f x11grab -s 1600x900 -i :0.0 -qscale 1 -ar 22050 -ab 64k -ac 1 -acodec libmp3lame -f flv rtmp://192.168.1.108/live/test

ffmpeg -f alsa -ac 1 -i pulse -acodec aac -f x11grab -s 1600x900 -i :0.0 -r 10 -b:v 10000k -qscale 1 -ar 22050 -ab 64k -ac 1 -acodec libmp3lame -f flv rtmp://192.168.1.108/live/test
```

## 屏幕录像，同时录音
```shell
#ffmpeg -f oss -i /dev/dsp-f x11grab -r 30 -s 1024x768 -i :0.0  output.mkv  
#ffmpeg -ac 2 -f oss -i /dev/dsp -f x11grab -r 30 -s 1024x768 -i :0.0 -acodec pcm_s16le -vcodec libx264 -vpre lossless_ultrafast -threads 0 output.mkv  
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

# mp3：
```shell
ffmpeg -y -i bg.mp3  -strict -2 -acodec vorbis -ac 2 -aq 50 bg.ogg
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

# ffmpeg 与 rtmp

1、将文件当做直播送至live
ffmpeg -re -i localFile.mp4 -c copy -f flv rtmp://server/live/streamName

2、将直播媒体保存至本地文件
ffmpeg -i rtmp://server/live/streamName -c copy dump.flv

3、将其中一个直播流，视频改用h264压缩，音频不变，送至另外一个直播服务流
ffmpeg -i rtmp://server/live/originalStream -c:a copy -c:v libx264 -vpre slow -f flv rtmp://server/live/h264Stream

4、将其中一个直播流，视频改用h264压缩，音频改用faac压缩，送至另外一个直播服务流
ffmpeg -i rtmp://server/live/originalStream -c:a libfaac -ar 44100 -ab 48k -c:v libx264 -vpre slow -vpre baseline -f flv rtmp://server/live/h264Stream

5、将其中一个直播流，视频不变，音频改用faac压缩，送至另外一个直播服务流
ffmpeg -i rtmp://server/live/originalStream -acodec libfaac -ar 44100 -ab 48k -vcodec copy -f flv rtmp://server/live/h264_AAC_Stream

6、将一个高清流，复制为几个不同视频清晰度的流重新发布，其中音频不变
ffmpeg -re -i rtmp://server/live/high_FMLE_stream 
-acodec copy -vcodec x264lib -s 640×360 -b 500k -vpre medium -vpre baseline rtmp://server/live/baseline_500k 
-acodec copy -vcodec x264lib -s 480×272 -b 300k -vpre medium -vpre baseline rtmp://server/live/baseline_300k 
-acodec copy -vcodec x264lib -s 320×200 -b 150k -vpre medium -vpre baseline rtmp://server/live/baseline_150k 
-acodec libfaac -vn -ab 48k rtmp://server/live/audio_only_AAC_48k

7、功能一样，只是采用-x264opts选项
ffmpeg -re -i rtmp://server/live/high_FMLE_stream 
-c:a copy -c:v x264lib -s 640×360 -x264opts bitrate=500:profile=baseline:preset=slow rtmp://server/live/baseline_500k 
-c:a copy -c:v x264lib -s 480×272 -x264opts bitrate=300:profile=baseline:preset=slow rtmp://server/live/baseline_300k
-c:a copy -c:v x264lib -s 320×200 -x264opts bitrate=150:profile=baseline:preset=slow rtmp://server/live/baseline_150k
-c:a libfaac -vn -b:a 48k rtmp://server/live/audio_only_AAC_48k

8、将当前摄像头及音频通过DSSHOW采集，视频h264、音频faac压缩后发布
ffmpeg -r 25 -f dshow -s 640×480 -i video=”video source name”:audio=”audio source name” -vcodec libx264 -b 600k -vpre slow -acodec libfaac -ab 128k rtmp://server/application/stream_name

9、将一个JPG图片经过h264压缩循环输出为mp4视频
ffmpeg -i INPUT.jpg -an -vcodec libx264 -coder 1 -flags +loop -cmp +chroma -subq 10 -qcomp 0.6 -qmin 10 -qmax 51 -qdiff 4 -flags2 +dct8x8 -trellis 2 -partitions +parti8x8+parti4x4 -crf 24 -threads 0 -r 25 -g 25 -y OUTPUT.mp4

10、将普通流视频改用h264压缩，音频不变，送至高清流服务(新版本FMS live=1)
ffmpeg -i rtmp://server/live/originalStream -c:a copy -c:v libx264 -vpre slow -f flv “rtmp://server/live/h264Stream live=1″


选项

a) 通用选项

-L license

-h 帮助

-fromats 显示可用的格式，编解码的，协议的。。。

-f fmt 强迫采用格式fmt

-I filename 输入文件

-y 覆盖输出文件

-t duration 设置纪录时间 hh:mm:ss[.xxx]格式的记录时间也支持

-ss position 搜索到指定的时间 [-]hh:mm:ss[.xxx]的格式也支持

-title string 设置标题

-author string 设置作者

-copyright string 设置版权

-comment string 设置评论

-target type 设置目标文件类型(vcd,svcd,dvd) 所有的格式选项（比特率，编解码以及缓冲区大小）自动设置 ，只需要输入如下的就可以了：
ffmpeg -i myfile.avi -target vcd /tmp/vcd.mpg

-hq 激活高质量设置

-itsoffset offset 设置以秒为基准的时间偏移，该选项影响所有后面的输入文件。该偏移被加到输入文件的时戳，定义一个正偏移意味着相应的流被延迟了 offset秒。 [-]hh:mm:ss[.xxx]的格式也支持

b) 视频选项

-b bitrate 设置比特率，缺省200kb/s

-r fps 设置帧频 缺省25

-s size 设置帧大小 格式为WXH 缺省160X128.下面的简写也可以直接使用：
Sqcif 128X96 qcif 176X144 cif 252X288 4cif 704X576

-aspect aspect 设置横纵比 4:3 16:9 或 1.3333 1.7777

-croptop size 设置顶部切除带大小 像素单位

-cropbottom size –cropleft size –cropright size

-padtop size 设置顶部补齐的大小 像素单位

-padbottom size –padleft size –padright size –padcolor color 设置补齐条颜色(hex,6个16进制的数，红:绿:兰排列，比如 000000代表黑色)

-vn 不做视频记录

-bt tolerance 设置视频码率容忍度kbit/s

-maxrate bitrate设置最大视频码率容忍度

-minrate bitreate 设置最小视频码率容忍度

-bufsize size 设置码率控制缓冲区大小

-vcodec codec 强制使用codec编解码方式。 如果用copy表示原始编解码数据必须被拷贝。

-sameq 使用同样视频质量作为源（VBR）

-pass n 选择处理遍数（1或者2）。两遍编码非常有用。第一遍生成统计信息，第二遍生成精确的请求的码率

-passlogfile file 选择两遍的纪录文件名为file


c)高级视频选项

-g gop_size 设置图像组大小

-intra 仅适用帧内编码

-qscale q 使用固定的视频量化标度(VBR)

-qmin q 最小视频量化标度(VBR)

-qmax q 最大视频量化标度(VBR)

-qdiff q 量化标度间最大偏差 (VBR)

-qblur blur 视频量化标度柔化(VBR)

-qcomp compression 视频量化标度压缩(VBR)

-rc_init_cplx complexity 一遍编码的初始复杂度

-b_qfactor factor 在p和b帧间的qp因子

-i_qfactor factor 在p和i帧间的qp因子

-b_qoffset offset 在p和b帧间的qp偏差

-i_qoffset offset 在p和i帧间的qp偏差

-rc_eq equation 设置码率控制方程 默认tex^qComp

-rc_override override 特定间隔下的速率控制重载

-me method 设置运动估计的方法 可用方法有 zero phods log x1 epzs(缺省) full

-dct_algo algo 设置dct的算法 可用的有 0 FF_DCT_AUTO 缺省的DCT 1 FF_DCT_FASTINT 2 FF_DCT_INT 3 FF_DCT_MMX 4 FF_DCT_MLIB 5 FF_DCT_ALTIVEC

-idct_algo algo 设置idct算法。可用的有 0 FF_IDCT_AUTO 缺省的IDCT 1 FF_IDCT_INT 2 FF_IDCT_SIMPLE 3 FF_IDCT_SIMPLEMMX 4 FF_IDCT_LIBMPEG2MMX 5 FF_IDCT_PS2 6 FF_IDCT_MLIB 7 FF_IDCT_ARM 8 FF_IDCT_ALTIVEC 9 FF_IDCT_SH4 10 FF_IDCT_SIMPLEARM

-er n 设置错误残留为n 1 FF_ER_CAREFULL 缺省 2 FF_ER_COMPLIANT 3 FF_ER_AGGRESSIVE 4 FF_ER_VERY_AGGRESSIVE

-ec bit_mask 设置错误掩蔽为bit_mask,该值为如下值的位掩码 1 FF_EC_GUESS_MVS (default=enabled) 2 FF_EC_DEBLOCK (default=enabled)

-bf frames 使用frames B 帧，支持mpeg1,mpeg2,mpeg4

-mbd mode 宏块决策 0 FF_MB_DECISION_SIMPLE 使用mb_cmp 1 FF_MB_DECISION_BITS 2 FF_MB_DECISION_RD

-4mv 使用4个运动矢量 仅用于mpeg4

-part 使用数据划分 仅用于mpeg4

-bug param 绕过没有被自动监测到编码器的问题

-strict strictness 跟标准的严格性

-aic 使能高级帧内编码 h263+

-umv 使能无限运动矢量 h263+

-deinterlace 不采用交织方法

-interlace 强迫交织法编码 仅对mpeg2和mpeg4有效。当你的输入是交织的并且你想要保持交织以最小图像损失的时候采用该选项。可选的方法是不交织，但是损失更大

-psnr 计算压缩帧的psnr

-vstats 输出视频编码统计到vstats_hhmmss.log

-vhook module 插入视频处理模块 module 包括了模块名和参数，用空格分开

D)音频选项

-ab bitrate 设置音频码率

-ar freq 设置音频采样率

-ac channels 设置通道 缺省为1

-an 不使能音频纪录

-acodec codec 使用codec编解码

E)音频/视频捕获选项

-vd device 设置视频捕获设备。比如/dev/video0

-vc channel 设置视频捕获通道 DV1394专用

-tvstd standard 设置电视标准 NTSC PAL(SECAM)

-dv1394 设置DV1394捕获

-av device 设置音频设备 比如/dev/dsp


F)高级选项

-map file:stream 设置输入流映射

-debug 打印特定调试信息

-benchmark 为基准测试加入时间

-hex 倾倒每一个输入包

-bitexact 仅使用位精确算法 用于编解码测试

-ps size 设置包大小，以bits为单位

-re 以本地帧频读数据，主要用于模拟捕获设备

-loop 循环输入流。只工作于图像流，用于ffserver测试