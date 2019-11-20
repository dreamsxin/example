---
layout:     post
title:      AAC音频码流解析
subtitle:   视音频开发
date:       2019-08-09
author:     YunLambert
header-img: img/post-bg-AAC.jpg
catalog: true
tags:
    - Tips
    - Share
    - Media
---

> AAC音频码流解析

## 什么是AAC
AAC是Advanced Audio Coding的简写，它的定义可以参看wiki：

```
Advanced Audio Coding (AAC) is an audio coding standard for lossy digital audio compression. Designed to be the successor of the MP3 format, AAC generally achieves better sound quality than MP3 at the same bit rate.
```
在音质方面，由于采用多声道，和使用低复杂性 的描述方式，使其比几乎所有的传统编码方式在同规格的情况下更胜一筹. AAC可以支持多达48个音轨，15个低频（LFE）音轨，5.1多声道支持，更高的采样率（最高可达96kHz，音频CD为44.1kHz）和更高的采 样精度（支持8bit、16bit、24bit、32bit，音频CD为16bit）以及有多种语言的兼容能力，更高的解码效率。

AAC音频格式有ADIF和ADTS：

**ADIF：**Audio Data Interchange Format 音频数据交换格式。这种格式的特征是可以确定的找到这个音频数据的开始，不需进行在音频数据流中间开始的解码，即它的解码必须在明确定义的开始处进行。故这种格式常用在磁盘文件中。  

**ADTS**：Audio Data Transport Stream 音频数据传输流。这种格式的特征是它是一个有同步字的比特流，解码可以在这个流中任何位置开始。它的特征类似于mp3数据流格式。  

## 什么是ADTS

如果我们把AAC音频的ES流从FLV封装格式中单独提取出来送给硬件解码器时，是无法被正确解码的。因为一般解码器都需要把AAC的ES流打包成ADTS的格式。而ADTS 头中相对有用的信息有 **采样率、声道数、帧长度** ，如果我是解码器，直接丢给我一个ES流，我也不知道这是什么东西，所以必须要知道ADTS头中的这些东西。

### ES流、PS流、PES流、TS流

`ES流(Elementary Stream)`: 也叫基本码流,包含视频、音频或数据的连续码流。每个ES都由若干个存取单元(AU)组成，每个视频AU或音频AU都是：头部+编码数据 两部分，相当于编码的1幅视频图像或1个音频帧

`PES流(Packet Elementary Stream)`: 是用来传递ES的一种数据结构，也叫打包的基本码流, 是将基本的码流ES流根据需要分成长度不等的数据包, 并加上包头就形成了打包的基本码流PES流.

`PS流–Program Stream(节目流)`：PS流由PS包组成，而一个PS包又由若干个PES包组成（到这里，ES经过了两层的封装）。PS包的包头中包含了`同步信息`与`时钟恢复信息`。一个PS包最多可包含具有`同一时钟基准`的16个视频PES包和32个音频PES包。 PS流的包结构是可变长度的，一旦丢失某一PS包的同步信息，接收机就会进入失去同步状态。

`TS流(Transport Stream)`: 也叫传输流, 是由固定长度为`188字节`的包组成, 含有独立时基的一个或多个program, 一个program又可以包含多个视频、音频、和文字信息的ES流。TS流的包结构是固定长度的，一旦破坏了某一TS包的同步信息时，接收机可在固定的位置检测它后面包中的同步信息，从而恢复同步，避免了信息丢失。

### ADTS的格式

把AAC的ES流打包成ADTS 7个字节的ADTS header就可以了，而每个ADTS frame之间都是通过syncword(同步字：0xFFF)进行分隔的

|  ADTS AAC   |        |             |        |      |             |        |
| :---------: | ------ | ----------- | ------ | ---- | ----------- | ------ |
| ADTS_header | AAC ES | ADTS_header | AAC ES | ...  | ADTS_header | AAC ES |

