---
layout:     post
title:      PCM音频采样处理
subtitle:   视音频开发
date:       2019-08-01
author:     YunLambert
header-img: img/post-bg-PCM.jpg
catalog: true
tags:
    - Tips
    - Share
    - Media
---

> PCM音频采样处理

## 一些音频基础知识

PCM(Pulse Code Modulation)，脉冲编码调制。人耳听到的是模拟信号，PCM是把声音从模拟信号转化为数字信号的技术。原理是用一个固定的频率对模拟信号进行采样，采样后的信号在波形上看就像一串连续的幅值不一的脉冲，把这些脉冲的幅值按一定精度进行量化，这些量化后的数值被连续的输出、传输、处理或记录到存储介质中，所有这些组成了数字音频的产生过程(抽样、量化、编码三个过程)。

对声音进行采样、量化过程被称为脉冲编码调制（Pulse Code Modulation），简称PCM。**PCM数据是最原始的音频数据完全无损**，所以PCM数据虽然音质优秀但体积庞大，为了解决这个问题先后诞生了一系列的音频格式，这些音频格式运用不同的方法对音频数据进行压缩，其中有无损压缩（ALAC、APE、FLAC）和有损压缩（MP3、AAC、OGG、WMA）两种。 

PCM相当于是没有压缩的音频数据，在**播放音频**的时候，应用程序从存储介质中读取音频数据(MP3、WMV、AAC....)，经过解码后，最终送到音频驱动程序中的就是PCM数据；在**录制音频**的时候则是音频驱动不停地把采样所得的PCM数据送给应用程序，由应用程序完成压缩、存储等任务。

### 采样频率

采样频率是设备一秒钟内对模拟信号的采样次数，在主流的采集卡上分为：(8Khz的电话采样率就可以达到人的对话程度) 

```
22.05KHz：无线电广播；
44.1KHz：音频 CD，MP3等；
48KHz：miniDV、数字电视、DVD、电影和专业音频。
```

在16位声卡中有22KHz、44KHz等几级，其中，22KHz相当于普通FM广播的音质，44KHz已相当于CD音质了，目前的常用采样频率都不超过48KHz。 

### 量化精度(采样位数)

即 采样值或取样值（就是将采样样本幅度量化）。它是用来衡量声音波动变化的一个参数，也可以说是声卡的分辨率。它的数值越大，分辨率也就越高，所发出声音的能力越强。 采样位数比如8bit(现在少见)、16bit(常用)和24bit，指的是描述数字信号所使用的位数。 

### 和声道数

声道数：声音在录制或播放时在不同空间位置采集或回放的相互独立的音频信号。   单声道 采样数据为8位的短整数（short）；   双声道 采样数据为16位的整数，（int），高八位(左声道)和低八位(右声道)分别代表两个声道。 

### PCM文件所占容量

[时长]s * [采样率]Hz * [采样位数]bit * [声道数] / 8 = [文件大小]byte
```
某音频信号是采样率为8kHz、声道数、位宽为16bit，时长为1s，则音频数据的大小为：
1 * 8000 * 16 *2 = 256000 bit / 8 = 32000 byte / 1024 = 31.25 KB
```

### 音频帧

音频跟视频很不一样，视频每一帧就是一张图像，音频数据是流式的，不同的编码格式各自不同的编码标准 。

## 常用音频格式

### WAV

WAV格式是微软公司开发的一种声音文件格式，也叫波形声音文件，是最早的数字音频格式，被Windows平台及其应用程序广泛支持。
WAV格式支持许多压缩算法，支持多种音频位数、采样频率和声道，采用44.1kHz的采样频率，16位量化位数，因此WAV的音质与CD相差无几，但WAV格式对存储空间需求太大不便于交流和传播。
补充：无损格式，缺点：体积十分大！

### MP3

MP3的全称是Moving Picture Experts Group Audio Layer III。简单的说，MP3就是一种音频压缩技术，由于这种压缩方式的全称叫MPEG Audio Layer3，所以人们把它简称为MP3。
 MP3是利用 MPEG Audio Layer 3 的技术，将音乐以1:10 甚至 1:12 的压缩率，压缩成容量较小的file，换句话说，能够在音质丢失很小的情况下把文件压缩到更小的程度。而且还非常好的保持了原来的音质。
正是因为MP3体积小，音质高的特点使得MP3格式几乎成为网上音乐的代名词。每分钟音乐的MP3格式只有1MB左右大小，这样每首歌的大小只有3-4MB。使用MP3播放器对MP3文件进行实时的解压缩(解码)，这样，高品质的MP3音乐就播放出来了。
缺点：最高比特率320K，缺少高频部分，音质不高！

