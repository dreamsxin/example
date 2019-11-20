---
layout:     post
title:      FLV封装格式解析
subtitle:   视音频开发
date:       2019-08-12
author:     YunLambert
header-img: img/post-bg-FLV.jpg
catalog: true
tags:
    - Tips
    - Share
    - Media
---

> FLV封装格式解析

之前的博客里已经介绍了[音频码流AAC](http://blog.yunlambert.top/2019/08/09/AAC%E9%9F%B3%E9%A2%91%E7%A0%81%E6%B5%81%E8%A7%A3%E6%9E%90/)和[视频码流H.264](http://blog.yunlambert.top/2019/08/07/H.264%E8%A7%86%E9%A2%91%E7%A0%81%E6%B5%81%E8%A7%A3%E6%9E%90/)，接下来就是要把他们打包并封装格式了。

## FLV用途

FLV(Flash Video)是Adobe公司设计开发的一种流行的流媒体格式，由于其视频文件体积轻巧、封装简单等特点，使其很适合在互联网上进行应用。 除了播放视频，在直播时也可以使用。采用FLV格式封装的文件后缀为.flv

## FLV封装格式

FLV封装格式和其他格式一样，一个文件头+一个文件体。结构如下图：

![FLV format](https://upload-images.jianshu.io/upload_images/7154520-f3b90c1eb23021bc.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

### 文件头

FLV文件头总共有9Byte，文件头部的内容结构如下：

![header](https://upload-images.jianshu.io/upload_images/7154520-b3b9c5abdbefb7dd.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

```C++
typedef unsigned char byte;
typedef struct {
	byte Signature[3];
	byte Version;
	byte Flags;
	uint DataOffset;
} FLV_HEADER;
```

其中Signature[3]是存储了"F"(0x46)，"L"(0x4c)，“V”(0x56)签名的3个字节；然后是version版本，表示FLV的版本，占1个字节；Flags是音频流标识，前5位为保留位，第7位同样保留，全为0，第6位表示是否存在音频tag，第8位表示是否存在视频tag。剩下的4个字节为整个文件头所占的大小，数据的起始位置就是从文件开头偏移这么多的大小。 

#### 音频tag解析

Flags音频流标志位的第6位是判断是否存在音频tag，音频tag的结构如下：

![音频tag](https://upload-images.jianshu.io/upload_images/7154520-5c20c140912d0d68.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

音频tag开始的第一个字节包含了音频数据的参数信息，从第二个字节开始为音频流。如上图所示，在第1个字节中，前4个bit为音频编码类型：

```
0 Linear PCM，platform endian
1 ADPCM
2 MP3
3 Linear PCM，little endian
4 Nellymoser 16-kHz mono
5 Nellymoser 8-kHz mono
6 Nellymoser
7 G.711 A-law logarithmic PCM
8 G.711 mu-law logarithmic PCM
9 reserved
10 AAC
14 MP3 8-Khz
15 Device-specific sound
```

第5、6个bit表示音频采样率：

```
0 5.5kHz
1 11KHz
2 22 kHz
3 44 kHz
```

*FLV并不支持48KHz的采样率*

第7个bit表示音频采样精度：

```
0 8bits
1 16bits
```

第8个表示音频类型：

```
0 sndMono
1 sndStereo
```

#### 视频tag解析

Flags音频流标志位的第8位是判断是否存在视频tag，视频tag的结构如下：

![视频tag](https://upload-images.jianshu.io/upload_images/7154520-57ad1d0b9446d639.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

2个字节第一个字节包含视频数据的参数信息，第2个字节为视频流数据。第一个字节的前4个bit的数值表示帧类型：

```
1 keyframe （for AVC，a seekable frame）
2 inter frame （for AVC，a nonseekable frame）
3 disposable inter frame （H.263 only）
4 generated keyframe （reserved for server use）
5 video info/command frame
```

第1位的后4个bit的数值表示视频编码类型：

```
1 JPEG （currently unused）
2 Sorenson H.263
3 Screen video
4 On2 VP6
5 On2 VP6 with alpha channel
6 Screen video version 2
7 AVC
```

### 文件体

FLV文件体结构如下：

![body](https://upload-images.jianshu.io/upload_images/7154520-21487b126be31663.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

FLV文件体是由一连串的back-pointers和tags构成：

![body-pointers+tags](https://upload-images.jianshu.io/upload_images/7154520-5f90e93aa9d60d71.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

### Tag

这里的tag同样是由header和data两部分组成，tag header里存放的数据如下：

![tag header](https://upload-images.jianshu.io/upload_images/7154520-cb3ba1e5e9cb0930.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

Tag的类型可以是视频、音频和Script(脚本类型)，视频tag和音频tag在上面已经介绍过了，下面介绍一下Script Tag这种类型。

```C++
typedef unsigned char byte;
typedef unsigned int uint;
typedef struct {
	byte TagType;
	byte DataSize[3];
	byte Timestamp[3];
	uint Reserved;
} TAG_HEADER;
```



#### Script Tag

该类型Tag又被称为MetaData Tag,存放一些关于FLV视频和音频的元信息，比如：duration、width、height等。通常该类型Tag会作为FLV文件的第一个tag，并且只有一个，跟在File Header后。该类型Tag Data的结构如下所示:

![script tag](https://upload-images.jianshu.io/upload_images/7154520-50041ffdb536305d.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

针对第一个AMF包：第1个字节表示AMF类型，一般为0x02，表示字符串；第2、3个字节用来标识字符长度，一般总是0x000A。后面字节为具体的字符串。

针对第二个AMF包：第1个字节表示AMF包类型，一般总是0x08，表示数组。第2-5个字节表示数组元素个数，后面几位各数组元素的封装，数组元素为(名称-值)组成的对：

```
duration 时长
width 视频宽度
height 视频高度
videodatarate 视频码率
framerate 视频帧率
videocodecid 视频编码方式
audiosamplerate 音频采样率
audiosamplesize 音频采样精度
stereo 是否为立体声
audiocodecid 音频编码方式
filesize 文件大小
```

## FLV解析

来自雷神

```C++
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
//Important!
#pragma pack(1)
 
 
#define TAG_TYPE_SCRIPT 18
#define TAG_TYPE_AUDIO  8
#define TAG_TYPE_VIDEO  9
 
typedef unsigned char byte;
typedef unsigned int uint;
 
typedef struct {
	byte Signature[3];
	byte Version;
	byte Flags;
	uint DataOffset;
} FLV_HEADER;
 
typedef struct {
	byte TagType;
	byte DataSize[3];
	byte Timestamp[3];
	uint Reserved;
} TAG_HEADER;
 
 
//reverse_bytes - turn a BigEndian byte array into a LittleEndian integer
uint reverse_bytes(byte *p, char c) {
	int r = 0;
	int i;
	for (i=0; i<c; i++) 
		r |= ( *(p+i) << (((c-1)*8)-8*i));
	return r;
}
 
/**
 * Analysis FLV file
 * @param url    Location of input FLV file.
 */
 
int simplest_flv_parser(char *url){
 
	//whether output audio/video stream
	int output_a=1;
	int output_v=1;
	//-------------
	FILE *ifh=NULL,*vfh=NULL, *afh = NULL;
 
	//FILE *myout=fopen("output_log.txt","wb+");
	FILE *myout=stdout;
 
	FLV_HEADER flv;
	TAG_HEADER tagheader;
	uint previoustagsize, previoustagsize_z=0;
	uint ts=0, ts_new=0;
 
	ifh = fopen(url, "rb+");
	if ( ifh== NULL) {
		printf("Failed to open files!");
		return -1;
	}
 
	//FLV file header
	fread((char *)&flv,1,sizeof(FLV_HEADER),ifh);
 
	fprintf(myout,"============== FLV Header ==============\n");
	fprintf(myout,"Signature:  0x %c %c %c\n",flv.Signature[0],flv.Signature[1],flv.Signature[2]);
	fprintf(myout,"Version:    0x %X\n",flv.Version);
	fprintf(myout,"Flags  :    0x %X\n",flv.Flags);
	fprintf(myout,"HeaderSize: 0x %X\n",reverse_bytes((byte *)&flv.DataOffset, sizeof(flv.DataOffset)));
	fprintf(myout,"========================================\n");
 
	//move the file pointer to the end of the header
	fseek(ifh, reverse_bytes((byte *)&flv.DataOffset, sizeof(flv.DataOffset)), SEEK_SET);
 
	//process each tag
	do {
 
		previoustagsize = _getw(ifh);
 
		fread((void *)&tagheader,sizeof(TAG_HEADER),1,ifh);
 
		//int temp_datasize1=reverse_bytes((byte *)&tagheader.DataSize, sizeof(tagheader.DataSize));
		int tagheader_datasize=tagheader.DataSize[0]*65536+tagheader.DataSize[1]*256+tagheader.DataSize[2];
		int tagheader_timestamp=tagheader.Timestamp[0]*65536+tagheader.Timestamp[1]*256+tagheader.Timestamp[2];
 
		char tagtype_str[10];
		switch(tagheader.TagType){
		case TAG_TYPE_AUDIO:sprintf(tagtype_str,"AUDIO");break;
		case TAG_TYPE_VIDEO:sprintf(tagtype_str,"VIDEO");break;
		case TAG_TYPE_SCRIPT:sprintf(tagtype_str,"SCRIPT");break;
		default:sprintf(tagtype_str,"UNKNOWN");break;
		}
		fprintf(myout,"[%6s] %6d %6d |",tagtype_str,tagheader_datasize,tagheader_timestamp);
 
		//if we are not past the end of file, process the tag
		if (feof(ifh)) {
			break;
		}
 
		//process tag by type
		switch (tagheader.TagType) {
 
		case TAG_TYPE_AUDIO:{ 
			char audiotag_str[100]={0};
			strcat(audiotag_str,"| ");
			char tagdata_first_byte;
			tagdata_first_byte=fgetc(ifh);
			int x=tagdata_first_byte&0xF0;
			x=x>>4;
			switch (x)
			{
			case 0:strcat(audiotag_str,"Linear PCM, platform endian");break;
			case 1:strcat(audiotag_str,"ADPCM");break;
			case 2:strcat(audiotag_str,"MP3");break;
			case 3:strcat(audiotag_str,"Linear PCM, little endian");break;
			case 4:strcat(audiotag_str,"Nellymoser 16-kHz mono");break;
			case 5:strcat(audiotag_str,"Nellymoser 8-kHz mono");break;
			case 6:strcat(audiotag_str,"Nellymoser");break;
			case 7:strcat(audiotag_str,"G.711 A-law logarithmic PCM");break;
			case 8:strcat(audiotag_str,"G.711 mu-law logarithmic PCM");break;
			case 9:strcat(audiotag_str,"reserved");break;
			case 10:strcat(audiotag_str,"AAC");break;
			case 11:strcat(audiotag_str,"Speex");break;
			case 14:strcat(audiotag_str,"MP3 8-Khz");break;
			case 15:strcat(audiotag_str,"Device-specific sound");break;
			default:strcat(audiotag_str,"UNKNOWN");break;
			}
			strcat(audiotag_str,"| ");
			x=tagdata_first_byte&0x0C;
			x=x>>2;
			switch (x)
			{
			case 0:strcat(audiotag_str,"5.5-kHz");break;
			case 1:strcat(audiotag_str,"1-kHz");break;
			case 2:strcat(audiotag_str,"22-kHz");break;
			case 3:strcat(audiotag_str,"44-kHz");break;
			default:strcat(audiotag_str,"UNKNOWN");break;
			}
			strcat(audiotag_str,"| ");
			x=tagdata_first_byte&0x02;
			x=x>>1;
			switch (x)
			{
			case 0:strcat(audiotag_str,"8Bit");break;
			case 1:strcat(audiotag_str,"16Bit");break;
			default:strcat(audiotag_str,"UNKNOWN");break;
			}
			strcat(audiotag_str,"| ");
			x=tagdata_first_byte&0x01;
			switch (x)
			{
			case 0:strcat(audiotag_str,"Mono");break;
			case 1:strcat(audiotag_str,"Stereo");break;
			default:strcat(audiotag_str,"UNKNOWN");break;
			}
			fprintf(myout,"%s",audiotag_str);
 
			//if the output file hasn't been opened, open it.
			if(output_a!=0&&afh == NULL){
				afh = fopen("output.mp3", "wb");
			}
 
			//TagData - First Byte Data
			int data_size=reverse_bytes((byte *)&tagheader.DataSize, sizeof(tagheader.DataSize))-1;
			if(output_a!=0){
				//TagData+1
				for (int i=0; i<data_size; i++)
					fputc(fgetc(ifh),afh);
 
			}else{
				for (int i=0; i<data_size; i++)
					fgetc(ifh);
			}
			break;
		}
		case TAG_TYPE_VIDEO:{
			char videotag_str[100]={0};
			strcat(videotag_str,"| ");
			char tagdata_first_byte;
			tagdata_first_byte=fgetc(ifh);
			int x=tagdata_first_byte&0xF0;
			x=x>>4;
			switch (x)
			{
			case 1:strcat(videotag_str,"key frame  ");break;
			case 2:strcat(videotag_str,"inter frame");break;
			case 3:strcat(videotag_str,"disposable inter frame");break;
			case 4:strcat(videotag_str,"generated keyframe");break;
			case 5:strcat(videotag_str,"video info/command frame");break;
			default:strcat(videotag_str,"UNKNOWN");break;
			}
			strcat(videotag_str,"| ");
			x=tagdata_first_byte&0x0F;
			switch (x)
			{
			case 1:strcat(videotag_str,"JPEG (currently unused)");break;
			case 2:strcat(videotag_str,"Sorenson H.263");break;
			case 3:strcat(videotag_str,"Screen video");break;
			case 4:strcat(videotag_str,"On2 VP6");break;
			case 5:strcat(videotag_str,"On2 VP6 with alpha channel");break;
			case 6:strcat(videotag_str,"Screen video version 2");break;
			case 7:strcat(videotag_str,"AVC");break;
			default:strcat(videotag_str,"UNKNOWN");break;
			}
			fprintf(myout,"%s",videotag_str);
 
			fseek(ifh, -1, SEEK_CUR);
			//if the output file hasn't been opened, open it.
			if (vfh == NULL&&output_v!=0) {
				//write the flv header (reuse the original file's hdr) and first previoustagsize
					vfh = fopen("output.flv", "wb");
					fwrite((char *)&flv,1, sizeof(flv),vfh);
					fwrite((char *)&previoustagsize_z,1,sizeof(previoustagsize_z),vfh);
			}
#if 0
			//Change Timestamp
			//Get Timestamp
			ts = reverse_bytes((byte *)&tagheader.Timestamp, sizeof(tagheader.Timestamp));
			ts=ts*2;
			//Writeback Timestamp
			ts_new = reverse_bytes((byte *)&ts, sizeof(ts));
			memcpy(&tagheader.Timestamp, ((char *)&ts_new) + 1, sizeof(tagheader.Timestamp));
#endif
 
 
			//TagData + Previous Tag Size
			int data_size=reverse_bytes((byte *)&tagheader.DataSize, sizeof(tagheader.DataSize))+4;
			if(output_v!=0){
				//TagHeader
				fwrite((char *)&tagheader,1, sizeof(tagheader),vfh);
				//TagData
				for (int i=0; i<data_size; i++)
					fputc(fgetc(ifh),vfh);
			}else{
				for (int i=0; i<data_size; i++)
					fgetc(ifh);
			}
			//rewind 4 bytes, because we need to read the previoustagsize again for the loop's sake
			fseek(ifh, -4, SEEK_CUR);
 
			break;
			}
		default:
 
			//skip the data of this tag
			fseek(ifh, reverse_bytes((byte *)&tagheader.DataSize, sizeof(tagheader.DataSize)), SEEK_CUR);
		}
 
		fprintf(myout,"\n");
 
	} while (!feof(ifh));
 
 
	_fcloseall();
 
	return 0;
}
```

此程序可以分离FLV中的视频码流和音频码流。

首先定义了两个header，FLV_HEADER和TAG_HEADER，然后`reverse_bytes()`是将大端存储的byte array翻转为小端存储的整型：

```C
uint reverse_bytes(byte *p, char c) {
	int r = 0;
	int i;
	for (i=0; i<c; i++)  //直接和char型的c比较是因为char是“短的int”
		r |= ( *(p+i) << (((c-1)*8)-8*i));
	return r;
}
```

他的用法是这样的：` reverse_bytes((byte *)&flv.DataOffset, sizeof(flv.DataOffset))`，然后用一个循环，将内存中的数据先后通过`|=`与整型进行比较，从而变成了小端存储的整型。

接着在simplest_flv_parser函数中先读文件头，再读文件体。然后一个do-while大循环，直到读取到文件结尾。利用 _getw()函数 （是查找码流中的整数的）读取Previous tag size，然后`fread((void *)&tagheader,sizeof(TAG_HEADER),1,ifh);`读取tag header，再获取tag data的字节数:`        int tagheader_datasize=tagheader.DataSize[0]*pow(2, 16)+tagheader.DataSize[1]*pow(2, 8)+tagheader.DataSize[2];`，获取时戳：`int tagheader_timestamp=tagheader.Timestamp[0]*pow(2, 16)+tagheader.Timestamp[1]*pow(2, 8)+tagheader.Timestamp[2];`

接着就在之前读取的tag header中tag typr所在的字节中，进行switch判断：

如果是**音频**：

```C
case TAG_TYPE_AUDIO:{  //音频
    char audiotag_str[100]={0};
    strcat(audiotag_str,"| ");
    char tagdata_first_byte;

    //读取一个字符，音频tag data区域的第一个字节，音频的信息
    tagdata_first_byte=fgetc(ifh);
    // &操作获取前四位，代表音频格式
    int x=tagdata_first_byte&0xF0;
    //右移4位
    x=x>>4;
    //判断音频格式
    switch (x)
    {
        case 0:strcat(audiotag_str,"Linear PCM, platform endian");break;
        case 1:strcat(audiotag_str,"ADPCM");break;
        case 2:strcat(audiotag_str,"MP3");break;
        case 3:strcat(audiotag_str,"Linear PCM, little endian");break;
        case 4:strcat(audiotag_str,"Nellymoser 16-kHz mono");break;
        case 5:strcat(audiotag_str,"Nellymoser 8-kHz mono");break;
        case 6:strcat(audiotag_str,"Nellymoser");break;
        case 7:strcat(audiotag_str,"G.711 A-law logarithmic PCM");break;
        case 8:strcat(audiotag_str,"G.711 mu-law logarithmic PCM");break;
        case 9:strcat(audiotag_str,"reserved");break;
        case 10:strcat(audiotag_str,"AAC");break;
        case 11:strcat(audiotag_str,"Speex");break;
        case 14:strcat(audiotag_str,"MP3 8-Khz");break;
        case 15:strcat(audiotag_str,"Device-specific sound");break;
        default:strcat(audiotag_str,"UNKNOWN");break;
    }
    strcat(audiotag_str,"| ");

    //获取5~6位，采样率
    x=tagdata_first_byte&0x0C;
    //右移2位
    x=x>>2;
    //判断采样率
    switch (x)
    {
        case 0:strcat(audiotag_str,"5.5-kHz");break;
        case 1:strcat(audiotag_str,"1-kHz");break;
        case 2:strcat(audiotag_str,"22-kHz");break;
        case 3:strcat(audiotag_str,"44-kHz");break;
        default:strcat(audiotag_str,"UNKNOWN");break;
    }
    strcat(audiotag_str,"| ");

    //获取第7位，采样精度
    x=tagdata_first_byte&0x02;
    x=x>>1;
    switch (x)
    {
        case 0:strcat(audiotag_str,"8Bit");break;
        case 1:strcat(audiotag_str,"16Bit");break;
        default:strcat(audiotag_str,"UNKNOWN");break;
    }
    strcat(audiotag_str,"| ");

    //获取第8位，音频声道数
    x=tagdata_first_byte&0x01;
    switch (x)
    {
        case 0:strcat(audiotag_str,"Mono");break;
        case 1:strcat(audiotag_str,"Stereo");break;
        default:strcat(audiotag_str,"UNKNOWN");break;
    }
    fprintf(myout,"%s",audiotag_str);

    //if the output file hasn't been opened, open it.
    if(output_a!=0&&afh == NULL){
        afh = fopen("output.mp3", "wb");
    }

    //TagData - First Byte Data
    //获取tag Data字节数,需要减去Tag Data区域的第一个字节
    int data_size=reverse_bytes((byte *)&tagheader.DataSize, sizeof(tagheader.DataSize))-1;

    //循环获取字节写入文件
    if(output_a!=0){
        //TagData+1
        for (int i=0; i<data_size; i++)
            fputc(fgetc(ifh),afh);

    }else{
        for (int i=0; i<data_size; i++)
            fgetc(ifh);
    }
    break;
}
```

首先先读取文件头，再将音频流导出成mp3格式，看了上面的音频tag应该很好理解这段代码。

如果是视频：

```C
case TAG_TYPE_VIDEO:{   //视频
    char videotag_str[100]={0};
    strcat(videotag_str,"| ");
    //读取TagData区域第一个字节，取出前4位。包含视频帧类型
    char tagdata_first_byte;
    tagdata_first_byte=fgetc(ifh);
    int x=tagdata_first_byte&0xF0;
    x=x>>4;
    switch (x)
    {
        case 1:strcat(videotag_str,"key frame  ");break;
        case 2:strcat(videotag_str,"inter frame");break;
        case 3:strcat(videotag_str,"disposable inter frame");break;
        case 4:strcat(videotag_str,"generated keyframe");break;
        case 5:strcat(videotag_str,"video info/command frame");break;
        default:strcat(videotag_str,"UNKNOWN");break;
    }
    strcat(videotag_str,"| ");

    //读取TagData区域第一个字节，取出后4位。包含视频编码类型
    x=tagdata_first_byte&0x0F;
    switch (x)
    {
        case 1:strcat(videotag_str,"JPEG (currently unused)");break;
        case 2:strcat(videotag_str,"Sorenson H.263");break;
        case 3:strcat(videotag_str,"Screen video");break;
        case 4:strcat(videotag_str,"On2 VP6");break;
        case 5:strcat(videotag_str,"On2 VP6 with alpha channel");break;
        case 6:strcat(videotag_str,"Screen video version 2");break;
        case 7:strcat(videotag_str,"AVC");break;
        default:strcat(videotag_str,"UNKNOWN");break;
    }
    fprintf(myout,"%s",videotag_str);

    fseek(ifh, -1, SEEK_CUR);
    //if the output file hasn't been opened, open it.
    if (vfh == NULL&&output_v!=0) {
        //write the flv header (reuse the original file's hdr) and first previoustagsize
        vfh = fopen("output.flv", "wb");
        fwrite((char *)&flv,1, sizeof(flv),vfh);
        fwrite((char *)&previoustagsize_z,1,sizeof(previoustagsize_z),vfh);
    }
    #if 0
    //Change Timestamp
    //Get Timestamp
    ts = reverse_bytes((byte *)&tagheader.Timestamp, sizeof(tagheader.Timestamp));
    ts=ts*2;
    //Writeback Timestamp
    ts_new = reverse_bytes((byte *)&ts, sizeof(ts));
    memcpy(&tagheader.Timestamp, ((char *)&ts_new) + 1, sizeof(tagheader.Timestamp));
    #endif


    //TagData + Previous Tag Size
    int data_size=reverse_bytes((byte *)&tagheader.DataSize, sizeof(tagheader.DataSize))+4;
    if(output_v!=0){
        //TagHeader
        fwrite((char *)&tagheader,1, sizeof(tagheader),vfh);
        //TagData
        for (int i=0; i<data_size; i++)
            fputc(fgetc(ifh),vfh);
    }else{
        for (int i=0; i<data_size; i++)
            fgetc(ifh);
    }
    //rewind 4 bytes, because we need to read the previoustagsize again for the loop's sake
    fseek(ifh, -4, SEEK_CUR);

    break;
}
```

结果如下：

![flv output](https://upload-images.jianshu.io/upload_images/7154520-d11d24dfc6d77d19.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)


