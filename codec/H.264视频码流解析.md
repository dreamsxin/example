---
layout:     post
title:      H.264视频码流解析
subtitle:   视音频开发
date:       2019-08-07
author:     YunLambert
header-img: img/post-bg-H264.jpg
catalog: true
tags:
    - Tips
    - Share
    - Media
---

> # H.264视频码流解析

## 一些基本概念

[YUV/RGB处理](http://blog.yunlambert.top/2019/07/22/RGB-and-YUV/)以及[PCM处理](http://blog.yunlambert.top/2019/08/01/PCM%E9%9F%B3%E9%A2%91%E9%87%87%E6%A0%B7%E5%A4%84%E7%90%86/)处于对视音频原始数据的处理，但是视音频在传输的过程中是以视音频码流传输的。接下来介绍一下视频码流：

**H**.**264**的标准定义如下：同时也是MPEG-4第十部分，是由ITU-T视频编码专家组（VCEG）和ISO/IEC动态图像专家组（MPEG）联合组成的联合视频组（JVT，Joint Video Team）提出的高度压缩数字视频编解码器标准。

### 什么是H.264

先通过PR编辑软件的导出列表形象生动地感受一下H.264格式：

![多种格式](https://upload-images.jianshu.io/upload_images/7154520-1e7a7169ef50e93c.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

H.264原始码流（又称为“裸流”）是由一个一个的NALU组成的，结构是这样的：

![NALU](https://upload-images.jianshu.io/upload_images/7154520-9afb53c7d403645c.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

每个NALU之间通过起始码进行分隔，起始码分成两种：0x000001(3Byte)或者0x00000001(4Byte)。NALU对应的Slice为一帧的开始就用0x00000001，否则就用0x000001。H.264码流解析的步骤就是首先从码流中搜索0x000001和0x00000001，分离出NALU；然后再分析NALU的各个字段 。

### 什么是I帧、P帧、B帧

视频压缩中，每帧代表一幅静止的图像。而在实际压缩时，会采取各种算法减少数据的容量，而IPB就是最常见的。I帧是关键帧(帧内压缩)，而P是向前搜索、B是双向搜索，都是基于I帧来压缩数据。

```
I帧表示关键帧，你可以理解为这一帧画面的完整保留；解码时只需要本帧数据就可以完成（因为包含完整画面）

P帧表示的是这一帧跟之前的一个关键帧（或P帧）的差别，解码时需要用之前缓存的画面叠加上本帧定义的差别，生成最终画面。（也就是差别帧，P帧没有完整画面数据，只有与前一帧的画面差别的数据）

B帧是双向差别帧，也就是B帧记录的是本帧与前后帧的差别（具体比较复杂，有4种情况），换言之，要解码B帧，不仅要取得之前的缓存画面，还要解码之后的画面，通过前后画面的与本帧数据的叠加取得最终的画面。B帧压缩率高，但是解码时CPU会比较累~
```

网络上的电影很多采用了B帧，因为B帧记录的是前后帧的差别，比P帧能节约更多的空间。但是如果遇到不支持B帧的播放器就会很尴尬，因为虽然文件通过B帧压缩确实缩小了非常多，但是在解码时，不仅要用之前缓存的画面，还要知道下一个I或者P的画面。**如果B帧丢失，就会用之前的画面简单重复，就会造成画面卡，B帧越多，画面越卡。**

![图片.png](https://upload-images.jianshu.io/upload_images/7154520-38f8a68471f25ac9.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

```
一般来说，I 帧的压缩率是7（跟JPG差不多），P 帧是20，B 帧可以达到50。

在如上图中，GOP (Group of Pictures)长度为13，S0~S7 表示 8个视点，T0~T12 为 GOP的 13个时刻。每个 GOP包含帧数为视点数 GOP 长度的乘积。在该图中一个 GOP 中，包含94 个 B帧。B 帧占一个 GOP 总帧数的 90.38%。GOP 越长，B 帧所占比例更高，编码的率失真性能越高
```

解释一下GOP，是图像组的意思，表示编码的视频序列分成了一组一组有序的帧的集合进行编码。

### H.264编码原理

H.264采用的编码算法就是帧内压缩和帧间压缩，帧内压缩是生成I帧的算法，帧间压缩是生成B帧和P帧的算法。

视频中一般会存在一段变化不大的图像画面，我们可以先编码出一个完整的图像帧`t1`，之后的图像帧`t2`就可以不编码全部图像，而只写入与`t1`帧的差别，之后的图像帧参考`t3`帧仍旧可以参照这种方式继续进行编码。这样循环下去就可以得到一个序列。如果下一个图像帧`t4`与之前的图像变化很大，无法参考前面的图像帧，就结束上一序列，并开始下一序列。

序列就可以定义为一段图像编码后的数据流，以I帧开始，到下一个I帧结束，H.264中图像就是以序列为单位进行组织的。

### H.264压缩方法

知道视频是如何编码的了，那么怎样具体进行编码呢？有以下步骤：

```
（1）分组：把几帧图像分为一组(GOP，也就是一个序列)，为防止运动变化,帧数不宜取多。
（2）定义帧：将每组内各帧图像定义为三种类型,即 I 帧、B 帧和 P 帧;
（3）预测帧：以I帧做为基础帧，以 I 帧预测 P 帧,再由 I 帧和 P 帧预测 B 帧;
（4）数据传输：最后将 I 帧数据与预测的差值信息进行存储和传输。
```

帧内（Intraframe）压缩也称为空间压缩（Spatial compression）。当压缩一帧图像时，仅考虑本帧的数据而不考虑相邻帧之间的冗余信息，这实际上与静态图像压缩类似。帧内一般采用有损压缩算法，由于帧内压缩是编码一个完整的图像，所以可以独立的解码、显示。帧内压缩一般达不到很高的压缩，跟编码 jpeg 差不多。　　

帧间（Interframe）压缩的原理是：相邻几帧的数据有很大的相关性，或者说前后两帧信息变化很小的特点。也即连续的视频其相邻帧之间具有冗余信息,根据这一特性，压缩相邻帧之间的冗余量就可以进一步提高压缩量，减小压缩比。帧间压缩也称为时间压缩（Temporal compression），它通过比较时间轴上不同帧之间的数据进行压缩。帧间压缩一般是无损的。帧差值（Frame differencing）算法是一种典型的时间压缩法，它通过比较本帧与相邻帧之间的差异，仅记录本帧与其相邻帧的差值，这样可以大大减少数据量。

### IDR帧

重点看一下P帧的定义：`P帧表示的是这一帧跟之前的一个关键帧（或P帧）的差别，解码.......`，**P帧是可以参照前面的P帧的**，并且GOP虽然一定是I帧开始的，但在一个GOP中不一定只有2个I帧，可能包含几个I帧，只有第一个I帧才是关键帧。但是在这种情况下，举个例子：`I P B P B P B B B P1 I1 P2 B`，为了方便表示，将需要用到的图像帧定义为P1 I1 P2；P2帧除了会参照I1帧之外，还会参考P1帧，如果P1与I帧画面相差较大的话，那么不但没有意义还会造成画面问题，所以我们需要一种帧能够禁止后面的帧向前面的帧参照，这就是IDR帧。

```
  I 和 IDR 帧都是使用帧内预测的。它们都是同一个东西而已，在编码和解码中为了方便，要首个 I 帧和其他 I 帧区别开，所以才把第一个首个 I 帧叫 IDR，这样就方便控制编码和解码流程。IDR 帧的作用是立刻刷新，使错误不致传播,从IDR帧开始，重新算一个新的序列开始编码。而 I 帧不具有随机访问的能力，这个功能是由 IDR 承担。IDR 会导致DPB（参考帧列表——这是关键所在）清空，而 I 不会。IDR 图像一定是 I 图像，但I图像不一定是 IDR 图像。一个序列中可以有很多的I图像，I 图像之后的图像可以引用 I 图像之间的图像做运动参考。一个序列中可以有很多的 I 图像，I 图像之后的图象可以引用I图像之间的图像做运动参考。
```

##  详解H.264 NALU语法结构

首先NALU是H.264编码数据存储或传输的基本单元，一般H.264码流最开始的两个NALU是SPS和PPS，第三个NALU是IDR。而SPS、PPS、SEI这三种NALU不属于帧的范畴，它们的定义如下：

```
SPS(Sequence Parameter Sets)：序列参数集，作用于一系列连续的编码图像。
PPS(Picture Parameter Set)：图像参数集，作用于编码视频序列中一个或多个独立的图像。
SEI(Supplemental enhancement information)：附加增强信息，包含了视频画面定时等信息，一般放在主编码图像数据之前，在某些应用中，它可以被省略掉。
IDR(Instantaneous Decoding Refresh)：即时解码刷新。
HRD(Hypothetical Reference Decoder)：假想码流调度器。
```

### 语法结构

在H.264/AVC视频编码标准中，整个系统框架被分为了两个层面：视频编码层面（VCL）和网络抽象层面（NAL）。其中，前者负责有效表示视频数据的内容，而后者则负责格式化数据并提供头信息，以保证数据适合各种信道和存储介质上的传输。因此我们平时的每帧数据就是一个NAL单元（SPS与PPS除外）。在实际的H264数据帧中，往往帧前面带有00 00 00 01 或 00 00 01分隔符，一般来说编码器编出的首帧数据为PPS与SPS，接着为I帧……

一个H.264文件如下：

![图片.png](https://upload-images.jianshu.io/upload_images/7154520-54b461cfe623f37d.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

在H264码流中，都是以"0x00 0x00 0x01"或者"0x00 0x00 0x00 0x01"为开始码的，找到开始码之后，使用开始码之后的第一个字节的低 5 位判断是否为 7(sps)或者 8(pps), 及 data[4] & 0x1f == 7 || data[4] & 0x1f == 8。然后对获取的 nal 去掉开始码之后进行 base64 编码，得到的信息就可以用于 sdp。 sps和pps需要用逗号分隔开来。

上图中，00 00 00 01是一个NALU的起始标志。后面的第一个字节，0x67，是NALU的类型，type &0x1f==0x7表示这个NALU是SPS，type &0x1f==0x8表示是PPS。

H.264基本码流结构分两层，视频编码层VCL和网络适配层NAL，这样使信号处理和网络传输分离，而H.264码流在网络中传输时实际上是以一系列的NALU组成，原始的NALU单元组成： 

```
[start code] + [NALU header] + [NALU payload] 
```

[start code]占1字节，为0x000001或0x00000001。

其中[NALU header]如下：

```
forbidden_zero_bit(1bit) + nal_ref_idc(2bit) + nal_unit_type(5bit)
```

![图片.png](https://upload-images.jianshu.io/upload_images/7154520-1286ec19ac5944a3.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

00 00 00 01 为起始符，67 即 nal_unit_type。

0x67的二进制是 0110 0111

则 forbidden_zero_bit(1bit) = 0；

nal_ref_idc(2bit) = 3；

nal_unit_type(5bit) = 7；即 SPS 类型。

更多关于H.264NALU的语法可以看这几篇：

[https://blog.csdn.net/wutong_login/article/details/5818763](https://blog.csdn.net/wutong_login/article/details/5818763)

[https://blog.csdn.net/wutong_login/article/details/5824509](https://blog.csdn.net/wutong_login/article/details/5824509)

## 应用：分离NALU并分析字段

来自雷神：

```C++
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
typedef enum {
	NALU_TYPE_SLICE    = 1,
	NALU_TYPE_DPA      = 2,
	NALU_TYPE_DPB      = 3,
	NALU_TYPE_DPC      = 4,
	NALU_TYPE_IDR      = 5,
	NALU_TYPE_SEI      = 6,
	NALU_TYPE_SPS      = 7,
	NALU_TYPE_PPS      = 8,
	NALU_TYPE_AUD      = 9,
	NALU_TYPE_EOSEQ    = 10,
	NALU_TYPE_EOSTREAM = 11,
	NALU_TYPE_FILL     = 12,
} NaluType;
 
typedef enum {
	NALU_PRIORITY_DISPOSABLE = 0,
	NALU_PRIRITY_LOW         = 1,
	NALU_PRIORITY_HIGH       = 2,
	NALU_PRIORITY_HIGHEST    = 3
} NaluPriority;
 
 
typedef struct
{
	int startcodeprefix_len;      //! 4 for parameter sets and first slice in picture, 3 for everything else (suggested)
	unsigned len;                 //! Length of the NAL unit (Excluding the start code, which does not belong to the NALU)
	unsigned max_size;            //! Nal Unit Buffer size
	int forbidden_bit;            //! should be always FALSE
	int nal_reference_idc;        //! NALU_PRIORITY_xxxx
	int nal_unit_type;            //! NALU_TYPE_xxxx    
	char *buf;                    //! contains the first byte followed by the EBSP
} NALU_t;
 
FILE *h264bitstream = NULL;                //!< the bit stream file
 
int info2=0, info3=0;
 
static int FindStartCode2 (unsigned char *Buf){
	if(Buf[0]!=0 || Buf[1]!=0 || Buf[2] !=1) return 0; //0x000001?
	else return 1;
}
 
static int FindStartCode3 (unsigned char *Buf){
	if(Buf[0]!=0 || Buf[1]!=0 || Buf[2] !=0 || Buf[3] !=1) return 0;//0x00000001?
	else return 1;
}
 
 
int GetAnnexbNALU (NALU_t *nalu){
	int pos = 0;
	int StartCodeFound, rewind;
	unsigned char *Buf;
 
	if ((Buf = (unsigned char*)calloc (nalu->max_size , sizeof(char))) == NULL) 
		printf ("GetAnnexbNALU: Could not allocate Buf memory\n");
 
	nalu->startcodeprefix_len=3;
 
	if (3 != fread (Buf, 1, 3, h264bitstream)){
		free(Buf);
		return 0;
	}
	info2 = FindStartCode2 (Buf);
	if(info2 != 1) {
		if(1 != fread(Buf+3, 1, 1, h264bitstream)){
			free(Buf);
			return 0;
		}
		info3 = FindStartCode3 (Buf);
		if (info3 != 1){ 
			free(Buf);
			return -1;
		}
		else {
			pos = 4;
			nalu->startcodeprefix_len = 4;
		}
	}
	else{
		nalu->startcodeprefix_len = 3;
		pos = 3;
	}
	StartCodeFound = 0;
	info2 = 0;
	info3 = 0;
 
	while (!StartCodeFound){
		if (feof (h264bitstream)){
			nalu->len = (pos-1)-nalu->startcodeprefix_len;
			memcpy (nalu->buf, &Buf[nalu->startcodeprefix_len], nalu->len);     
			nalu->forbidden_bit = nalu->buf[0] & 0x80; //1 bit
			nalu->nal_reference_idc = nalu->buf[0] & 0x60; // 2 bit
			nalu->nal_unit_type = (nalu->buf[0]) & 0x1f;// 5 bit
			free(Buf);
			return pos-1;
		}
		Buf[pos++] = fgetc (h264bitstream);
		info3 = FindStartCode3(&Buf[pos-4]);
		if(info3 != 1)
			info2 = FindStartCode2(&Buf[pos-3]);
		StartCodeFound = (info2 == 1 || info3 == 1);
	}
 
	// Here, we have found another start code (and read length of startcode bytes more than we should
	// have.  Hence, go back in the file
	rewind = (info3 == 1)? -4 : -3;
 
	if (0 != fseek (h264bitstream, rewind, SEEK_CUR)){
		free(Buf);
		printf("GetAnnexbNALU: Cannot fseek in the bit stream file");
	}
 
	// Here the Start code, the complete NALU, and the next start code is in the Buf.  
	// The size of Buf is pos, pos+rewind are the number of bytes excluding the next
	// start code, and (pos+rewind)-startcodeprefix_len is the size of the NALU excluding the start code
 
	nalu->len = (pos+rewind)-nalu->startcodeprefix_len;
	memcpy (nalu->buf, &Buf[nalu->startcodeprefix_len], nalu->len);//
	nalu->forbidden_bit = nalu->buf[0] & 0x80; //1 bit
	nalu->nal_reference_idc = nalu->buf[0] & 0x60; // 2 bit
	nalu->nal_unit_type = (nalu->buf[0]) & 0x1f;// 5 bit
	free(Buf);
 
	return (pos+rewind);
}
 
/**
 * Analysis H.264 Bitstream
 * @param url    Location of input H.264 bitstream file.
 */
int simplest_h264_parser(char *url){
 
	NALU_t *n;
	int buffersize=100000;
 
	//FILE *myout=fopen("output_log.txt","wb+");
	FILE *myout=stdout;
 
	h264bitstream=fopen(url, "rb+");
	if (h264bitstream==NULL){
		printf("Open file error\n");
		return 0;
	}
 
	n = (NALU_t*)calloc (1, sizeof (NALU_t));
	if (n == NULL){
		printf("Alloc NALU Error\n");
		return 0;
	}
 
	n->max_size=buffersize;
	n->buf = (char*)calloc (buffersize, sizeof (char));
	if (n->buf == NULL){
		free (n);
		printf ("AllocNALU: n->buf");
		return 0;
	}
 
	int data_offset=0;
	int nal_num=0;
	printf("-----+-------- NALU Table ------+---------+\n");
	printf(" NUM |    POS  |    IDC |  TYPE |   LEN   |\n");
	printf("-----+---------+--------+-------+---------+\n");
 
	while(!feof(h264bitstream)) 
	{
		int data_lenth;
		data_lenth=GetAnnexbNALU(n);
 
		char type_str[20]={0};
		switch(n->nal_unit_type){
			case NALU_TYPE_SLICE:sprintf(type_str,"SLICE");break;
			case NALU_TYPE_DPA:sprintf(type_str,"DPA");break;
			case NALU_TYPE_DPB:sprintf(type_str,"DPB");break;
			case NALU_TYPE_DPC:sprintf(type_str,"DPC");break;
			case NALU_TYPE_IDR:sprintf(type_str,"IDR");break;
			case NALU_TYPE_SEI:sprintf(type_str,"SEI");break;
			case NALU_TYPE_SPS:sprintf(type_str,"SPS");break;
			case NALU_TYPE_PPS:sprintf(type_str,"PPS");break;
			case NALU_TYPE_AUD:sprintf(type_str,"AUD");break;
			case NALU_TYPE_EOSEQ:sprintf(type_str,"EOSEQ");break;
			case NALU_TYPE_EOSTREAM:sprintf(type_str,"EOSTREAM");break;
			case NALU_TYPE_FILL:sprintf(type_str,"FILL");break;
		}
		char idc_str[20]={0};
		switch(n->nal_reference_idc>>5){
			case NALU_PRIORITY_DISPOSABLE:sprintf(idc_str,"DISPOS");break;
			case NALU_PRIRITY_LOW:sprintf(idc_str,"LOW");break;
			case NALU_PRIORITY_HIGH:sprintf(idc_str,"HIGH");break;
			case NALU_PRIORITY_HIGHEST:sprintf(idc_str,"HIGHEST");break;
		}
 
		fprintf(myout,"%5d| %8d| %7s| %6s| %8d|\n",nal_num,data_offset,idc_str,type_str,n->len);
 
		data_offset=data_offset+data_lenth;
 
		nal_num++;
	}
 
	//Free
	if (n){
		if (n->buf){
			free(n->buf);
			n->buf=NULL;
		}
		free (n);
	}
	return 0;
}
```

结果如下：

![图片.png](https://upload-images.jianshu.io/upload_images/7154520-d1302d5c263ea516.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

## 应用：网页视频流m3u8/ts视频下载

现在很多视频网站播放流视频，都不采用mp4/flv文件播放，而是采用m3u8/ts这种方式播放。这种方式既是为了缓解浏览器缓存的压力，也是为了防止视频泄漏。简单来说就是，网站后台把视频切片成上千个xx.ts文件，一般10s一个，每个都几百个kb很小，然后通过`xx.m3u8`播放列表把这些文件连接起来。

![图片.png](https://upload-images.jianshu.io/upload_images/7154520-4f2080c1ff7e00b1.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

我们直接点击这个`playlist.m3u8`播放列表文件，在旁边的`preview`栏中查看内容，可以看到：

```
#EXTM3U
#EXT-X-VERSION:3
#EXT-X-MEDIA-SEQUENCE:0
#EXT-X-ALLOW-CACHE:YES
#EXT-X-TARGETDURATION:11
#EXTINF:5.250000,
out000.ts
#EXTINF:9.500000,
out001.ts
#EXTINF:8.375000,
out002.ts
#EXTINF:5.375000,
out003.ts
#EXTINF:9.000000,
out004.ts
...........
```

为了下载视频，得想办法把所有的ts切片文件下载下来，然后合成一个完整的视频。因为ts流命名规范有规律，非常容易下载，配合`xx.m3u8`播放列表文件，就可以直接用ffmpeg在线下载所有的视频并合并。直接执行这一句命令：

```
$ ffmpeg -i <m3u8-path> -c copy OUTPUT.mp4
$ ffmpeg -i <m3u8-path> -vcodec copy -acodec copy OUTPUT.mp4
```

而m3u8实际上是HLS直播协议，在之后的文章中会介绍到。但是我们完全可以利用VLC播放软件播放直播源，从而看到视频。

```
为什么下载播放列表就能下载所有的切片文件？
因为播放列表里的都是相对路径，既然我们有了播放列表的绝对路径，那么其它所有文件的绝对路径也就不难获取了。
好在ffmpeg直接实现了这种播放列表一键下载的方式。
```

当然部分网站可能会对视频流数据进行加密，从而导致ts文件不能合并。但是比如有key的话，同样可以通过key进行合并视频。