### AMR

全称Adaptive Multi-Rate 和 Adaptive Multi-Rate Wideband，主要用于移动设备的音频，压缩比比较大，但相对其他的压缩格式质量比较差，多用于人声，通话，效果还是很不错的。 

### Ogg

Ogg全称应该是OGG Vobis(ogg Vorbis) 是一种新的音频压缩格式，类似于MP3等现有的音乐格式。现在创建的OGG文件可以在未来的任何播放器上播放，因此，这种文件格式可以不断地进行大小和音质的改良，而不影响旧有的编码器或播放器。  补充：目前最好的有损格式之一，MP3部分支持，智能手机装软件部分可以支持，最高比特率500kbps。  

### AAC

AAC（Advanced Audio Coding），中文称为“高级音频编码”，出现于1997年，基于 MPEG-2的音频编码技术。
优点：相对于mp3，AAC格式的音质更佳，文件更小。
不足：AAC属于有损压缩的格式，与时下流行的APE、FLAC等无损格式相比音质存在“本质上”的差距。加之，目前传输速度更快的USB3.0和16G以上大容量MP3正在加速普及，也使得AAC头上“小巧”的光环不复存在了。
前景：以发展的眼光来看，正如“高清”正在被越来越多的人所接受一样，“无损”必定是未来音乐格式的绝对主流。AAC这种“有损”格式的前景不容乐观

 ### FLAC

FLAC即是Free Lossless Audio Codec的缩写，中文可解为无损音频压缩编码。
FLAC是一套著名的自由音频压缩编码，其特点是无损压缩。不同于其他有损压缩编码如MP3 及 AAC，它不会破任何原有的音频资讯，所以可以还原音乐光盘音质。现在它已被很多软件及硬件音频产品所支持。简而言之，FLAC与MP3相仿，但是是无损压缩的，也就是说音频以FLAC方式压缩不会丢失任何信息。这种压缩与Zip的方式类似，但是FLAC将给你更大的压缩比率，因为FLAC是专门针对音频的特点设计的压缩方式，并且你可以使用播放器播放FLAC压缩的文件，就象通常播放你的MP3文件一样。
补充：为无损格式，较ape而言，他体积大点，但是兼容性好，编码速度快，播放器支持更广。

 ## WAV和PCM的区别和联系

如果要对音频进行编辑(裁剪、插入、合成等)操作，通常是需要将音频文件解码称为WAV格式的音频文件或者PCM文件。

 ```
1.PCM（Pulse Code Modulation----脉码调制录音)。所谓PCM录音就是将声音等模拟信号变成符号化的脉冲列，再予以记录。PCM信号是由[1]、[0]等符号构成的数字信号，而未经过任何编码和压缩处理。与模拟信号比，它不易受传送系统的杂波及失真的影响。动态范围宽，可得到音质相当好的影响效果。也就是说，PCM就是没有压缩的编码方式，PCM文件就是采用PCM这种没有压缩的编码方式编码的音频数据文件。


2.WAV是由微软开发的一种音频格式。WAV符合 PIFF Resource Interchange File Format规范。所有的WAV都有一个文件头，这个文件头音频流的编码参数。WAV对音频流的编码没有硬性规定，除了PCM之外，还有几乎所有支持ACM规范的编码都可以为WAV的音频流进行编码。WAV也可以使用多种音频编码来压缩其音频流，不过我们常见的都是音频流被PCM编码处理的WAV，但这不表示WAV只能使用PCM编码，MP3编码同样也可以运用在WAV中，和AVI一样，只要安装好了相应的Decode，就可以欣赏这些WAV了。


在Windows平台下，基于PCM编码的WAV是被支持得最好的音频格式，所有音频软件都能完美支持，由于本身可以达到较高的音质的要求，因此，WAV也是音乐编辑创作的首选格式，适合保存音乐素材。因此，基于PCM编码的WAV被作为了一种中介的格式，常常使用在其他编码的相互转换之中，例如MP3转换成WMA。
 ```

## 一些应用

参考雷神的博客

### 分离PCM16LE双声道音频采样数据的左声道和右声道

PCM是格式，16代表采样位数是16bit；一个声道的一个采样值占用2Byte；"LE"代表Little Endian，代表2Byte采样值的存储方式为高位存在高地址中。

