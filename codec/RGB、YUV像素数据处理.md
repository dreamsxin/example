---
layout:     post
title:      RGB、YUV像素数据处理
subtitle:   视音频开发
date:       2019-07-22
author:     YunLambert
header-img: img/post-bg-RGBandYUV.jpg
catalog: true
tags:
    - Tips
    - Share
    - Media
---

> RGB、YUV像素数据处理

在之前的一篇关于JPEG格式的文章里提到了YUV和RGB之间的变换，有读者后台回复我说没有理解，那么这次展开来讲一讲，并且会掺杂最近的视音频学习的一些内容。
## 什么是RGB、YUV

首先在上一篇关于JPEG编解码的文章中提到了YCbCr色彩空间(不用回去找原文，这里会简单总结一下)。YUV、RGB和YCbCr是色彩空间模型，而我们最常听到的BMP、PNG、JPEG、GIF都是文件存储格式。

RGB就是red green blue三原色，任何颜色都可以通过这三原色按不同比例混合出来。所以应用很广泛。

YUV则是亮度Y(灰度值)、色差信号U和V。我们人眼对色彩的亮度信息会比色差信息敏感，没有UV信息一样可以显示完整的图像，只不过是黑白的。在默认情况下是图像和视频压缩的标准。

YCbCr 其实是YUV经过缩放和偏移的翻版。其中Y与YUV 中的Y含义一致,Cb,Cr 同样都指色彩，只是在表示方法上不同而已。YCbCr其中Y是指亮度分量，Cb指蓝色色度分量，而Cr指红色色度分量。JPEG、MPEG均采用此格式。

**这三种色彩空间模型都可以通过格式相互转换。**

## RGB到YUV的转换

RGB到YUV的转换，就是将图像所有像素点的R、G、B分量转换到Y、U、V分量：

```
YUV和RGB的转换:
      Y = 0.299 R + 0.587 G + 0.114 B
      U = -0.1687 R - 0.3313 G + 0.5 B + 128
      V = 0.5 R - 0.4187 G - 0.0813 B + 128

      R = Y + 1.402 (V-128)
      G= Y - 0.34414 (U-128) - 0.71414 (V-128)
      B= Y + 1.772 (U-128)
```

既然能够转换，为什么还需要YUV和RGB两种色彩模型呢？因为对于图像显示器来说，它是通过RGB模型来显示图像的。而在传输图像数据时是使用YUV模型的，因为YUV模型可以节省带宽。所以就需要采集图像时将RGB模型转换到YUV模型，显示时再将YUV模型转换为RGB模型。

## 视频采样

从视频采集与处理的角度来说，一般的视频采集芯片输出的码流一般都是YUV数据流的形式，而从视频处理（例如H.264、MPEG视频编解码）的角度来说，也是在原始YUV码流进行编码和解析 ；如果采集的资源时RGB的，也需要转换成YUV。 

我们必须根据采样格式从码流中还原每个像素点的YUV值，YUV格式有两大类：planar和packed。对于planar的YUV格式，先连续存储所有像素点的Y，紧接着存储所有像素点的U，随后是所有像素点的V。对于packed的YUV格式，每个像素点的Y,U,V是连续交叉存储的。 

而YUV码流的存储格式其实与其采样的方式密切相关，主流的采样方式有三种，YUV4:4:4，YUV4:2:2，YUV4:2:0 。

## YUV采样格式

### YUV 4:4:4采样

意味着Y、U、V三个分量的采样比例相同，所以在生成的图像里，每个像素的三个分量信息都是8bit。（Y用×表示，UV用○表示）