![adts](https://upload-images.jianshu.io/upload_images/7154520-56174b9a2318006c.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

贴一段ffmpeg中添加ADTS头的代码：

```C++
int ff_adts_write_frame_header(ADTSContext *ctx,
                               uint8_t *buf, int size, int pce_size)
{
    PutBitContext pb;
 
    init_put_bits(&pb, buf, ADTS_HEADER_SIZE);
 
    /* adts_fixed_header */
    put_bits(&pb, 12, 0xfff);   /* syncword */
    put_bits(&pb, 1, 0);        /* ID */
    put_bits(&pb, 2, 0);        /* layer */
    put_bits(&pb, 1, 1);        /* protection_absent */
    put_bits(&pb, 2, ctx->objecttype); /* profile_objecttype */
    put_bits(&pb, 4, ctx->sample_rate_index);
    put_bits(&pb, 1, 0);        /* private_bit */
    put_bits(&pb, 3, ctx->channel_conf); /* channel_configuration */
    put_bits(&pb, 1, 0);        /* original_copy */
    put_bits(&pb, 1, 0);        /* home */
 
    /* adts_variable_header */
    put_bits(&pb, 1, 0);        /* copyright_identification_bit */
    put_bits(&pb, 1, 0);        /* copyright_identification_start */
    put_bits(&pb, 13, ADTS_HEADER_SIZE + size + pce_size); /* aac_frame_length */
    put_bits(&pb, 11, 0x7ff);   /* adts_buffer_fullness */
    put_bits(&pb, 2, 0);        /* number_of_raw_data_blocks_in_frame */
 
    flush_put_bits(&pb);
 
    return 0;
}
```

### AAC解码

关于AAC解码，推荐一个开源的FAAD:[http://www.audiocoding.com/faad2.html](http://www.audiocoding.com/faad2.html )

## 应用：分离ADTS frame并分析首部各个字段

来自雷神

```C++
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
int getADTSframe(unsigned char* buffer, int buf_size, unsigned char* data ,int* data_size){
	int size = 0;
 
	if(!buffer || !data || !data_size ){
		return -1;
	}
 
	while(1){
		if(buf_size  < 7 ){
			return -1;
		}
		//Sync words
		if((buffer[0] == 0xff) && ((buffer[1] & 0xf0) == 0xf0) ){
			size |= ((buffer[3] & 0x03) <<11);     //high 2 bit
			size |= buffer[4]<<3;                //middle 8 bit
			size |= ((buffer[5] & 0xe0)>>5);        //low 3bit
			break;
		}
		--buf_size;
		++buffer;
	}
 
	if(buf_size < size){
		return 1;
	}
 
	memcpy(data, buffer, size);
	*data_size = size;
 
	return 0;
}
 
int simplest_aac_parser(char *url)
{
	int data_size = 0;
	int size = 0;
	int cnt=0;
	int offset=0;
 
	//FILE *myout=fopen("output_log.txt","wb+");
	FILE *myout=stdout;
 
	unsigned char *aacframe=(unsigned char *)malloc(1024*5);
	unsigned char *aacbuffer=(unsigned char *)malloc(1024*1024);
 
	FILE *ifile = fopen(url, "rb");
	if(!ifile){
		printf("Open file error");
		return -1;
	}
 
	printf("-----+- ADTS Frame Table -+------+\n");
	printf(" NUM | Profile | Frequency| Size |\n");
	printf("-----+---------+----------+------+\n");
 
	while(!feof(ifile)){
		data_size = fread(aacbuffer+offset, 1, 1024*1024-offset, ifile);
		unsigned char* input_data = aacbuffer;
 
		while(1)
		{
			int ret=getADTSframe(input_data, data_size, aacframe, &size);
			if(ret==-1){
				break;
			}else if(ret==1){
				memcpy(aacbuffer,input_data,data_size);
				offset=data_size;
				break;
			}
 
			char profile_str[10]={0};
			char frequence_str[10]={0};
 
			unsigned char profile=aacframe[2]&0xC0;
			profile=profile>>6;
			switch(profile){
			case 0: sprintf(profile_str,"Main");break;
			case 1: sprintf(profile_str,"LC");break;
			case 2: sprintf(profile_str,"SSR");break;
			default:sprintf(profile_str,"unknown");break;
			}
 
			unsigned char sampling_frequency_index=aacframe[2]&0x3C;
			sampling_frequency_index=sampling_frequency_index>>2;
			switch(sampling_frequency_index){
			case 0: sprintf(frequence_str,"96000Hz");break;
			case 1: sprintf(frequence_str,"88200Hz");break;
			case 2: sprintf(frequence_str,"64000Hz");break;
			case 3: sprintf(frequence_str,"48000Hz");break;
			case 4: sprintf(frequence_str,"44100Hz");break;
			case 5: sprintf(frequence_str,"32000Hz");break;
			case 6: sprintf(frequence_str,"24000Hz");break;
			case 7: sprintf(frequence_str,"22050Hz");break;
			case 8: sprintf(frequence_str,"16000Hz");break;
			case 9: sprintf(frequence_str,"12000Hz");break;
			case 10: sprintf(frequence_str,"11025Hz");break;
			case 11: sprintf(frequence_str,"8000Hz");break;
			default:sprintf(frequence_str,"unknown");break;
			}
 
 
			fprintf(myout,"%5d| %8s|  %8s| %5d|\n",cnt,profile_str ,frequence_str,size);
			data_size -= size;
			input_data += size;
			cnt++;
		}   
 
	}
	fclose(ifile);
	free(aacbuffer);
	free(aacframe);
 
	return 0;
}
```

代码中的size是ADTS数据的长度，这个长度的值存储在ADTS头文件7个字节（58位）中的第31-43位中，共13位，因为一个字节占8位，所以这13位的数据分布在三个字节中，也就是第四个字节的最后两位、第五个字节的八位、第六个字节的前三位 