```C++
int simplest_pcm16le_split(char *url){
	FILE *fp=fopen(url,"r");
	FILE *fp1=fopen("output_l.pcm","w");
	FILE *fp2=fopen("output_r.pcm","w");
 
	unsigned char *sample=(unsigned char *)malloc(4);
 
	while(!feof(fp)){
		fread(sample,1,4,fp);
		//L
		fwrite(sample,1,2,fp1);
		//R
		fwrite(sample+2,1,2,fp2);
	}
 
	free(sample);
	fclose(fp);
	fclose(fp1);
	fclose(fp2);
	return 0;
}
```

当我们调用时：`simplest_pcm16le_split("NocturneNo2inEflat_44.1k_s16le.pcm");`

从代码中儿科可以看出，PCM16LE双声道数据中左声道和右声道的采样值是间隔存储的，每个采样值占用2Byte。

### 将PCMLE16双声道音频采样数据中左声道的音量下降一半

首先声明一个short类型的变量samplenum，这样就可以指向左声道的2Byte的数据，然后将该内存存储的数据大小除以2就可以使左声道的音量下降一半了。**16bit采样数据是通过short类型变量存储的，而8bit采样数据是通过unsigned char类型存储的。 **

```C++
int simplest_pcm16le_halfvolumeleft(char *url){
	FILE *fp=fopen(url,"rb+");
	FILE *fp1=fopen("output_halfleft.pcm","w");
 
	int cnt=0;
 
	unsigned char *sample=(unsigned char *)malloc(4);
 
	while(!feof(fp)){
		short *samplenum=NULL;
		fread(sample,1,4,fp);
 
		samplenum=(short *)sample;
		*samplenum=*samplenum/2;
		//L
		fwrite(sample,1,2,fp1);
		//R
		fwrite(sample+2,1,2,fp1);
 
		cnt++;
	}
	printf("Sample Cnt:%d\n",cnt);
 
	free(sample);
	fclose(fp);
	fclose(fp1);
	return 0;
}
```

### 将PCM16LE双声道音频采样数据的声音速度提高一倍

```C++
int simplest_pcm16le_doublespeed(char *url){
	FILE *fp=fopen(url,"r");
	FILE *fp1=fopen("output_doublespeed.pcm","w");
 
	int cnt=0;
 
	unsigned char *sample=(unsigned char *)malloc(4);
 
	while(!feof(fp)){
 
		fread(sample,1,4,fp);
 
		if(cnt%2!=0){
			//L
			fwrite(sample,1,2,fp1);
			//R
			fwrite(sample+2,1,2,fp1);
		}
		cnt++;
	}
	printf("Sample Cnt:%d\n",cnt);
 
	free(sample);
	fclose(fp);
	fclose(fp1);
	return 0;
}
```

### 将PCM16LE双声道音频采样数据转换为PCM8音频采样数据

```C++
int simplest_pcm16le_to_pcm8(char *url){
	FILE *fp=fopen(url,"r");
	FILE *fp1=fopen("output_8.pcm","w");
 
	int cnt=0;
 
	unsigned char *sample=(unsigned char *)malloc(4);
 
	while(!feof(fp)){
 
		short *samplenum16=NULL;
		char samplenum8=0;
		unsigned char samplenum8_u=0;
		fread(sample,1,4,fp);
		//(-32768-32767)
		samplenum16=(short *)sample;
		samplenum8=(*samplenum16)>>8;
		//(0-255)
		samplenum8_u=samplenum8+128;
		//L
		fwrite(&samplenum8_u,1,1,fp1);
 
		samplenum16=(short *)(sample+2);
		samplenum8=(*samplenum16)>>8;
		samplenum8_u=samplenum8+128;
		//R
		fwrite(&samplenum8_u,1,1,fp1);
		cnt++;
	}
	printf("Sample Cnt:%d\n",cnt);
 
	free(sample);
	fclose(fp);
	fclose(fp1);
	return 0;
}
```

### 将从PCM16LE单声道音频采样数据中截取一部分数据

```C++
int simplest_pcm16le_cut_singlechannel(char *url,int start_num,int dur_num){
	FILE *fp=fopen(url,"r");
	FILE *fp1=fopen("output_cut.pcm","w");
	FILE *fp_stat=fopen("output_cut.txt","w");
 
	unsigned char *sample=(unsigned char *)malloc(2);
 
	int cnt=0;
	while(!feof(fp)){
		fread(sample,1,2,fp);
		if(cnt>start_num&&cnt<=(start_num+dur_num)){
			fwrite(sample,1,2,fp1);
 
			short samplenum=sample[1];
			samplenum=samplenum*256;
			samplenum=samplenum+sample[0];
 
			fprintf(fp_stat,"%6d,",samplenum);
			if(cnt%10==0)
				fprintf(fp_stat,"\n",samplenum);
		}
		cnt++;
	}
 
	free(sample);
	fclose(fp);
	fclose(fp1);
	fclose(fp_stat);
	return 0;
}
```