![图片.png](https://upload-images.jianshu.io/upload_images/7154520-0865d1961383452f.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

```
假如图像像素为：[Y0 U0 V0]、[Y1 U1 V1]、[Y2 U2 V2]、[Y3 U3 V3]

那么采样的码流为：Y0 U0 V0 Y1 U1 V1 Y2 U2 V2 Y3 U3 V3 

最后映射出的像素点依旧为 [Y0 U0 V0]、[Y1 U1 V1]、[Y2 U2 V2]、[Y3 U3 V3] 
```

这种采样方式和RGB颜色模型的图像大小是一样的，并没有达到节省带宽的目的。

### YUV 4:2:2采样

UV分量是Y分量的一般，Y分量和UV分量按照2:1的比例采样，如果水平方向有10个像素点，那么采样了10个Y分量，就只采样了5个UV分量。

![图片.png](https://upload-images.jianshu.io/upload_images/7154520-e1a4d4d447d1ae5d.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

```
假如图像像素为：[Y0 U0 V0]、[Y1 U1 V1]、[Y2 U2 V2]、[Y3 U3 V3]

 那么采样的码流为：Y0 U0 Y1 V1 Y2 U2 Y3 V3 

 其中，每采样过一个像素点，都会采样其 Y 分量，而 U、V 分量就会间隔一个采集一个。

 最后映射出的像素点为 [Y0 U0 V1]、[Y1 U0 V1]、[Y2 U2 V3]、[Y3 U2 V3]
```

通过这个例子就可以发现第一个像素点和第二个像素点共用了[U0、V1]分量，第三个像素点和第四个像素点共用了[U2、V3]分量，这样就节省了图像空间。比如一张1280*720大小的图片，如果按照RGB方式存储，会耗费：

$$(1280*720*8+1280*720*8+1280*720*8)、8/1024/1024=2.637MB$$

其中1280*720是表示有多少个像素点。但如果采用了YUV4:2:采样格式：

$$（1280 * 720 * 8 + 1280 * 720 * 0.5 * 8 * 2）/ 8 / 1024 / 1024 = 1.76 MB $$

节省了1/3的存储空间，适合进行图像传输。

### YUV 4:2:0采样

YUV 4:2:0 采样，并不是指只采样 U 分量而不采样 V 分量。而是指，在每一行扫描时，只扫描一种色度分量（U 或者 V），和 Y  分量按照 2 : 1 的方式采样。比如，第一行扫描时，YU 按照 2 : 1 的方式采样，那么第二行扫描时，YV 分量按照 2:1  的方式采样。对于每个色度分量来说，它的水平方向和竖直方向的采样和 Y 分量相比都是 2:1 。 

![图片.png](https://upload-images.jianshu.io/upload_images/7154520-c0a4ef48a24c1fa7.png?imageMogr2/auto-orient)

```
假设图像像素为：
 
[Y0 U0 V0]、[Y1 U1 V1]、 [Y2 U2 V2]、 [Y3 U3 V3]
[Y4 U4 V4]、[Y5 U5 V5]、[Y6 U6 V6]、 [Y7 U7 V7]
 
那么采样的码流为：Y0 U0 Y1 Y2 U2 Y3 Y4 V4 Y5 Y6 V6 Y7
 
其中，每采样过一个像素点，都会采样其 Y 分量，而 U、V 分量就会间隔一行按照 2 : 1 进行采样。
 
最后映射出的像素点为：

[Y0 U0 V5]、[Y1 U0 V5]、[Y2 U2 V7]、[Y3 U2 V7]
[Y5 U0 V5]、[Y6 U0 V5]、[Y7 U2 V7]、[Y8 U2 V7]
```

通过YUV 4:2:0采样后的图片大小为：

$$（1280 * 720 * 8 + 1280 * 720 * 0.25 * 8 * 2）/ 8 / 1024 / 1024 = 1.32 MB $$

采样的图像比RGB模型图像节省了一半的存储空间，因此也是比较主流的采样方式。

## YUV存储格式

前面简单提到了YUV的存储格式有两种：

    planar 平面格式
        指先连续存储所有像素点的 Y 分量，然后存储 U 分量，最后是 V 分量。
    packed 打包模式
        指每个像素点的 Y、U、V 分量是连续交替存储的。
根据采样方式和存储格式的不同，就有了多种 YUV 格式。这些格式主要是基于 YUV 4:2:2 和 YUV 4:2:0 采样。 

### 基于YUV 4:2:2采样的格式

YUV 4:2:2 采样规定了 Y 和 UV 分量按照 2: 1 的比例采样，两个 Y 分量公用一组 UV 分量。 

#### YUYV格式

YUYV 格式是采用打包格式进行存储的，指每个像素点都采用 Y 分量，但是每隔一个像素采样它的 UV 分量，排列顺序如下：
```
Y0 UO Y1 V0  Y2 U2 Y3 V2
```
Y0 和 Y1 公用 U0 V0 分量，Y2 和 Y3 公用 U2 V2 分量.

![图片.png](https://upload-images.jianshu.io/upload_images/7154520-18725c14d3f88cff.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

#### UYVY格式

UYVY 格式也是采用打包格式进行存储，它的顺序和 YUYV 相反，先采用 U 分量再采样 Y 分量，排列顺序如下：

```
U0 Y0 V0 Y1 U2 Y2 V2 Y3
```

Y0 和 Y1 公用 U0 V0 分量，Y2 和 Y3 公用 U2 V2 分量。

![图片.png](https://upload-images.jianshu.io/upload_images/7154520-e445d3ca51d2f08a.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

#### YUV 422P格式

YUV 422P 格式，又叫做 I422，采用的是平面格式进行存储，先存储所有的 Y 分量，再存储所有的 U 分量，再存储所有的 V 分量 

### 基于YUV 4:2:0采样的格式

基于 YUV 4:2:0 采样的格式主要有 YUV 420P 和 YUV 420SP 两种类型 。YUV 420P 和 YUV 420SP 都是基于  Planar 平面模式 进行存储的，先存储所有的 Y 分量后， YUV420P 类型就会先存储所有的 U 分量或者 V 分量，而 YUV420SP 则是按照 UV 或者 VU 的交替顺序进行存储 。所以针对这两种类型的格式就为YU12格式、YV112格式和NV12格式、NV21格式。

#### YU12和YV12格式

YU12 和 YV12 格式都属于 YUV 420P 类型，即先存储 Y 分量，再存储 U、V 分量，区别在于：YU12 是先 Y 再 U 后 V，而 YV12 是先 Y 再 V 后 U 。 

![YU12格式](https://upload-images.jianshu.io/upload_images/7154520-92f42b3a01bdf874.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

#### NV12和NV21格式

NV12 和 NV21 格式都属于 YUV420SP 类型。它也是先存储了 Y 分量，但接下来并不是再存储所有的 U 或者 V 分量，而是把 UV 分量交替连续存储。

NV12 是 IOS 中有的模式，它的存储顺序是先存 Y 分量，再 UV 进行交替存储。

![NV12格式](https://upload-images.jianshu.io/upload_images/7154520-02e159b49a7a508f.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

## RGB格式

> RGB565    每个像素用16位表示，RGB分量分别使用5位、6位、5位
>
> RGB555    每个像素用16位表示，RGB分量都使用5位（剩下1位不用）
>
> RGB24    每个像素用24位表示，RGB分量各使用8位
>
> RGB32    每个像素用32位表示，RGB分量各使用8位（剩下8位不用）
>
> ARGB32    每个像素用32位表示，RGB分量各使用8位（剩下的8位用于表示Alpha通道值）

在这里介绍一下常用的格式：

RGB565，使用16位表示一个像素，5位用于R，6位用于G，5位用于B。

RGB24，使用24位表示一个像素，RGB分量都用8位表示，取值范围为0-255，排列顺序为：BGR

```C++
typedef struct tagRGBTRIPLE {
    BYTE rgbtBlue; // 蓝色分量
    BYTE rgbtGreen; // 绿色分量
    BYTE rgbtRed; // 红色分量
} RGBTRIPLE;
```

## 一些应用

这里参考了雷神的博客。

### 分离YUV420P像素数据中的Y、U、V分量

首先建立一个函数能够将根据格式将YUV420P格式的文件分离成三个分量的文件：

    output_420_y.y：纯Y数据，分辨率为256x256。
    output_420_u.y：纯U数据，分辨率为128x128。
    output_420_v.y：纯V数据，分辨率为128x128。
函数如下：

```C++
int yuv420_split(const char *path, int w, int h, int num)
{
    FILE *fp = fopen(path, "r");
    if (fp < 0)
        cerr << "File doesn't exist.";

    FILE *fy, *fu, *fv;
    fy = fopen("output_420_y.y", "w");
    fu = fopen("output_420_u.y", "w");
    fv = fopen("output_420_v.y", "w");
    if (fv < 0 || fy < 0 || fu < 0)
        cerr << "Cannot create file.";

    //YUV420:Y->w*h + U->w*h*1/4 + V->w*h*1/4
    unsigned char *pic = (unsigned char*)malloc(w * h * 3 / 2);
    for (int i = 0; i < num; ++i)
    {
        fread(pic, 1, w * h * 3 / 2, fp);
        fwrite(pic, 1, w*h, fy);
        fwrite(pic + w*h, 1, w*h * 1 / 4, fu);
        fwrite(pic + w * h * 5 / 4,1, w*h * 1 / 4, fv);
    }

    free(pic);
    fclose(fp);
    fclose(fy);
    fclose(fu);
    fclose(fv);
    
    return 0;
}
```

得到的效果图就是下图：

![图片.png](https://upload-images.jianshu.io/upload_images/7154520-606b6682fd11a3e8.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

### 分离YUV444P像素数据中的Y、U、V分量

这就相当于提取一个根据RGB相同大小的像素数据转换而来的Y、U、V三个分量,稍微更改一下读取的位置就行了，函数如下:

```C++
int yuv444_split(const char *path, int w, int h, int num)
{
    FILE *fp = fopen(path, "r");
    if (fp < 0)
        cerr << "File doesn't exist.";

    FILE *fy, *fu, *fv;
    fy = fopen("output_444_y.y", "w");
    fu = fopen("output_444_u.y", "w");
    fv = fopen("output_444_v.y", "w");
    if (fv < 0 || fy < 0 || fu < 0)
        cerr << "Cannot create file.";

    //YUV420:Y->w*h + U->w*h + V->w*h
    unsigned char *pic = (unsigned char*)malloc(w * h * 3);
    for (int i = 0; i < num; ++i)
    {
        int size = w * h;
        fread(pic, 1, size * 3 , fp);
        fwrite(pic, 1, size, fy);
        fwrite(pic +size, 1, size, fu);
        fwrite(pic + 2*size, 1, size, fv);
    }

    free(pic);
    fclose(fp);
    fclose(fy);
    fclose(fu);
    fclose(fv);

    return 0;
}
```

### 将YUV420P像素数据变为灰度图

这个只需要保存Y分量就可以了，最后保存的文件更改为.yuv格式即可。但是在生成.yuv文件的时候是需要uv分量的，这个只需要将U、V分量设置成128即可。

> 这是因为U、V是图像中的经过偏置处理的色度分量。色度分量在偏置处理前的取值范围是-128至127，这时候的无色对应的是“0”值。经过偏置后色度分量取值变成了0至255，因而此时的无色对应的就是128了。 

程序如下：

```C++
int yuv420_gray(const char *path, int w, int h, int num)
{
    FILE *fp = fopen(path, "r");
    if (fp < 0)
        cerr << "File doesn't exist.";

    FILE *fgray;
    fgray = fopen("output_420_gray.yuv", "w");
    if (fgray < 0)
        cerr << "Cannot create file.";

    unsigned char *pic = (unsigned char*)malloc(w * h * 3);
    for (int i = 0; i < num; ++i)
    {
        fread(pic, 1, w*h * 3 / 2, fp);
        fwrite(pic, 1, w*h, fgray);
        memset(pic + w * h, 128, w*h/2);
        fwrite(pic + w * h, 1, w*h * 1 / 2, fgray);
    }

    free(pic);
    fclose(fp);
    fclose(fgray);

    return 0;
}
```

效果图如下：

![图片.png](https://upload-images.jianshu.io/upload_images/7154520-4403681a9ca94fd0.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

### 将YUV420P像素数据的亮度减半

只需将亮度分量Y的数值减半就可以了，程序实现如下：

```C++
int yuv420_halfy(const char *path, int w, int h, int num)
{
    FILE *fp = fopen(path, "r");
    if (fp < 0)
        cerr << "File doesn't exist.";

    FILE *fhalfy;
    fhalfy = fopen("output_420_halfy.yuv", "w");
    if (fhalfy < 0)
        cerr << "Cannot create file.";

    unsigned char *pic = (unsigned char*)malloc(w * h * 3);
    for (int i = 0; i < num; ++i)
    {
        fread(pic, 1, w*h * 3 / 2, fp);
        for (int j = 0; j < w*h; ++j)
            pic[j] /= 2;
        fwrite(pic, 1, w*h, fhalfy);
        fwrite(pic + w * h, 1, w*h * 1 / 2, fhalfy);
    }

    free(pic);
    fclose(fp);
    fclose(fhalfy);

    return 0;
}
```

![图片.png](https://upload-images.jianshu.io/upload_images/7154520-37b83dfd0c18af82.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

### 在YUV420P像素数据的周围加上边框

修改YUV数据中特定位置(边框)的分量Y数值，给图像添加一个边框效果，程序如下：

```C++
int yuv420_border(const char *path, int w, int h, int border, int num)
{
    FILE *fp = fopen(path, "r");
    if (fp < 0)
        cerr << "File doesn't exist.";

    FILE *fborder;
    fborder = fopen("output_420_border.yuv", "w");
    if (fborder < 0)
        cerr << "Cannot create file.";

    unsigned char *pic = (unsigned char*)malloc(w * h * 3);
    for (int i = 0; i < num; ++i)
    {
        fread(pic, 1, w*h * 3 / 2, fp);
        for (int cy = 0; cy < h; ++cy)
        {
            for (int cx = 0; cx < w; ++cx)
            {
                if (cx<border || cx>w - border || cy<border || cy>h - border)
                    pic[cx + cy * w] = 255; //二维数组的线性表示
            }
        }
        fwrite(pic, 1, w*h*3/2, fborder);
    }

    free(pic);
    fclose(fp);
    fclose(fborder);

    return 0;
}
```

![图片.png](https://upload-images.jianshu.io/upload_images/7154520-59f4a9eb6795c62d.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

### 生成YUV420P格式的灰阶测试图

首先得确定灰阶测试图的亮度最小值和最大值、灰阶数量，然后生成相应的图，不妨设置成这样的取值：

![图片.png](https://upload-images.jianshu.io/upload_images/7154520-417d216850a19d9b.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

```C++
int yuv420_graybar(int w, int h, int ymin, int ymax, int barnum)
{
    int barwidth = w/barnum;
    unsigned lum;
    float dlum = (float)(ymax - ymin) / (float)(barnum - 1);
    unsigned char *data_y, *data_u, *data_v;

    int uv_width = w / 2;
    int uv_height = h / 2;

    data_y = (unsigned char *)malloc(w*h);
    data_u = (unsigned char *)malloc(uv_width*uv_height);
    data_v = (unsigned char *)malloc(uv_width*uv_height);

    FILE *fp;
    fp = fopen("output_yun420_graybar.yuv", "w");
    if (fp < 0)
        cerr << "Cannot create file.";

    //for (int i = 0; i < w / barwidth; ++i)
    //{
    //    lum = ymin + (char)(i*dlum);
    //}

    for (int i = 0; i < h; ++i)
        for (int j = 0; j < w; ++j)
            data_y[i*w + j] = ymin + (char)(j / barwidth * dlum);
    for (int i = 0; i < uv_height; ++i)
        for (int j = 0; j < uv_width; ++j)
        {
            data_u[i*uv_width + j] = 128;
            data_v[i*uv_width + j] = 128;
        }

    fwrite(data_y, 1, w*h, fp);
    fwrite(data_u, 1, uv_width*uv_height, fp);
    fwrite(data_v, 1, uv_width*uv_height, fp);
    fclose(fp);
    free(data_y);
    free(data_u);
    free(data_v);
    return 0;  
}
```

![图片.png](https://upload-images.jianshu.io/upload_images/7154520-5f9c22b64377d565.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

### 计算两个YUV420P像素数据的PSNR

PSNR通常用于图像质量评价，即计算受损图像与原始图像之间的区别，以此来评价受损图像的质量，其计算公式为：

$$PSNR = 10*log_{10}(\frac{255^{2}}{MSE})$$

其中MSE的定义为：

$$MSE = \frac{1}{M*N}\sum{\sum(x_{ij}-y_{ij})^{2}}$$

M，N分别为图像的宽高，xij和yij分别为两张图像的每一个像素值。 用程序来直接计算两者之间的区别：

```C++
int yuv420_psnr(const char *path1, const char *path2, int w, int h, int num)
{
    FILE *f1, *f2;
    f1 = fopen(path1, "r");
    f2 = fopen(path2, "r");
    if (f1 < 0 || f2 < 0) cerr << "Error when opening files";
    unsigned char *pic1 = (unsigned char *)malloc(w*h);
    unsigned char *pic2 = (unsigned char *)malloc(w*h);

    for (int i = 0; i < num; i++) 
    {
        if (fread(pic1, 1, w*h, f1)<0) cerr<<"Error when reading file1";
        if (fread(pic2, 1, w*h, f2)<0) cerr<<"Error when reading file2";

        double mse_sum = 0, mse = 0, psnr = 0;
        for (int j = 0; j < w*h; j++) {
            mse_sum += pow((double)(pic1[j] - pic2[j]), 2);
        }
        mse = mse_sum / (w*h);
        psnr = 10 * log10(255.0*255.0 / mse);
        cout << psnr << endl;
    }
    free(pic1);
    free(pic2);
    fclose(f1);
    fclose(f2);
    return 0;
}
```

### 分离RGB24像素数据中的RGB

和分离YUV文件中的Y、U、V分量一样，不同的是RGB的排列顺序为R-G-B-R-G-B-...RGB24格式的每个像素的三个分量是连续存储的。一帧宽高分别为w、h的RGB24图像一共占用w*h*3 Byte的存储空间 

所以程序为：

```C++
int rgb24_split(const char *path, int w, int h, int num) {
    FILE *fp = fopen(path, "r");
    FILE *fp1 = fopen("output_r.y", "w");
    FILE *fp2 = fopen("output_g.y", "w");
    FILE *fp3 = fopen("output_b.y", "w");

    unsigned char *pic = (unsigned char *)malloc(w*h * 3);

    for (int i = 0; i < num; i++) {

        fread(pic, 1, w*h * 3, fp);

        for (int j = 0; j < w*h * 3; j = j + 3) {
            //R
            fwrite(pic + j, 1, 1, fp1);
            //G
            fwrite(pic + j + 1, 1, 1, fp2);
            //B
            fwrite(pic + j + 2, 1, 1, fp3);
        }
    }

    free(pic);
    fclose(fp);
    fclose(fp1);
    fclose(fp2);
    fclose(fp3);

    return 0;
}
```

![图片.png](https://upload-images.jianshu.io/upload_images/7154520-1822e424d7f197bb.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

![图片.png](https://upload-images.jianshu.io/upload_images/7154520-caf80dfd0c809042.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

### 将RGB24格式像素数据封装为BMP图像

BMP图像实际上存的就是RGB数据，所以需要程序实现对RGB像素数据的封装处理,具体实现起来就是在RGB数据前面加上文件头，同时将RGB数据中每个像素的"B"和"R"的位置互换，这是因为BMP文件是小端存储，在内存中存储的先后顺序为B-G-R。

#### BMP文件结构

BMP文件由4部分组成：

1. 位图文件头(bitmap-file header)
2. 位图信息头(bitmap-informationheader)
3. 颜色表(color table)
4. 颜色点阵数据(bits data)

24位真彩色位图没有颜色表，所以只有1、2、4这三部分。

位图文件头分4部分，共**14**字节：

|   **名称**    | **占用空间** | **内容**                                       | **实际数据**                                             |
| :-----------: | :----------: | ---------------------------------------------- | -------------------------------------------------------- |
|    bfType     |    2字节     | 标识，就是“BM”二字                             | BM                                                       |
|    bfSize     |    4字节     | 整个BMP文件的大小                              | 0x000C0036(786486)【与右键查看图片属性里面的大小值一样】 |
| bfReserved1/2 |    4字节     | 保留字，没用                                   | 0                                                        |
|   bfOffBits   |    4字节     | 偏移数，即 位图文件头+位图信息头+调色板 的大小 | 0x36(54)                                                 |

位图信息头共**40**字节：

| **名称**        | **占用空间** | **内容**                                                     | **实际数据** |
| --------------- | ------------ | ------------------------------------------------------------ | ------------ |
| **biSize**      | 4字节        | 位图信息头的大小，为40                                       | 0x28(40)     |
| **biWidth**     | 4字节        | 位图的宽度，单位是像素                                       | 0x200(512)   |
| **biHeight**    | 4字节        | 位图的高度，单位是像素                                       | 0x200(512)   |
| **biPlanes**    | 2字节        | 固定值1                                                      | 1            |
| **biBitCount**  | 2字节        | 每个像素的位数  			1-黑白图，4-16色，8-256色，24-真彩色 | 0x18(24)     |
| biCompression   | 4字节        | 压缩方式，BI_RGB(0)为不压缩                                  | 0            |
| biSizeImage     | 4字节        | 位图全部像素占用的字节数，BI_RGB时可设为0                    | 0x0C         |
| biXPelsPerMeter | 4字节        | 水平分辨率(像素/米)                                          | 0            |
| biYPelsPerMeter | 4字节        | 垂直分辨率(像素/米)                                          | 0            |
| biClrUsed       | 4字节        | 位图使用的颜色数  			如果为0，则颜色数为2的biBitCount次方 | 0            |
| biClrImportant  | 4字节        | 重要的颜色数，0代表所有颜色都重要                            | 0            |

颜色点阵数据

位图全部的像素，是按照自下向上，自左向右的顺序排列的。

RGB数据也是倒着念的，原始数据是按B、G、R的顺序排列的。

#### BMP文件程序结构

```C++
// 位图文件头
typedef  struct  tagBITMAPFILEHEADER
{ 
    unsigned short int  bfType;       //位图文件的类型，必须为BM 
    unsigned int       bfSize;       //文件大小，以字节为单位
    unsigned short int  bfReserverd1; //位图文件保留字，必须为0 
    unsigned short int  bfReserverd2; //位图文件保留字，必须为0 
    unsigned int       bfbfOffBits;  //位图文件头到数据的偏移量，以字节为单位
}BITMAPFILEHEADER; 

// 位图信息头
typedef  struct  tagBITMAPINFOHEADER 
{ 
    unsigned int biSize;                        //该结构大小，字节为单位
    int  biWidth;                     //图形宽度以象素为单位
    int  biHeight;                     //图形高度以象素为单位
    unsigned short biPlanes;               //目标设备的级别，必须为1 
    unsigned short  biBitcount;             //颜色深度，每个象素所需要的位数
    unsigned int  biCompression;        //位图的压缩类型
    unsigned int  biSizeImage;              //位图的大小，以字节为单位
    int  biXPelsPermeter;       //位图水平分辨率，每米像素数
    int  biYPelsPermeter;       //位图垂直分辨率，每米像素数
    unsigned int  biClrUsed;            //位图实际使用的颜色表中的颜色数
    unsigned int  biClrImportant;       //位图显示过程中重要的颜色数
}BITMAPINFOHEADER;

// RGB数据
...
```

#### RGB24->BMP

而实现RGB到BMP的程序如下：

```C++
int rgb24_to_bmp(const char* rgb24path, int width, int height, const char* bmppath)
{
    BmpHead m_header = { 0 };
    InfoHead m_infoHeader = { 0 };
    char bfType[2] = { 'B', 'M' };
    int header_size = sizeof(bfType) + sizeof(BmpHead) + sizeof(InfoHead);
    unsigned char* rgb24_buffer;
    FILE *f_rgb, *f_bmp;
    f_rgb = fopen(rgb24path, "r");
    if (f_rgb < 0) cerr << "Cannot open RGB file";
    f_bmp = fopen(bmppath, "w");
    if (f_bmp < 0) cerr << "Cannot create BMP file";

    rgb24_buffer = (unsigned char *)malloc(width*height * 3);
    if (fread(rgb24_buffer, 1, width*height * 3, f_rgb) < 0)
        cerr << "Error when reading RGB";
    m_header.imageSize = header_size + width * height *3; //1像素=8bit,1byte=8bit
    m_header.startPosition = header_size;
    m_infoHeader.biSize = sizeof(InfoHead);
    m_infoHeader.width = width;
    m_infoHeader.height = -height; // BMP存储像素是从下至上的
    m_infoHeader.colorPlane = 1;
    m_infoHeader.bitColor = 24; //24位真彩
    m_infoHeader.realSize = 3 * width*height;
    cout << sizeof(BmpHead) << endl;
    cout << sizeof(InfoHead) << endl;

    //for (int i = 0; i < height; ++i)
    //{
    //    for (int j = 0; j < width; ++j)
    //    {
    //        char temp = rgb24_buffer[(i*width + j) * 3 + 2];
    //        rgb24_buffer[(i*width + j) * 3 + 2] = rgb24_buffer[(i*width + j) * 3];
    //        rgb24_buffer[(i*width + j) * 3] = temp;
    //    }
    //}

    for (int i = 0; i < height*width; ++i)
    {
        unsigned char temp = *(rgb24_buffer+i*3);
        *(rgb24_buffer + i * 3) = *(rgb24_buffer + i * 3 + 2);
        *(rgb24_buffer + i * 3 + 2) = temp;
    }

    fwrite(bfType, 1, sizeof(bfType), f_bmp);
    fwrite(&m_header, 1, sizeof(m_header), f_bmp);
    fwrite(&m_infoHeader, 1, sizeof(m_infoHeader), f_bmp);
    //fwrite(rgb24_buffer, 1, sizeof(rgb24_buffer), f_bmp);
    fwrite(rgb24_buffer, 3*width*height, 1, f_bmp);


    fclose(f_rgb);
    fclose(f_bmp);
    free(rgb24_buffer);
    return 0;
}
```

![图片.png](https://upload-images.jianshu.io/upload_images/7154520-d081216d1f0572f7.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

生成的BMP文件如下，排查了很长时间但没有发现错误(汗-_-||)，出现这样子的颜色觉得可能是RGB互换的时候出现了问题，但是思路并没有出错。日后发现原因再补吧，这里留一个坑......

### 将RGB24格式像素数据转换为YUV420P格式像素数据

运用RGB和YUV的转换公式，需要注意的地方时RGB24的存储方式是Packed，YUV420P的存储方式也是Packed程序如下:

```C++
unsigned char clip_value(unsigned char x, unsigned char min_val, unsigned char  max_val) {
    if (x>max_val) {
        return max_val;
    }
    else if (x<min_val) {
        return min_val;
    }
    else {
        return x;
    }
}

//RGB to YUV420
bool RGB24_TO_YUV420(unsigned char *RgbBuf, int w, int h, unsigned char *yuvBuf)
{
    unsigned char *ptrY, *ptrU, *ptrV, *ptrRGB;
    memset(yuvBuf, 0, w*h * 3 / 2);
    ptrY = yuvBuf;
    ptrU = yuvBuf + w * h;
    ptrV = ptrU + (w*h * 1 / 4);
    unsigned char y, u, v, r, g, b;
    for (int j = 0; j<h; j++) {
        ptrRGB = RgbBuf + w * j * 3; //从RgbBuf的每一行的开头开始
        for (int i = 0; i<w; i++) {

            r = *(ptrRGB++);
            g = *(ptrRGB++);
            b = *(ptrRGB++);
            y = (unsigned char)((66 * r + 129 * g + 25 * b + 128) >> 8) + 16;
            u = (unsigned char)((-38 * r - 74 * g + 112 * b + 128) >> 8) + 128;
            v = (unsigned char)((112 * r - 94 * g - 18 * b + 128) >> 8) + 128;
            *(ptrY++) = clip_value(y, 0, 255);
            if (j % 2 == 0 && i % 2 == 0) {
                *(ptrU++) = clip_value(u, 0, 255);
            }
            else {
                if (i % 2 == 0) {
                    *(ptrV++) = clip_value(v, 0, 255);
                }
            }
        }
    }
    return true;
}


int rgb24_to_yuv420(const char *path1, int w, int h, int num, const char *path2) {
    FILE *fp = fopen(path1, "r");
    FILE *fp1 = fopen(path2, "w");

    unsigned char *pic_rgb24 = (unsigned char *)malloc(w*h * 3);
    unsigned char *pic_yuv420 = (unsigned char *)malloc(w*h * 3 / 2);

    for (int i = 0; i<num; i++) {
        fread(pic_rgb24, 1, w*h * 3, fp);
        RGB24_TO_YUV420(pic_rgb24, w, h, pic_yuv420);
        fwrite(pic_yuv420, 1, w*h * 3 / 2, fp1);
    }

    free(pic_rgb24);
    free(pic_yuv420);
    fclose(fp);
    fclose(fp1);

    return 0;
}
```

生成的YUV文件大小会缩小很多：

![图片.png](https://upload-images.jianshu.io/upload_images/7154520-84a5ee1422d07c6e.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)



**当然只涉及了RGB24和YUV420P格式的图像文件，所以如果格式不一样，需要更改代码。**