### 将PCM16LE双声道音频采样数据转换为WAVE格式音频数据

WAV格式是一种RIFF格式的文件，基本格式如下：

![图片.png](https://upload-images.jianshu.io/upload_images/7154520-28ce65bb5215756b.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

```C++
/**
 * Convert PCM16LE raw data to WAVE format
 * @param pcmpath      Input PCM file.
 * @param channels     Channel number of PCM file.
 * @param sample_rate  Sample rate of PCM file.
 * @param wavepath     Output WAVE file.
 */
int simplest_pcm16le_to_wave(const char *pcmpath,int channels,int sample_rate,const char *wavepath)
{
 
	typedef struct WAVE_HEADER{  
		char         fccID[4];        
		unsigned   long    dwSize;            
		char         fccType[4];    
	}WAVE_HEADER;  
 
	typedef struct WAVE_FMT{  
		char         fccID[4];        
		unsigned   long       dwSize;            
		unsigned   short     wFormatTag;    
		unsigned   short     wChannels;  
		unsigned   long       dwSamplesPerSec;  
		unsigned   long       dwAvgBytesPerSec;  
		unsigned   short     wBlockAlign;  
		unsigned   short     uiBitsPerSample;  
	}WAVE_FMT;  
 
	typedef struct WAVE_DATA{  
		char       fccID[4];          
		unsigned long dwSize;              
	}WAVE_DATA;  
 
 
	if(channels==0||sample_rate==0){
    channels = 2;
    sample_rate = 44100;
	}
	int bits = 16;
 
    WAVE_HEADER   pcmHEADER;  
    WAVE_FMT   pcmFMT;  
    WAVE_DATA   pcmDATA;  
 
    unsigned   short   m_pcmData;
    FILE   *fp,*fpout;  
 
	fp=fopen(pcmpath, "r");
    if(fp == NULL) {  
        printf("open pcm file error\n");
        return -1;  
    }
	fpout=fopen(wavepath,   "w");
    if(fpout == NULL) {    
        printf("create wav file error\n");  
        return -1; 
    }        
	//WAVE_HEADER
    memcpy(pcmHEADER.fccID,"RIFF",strlen("RIFF"));                    
    memcpy(pcmHEADER.fccType,"WAVE",strlen("WAVE"));  
    fseek(fpout,sizeof(WAVE_HEADER),1); 
	//WAVE_FMT
    pcmFMT.dwSamplesPerSec=sample_rate;  
    pcmFMT.dwAvgBytesPerSec=pcmFMT.dwSamplesPerSec*sizeof(m_pcmData);  
    pcmFMT.uiBitsPerSample=bits;
    memcpy(pcmFMT.fccID,"fmt ",strlen("fmt "));  
    pcmFMT.dwSize=16;  
    pcmFMT.wBlockAlign=2;  
    pcmFMT.wChannels=channels;  
    pcmFMT.wFormatTag=1;  
 
    fwrite(&pcmFMT,sizeof(WAVE_FMT),1,fpout); 
 
    //WAVE_DATA;
    memcpy(pcmDATA.fccID,"data",strlen("data"));  
    pcmDATA.dwSize=0;
    fseek(fpout,sizeof(WAVE_DATA),SEEK_CUR);
 
    fread(&m_pcmData,sizeof(unsigned short),1,fp);
    while(!feof(fp)){  
        pcmDATA.dwSize+=2;
        fwrite(&m_pcmData,sizeof(unsigned short),1,fpout);
        fread(&m_pcmData,sizeof(unsigned short),1,fp);
    }  
 
    pcmHEADER.dwSize=44+pcmDATA.dwSize;
 
    rewind(fpout);
    fwrite(&pcmHEADER,sizeof(WAVE_HEADER),1,fpout);
    fseek(fpout,sizeof(WAVE_FMT),SEEK_CUR);
    fwrite(&pcmDATA,sizeof(WAVE_DATA),1,fpout);
	
	fclose(fp);
    fclose(fpout);
 
    return 0;
}
```


