# GPUImage 自定义实时滤镜

## 下载

```shell
git clone https://github.com/BradLarson/GPUImage.git
```

## 引入GPUImage库

### 方法一，工程中以依赖的形式加入GPUImage

- 把framework文件夹拷贝到你工程目录下面,把GPUImage.xcodeproj拖入到工程中；
- 选中工程名 -> targets -> 工程名 -> BuildPhases -> Target Dependencies, 点加号，把GPUImage(GPUImage)加上；
- 选中工程名 -> targets -> 工程名 -> General -> Linked Frameworks and Libraries，点加号，把libGPUImage.a加上；
- 选中工程名 -> targets -> 工程名 -> Build Settings -> Header Search Paths, 把GPUImage/source中的相对路径加上，后面选recursive。
- 这样整个GPUImage 库就导入进来了，下一章节继续写如何在自定义camera中使用GPU Image。

## 方法二，pod 安装GPUImage

```conf
pod 'GPUImage'
pod install
```

## 用自定义相机加上GPUImage实时滤镜

新建一个Camera, 继承自GPUImageStillCamera（如果只想用静态而不拍摄视频）

里面主要负责自定义曝光，对焦，保存拍照完成后的图片, init方法可以这样写：

```object-c
- (id)initWithSessionPreset:(NSString *)sessionPreset cameraPosition:(AVCaptureDevicePosition)cameraPosition {
if (!(self = [super initWithSessionPreset:sessionPreset cameraPosition:cameraPosition]))
{
return nil;
}

_stillImageOutput = [[AVCaptureStillImageOutput alloc] init];
[_stillImageOutput setOutputSettings:@{ AVVideoCodecKey : AVVideoCodecJPEG }];
[self.captureSession addOutput:_stillImageOutput];
[self setFlashMode:AVCaptureFlashModeOff];
return self;
}
```

先调用父类的init方法，再添加一个AVCaptureStillImageOutput，用于存储静态图片, 这个是能保存原图的方法，如果只想保存加上滤镜后的图片，在controller里面可以这样调：

```object-c
[self.cameraManager capturePhotoAsImageProcessedUpToFilter:_groupFilter withCompletionHandler:^(UIImage *processedImage, NSError *error) {
        [self captureImageDidFinish:processedImage];
    }];
```

返回的processedImage就是加上_groupFilter滤镜效果的图。

### 自定义一个cameraView

写一个cameraView， 可以分成三部分，topView, 中间显示的preView, 和下面的bottomView

### 再写一个CameraViewController

在viewDidLoad里面这样写
```object-c
if ([self.cameraManager initWithSessionPreset:AVCaptureSessionPresetPhoto cameraPosition:AVCaptureDevicePositionFront]) {
GPUImageOutput<GPUImageInput> *filter = [[GPUImageHueFilter alloc] init];
[self.cameraManager addTarget:filter];
GPUImageView *filterView = [[GPUImageView alloc] initWithFrame:CGRectMake(0, 0, WIDTH, HEIGHT - CameraBottomHeight - CameraFilterHeight)];
[filter addTarget:filterView];
self.cameraManager.outputImageOrientation = UIInterfaceOrientationPortrait;
[self.cameraManager startCameraCapture];
[self.view addSubView:self.cameraView];
[self.cameraView.preview addSubview:filterView];
}
```
cameraManager为继承自GPUImageStillCamera的自定义相机（想处理视频请继承自GPUImageVideoCamera），GPUImageHueFilter 为其中一个GPUImage自带的滤镜，filterView为实时预览的view, 把它加在cameraView中间的preview上。
这样打开相机后，实时预览里面应该会有绿色的Hue的效果。

## 自定义相机加上切换滤镜效果

滤镜视图采用collectionView，只用一行，水平滚动
写一个cameraFilterView, .h文件:

```object-c
#import <UIKit/UIKit.h>

@protocol XKCameraFilterViewDelegate;

@interface XKCameraFilterCollectionView : UICollectionView <UICollectionViewDataSource, UICollectionViewDelegate, UICollectionViewDelegateFlowLayout>

@property (strong, nonatomic) NSMutableArray *picArray;
@property (strong, nonatomic) id <XKCameraFilterViewDelegate> cameraFilterDelegate;
@end

@protocol XKCameraFilterViewDelegate <NSObject>

- (void)switchCameraFilter:(NSInteger)index;
@end
```

cameraFilterCollectionView的实现文件：
```object-c
#import "XKCameraFilterCollectionView.h"

@implementation XKCameraFilterCollectionView

- (id)initWithFrame:(CGRect)frame collectionViewLayout:(UICollectionViewLayout *)layout{
self = [super initWithFrame:frame collectionViewLayout:layout];
if (self) {
self.delegate = self;
self.dataSource = self;
}
return self;
}

#pragma mark - delegate
#pragma UICollectionView datasource
-(NSInteger)numberOfSectionsInCollectionView:(UICollectionView *)collectionView
{
return 1;
}

- (NSInteger)collectionView:(UICollectionView *)collectionView numberOfItemsInSection:(NSInteger)section{
return [_picArray count];
}

- (UICollectionViewCell *)collectionView:(UICollectionView *)collectionView cellForItemAtIndexPath:(NSIndexPath *)indexPath{
static NSString *identifier = @"cameraFilterCellID";
[collectionView registerClass:[UICollectionViewCell class] forCellWithReuseIdentifier:identifier];
UICollectionViewCell *cell = [collectionView dequeueReusableCellWithReuseIdentifier:identifier forIndexPath:indexPath];
UIImageView *imageView = [[UIImageView alloc]initWithFrame:CGRectMake(0, 0, cell.width, cell.height)];
imageView.image = [_picArray objectAtIndex:indexPath.row];
[cell addSubview:imageView];
cell.backgroundColor = [UIColor orangeColor];

return cell;
}

#pragma mark collecton flowlayout delegate
- (CGSize)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)collectionViewLayout sizeForItemAtIndexPath:(NSIndexPath *)indexPath{
return CGSizeMake((WIDTH - 4) / 5, (WIDTH - 4) / 5);
}

#pragma mark collectionView delegate
- (void)collectionView:(UICollectionView *)collectionView didSelectItemAtIndexPath:(NSIndexPath *)indexPath{
[_cameraFilterDelegate switchCameraFilter:indexPath.row];
}

-(BOOL)collectionView:(UICollectionView *)collectionView shouldSelectItemAtIndexPath:(NSIndexPath *)indexPath
{
return YES;
}

@end
```

写上flowlayout 的delegate方法，返回每一个cell的宽高，系统自动计算间距，再把collectionView的datasource和delegate的几个方法实现。
在controller中初始化：

```object-c
- (void)addCameraFilterView {
UICollectionViewFlowLayout *layout = [[UICollectionViewFlowLayout alloc] init];
layout.scrollDirection = UICollectionViewScrollDirectionHorizontal;
_cameraFilterView = [[XKCameraFilterCollectionView alloc] initWithFrame:CGRectMake(0, HEIGHT - CameraBottomHeight - (WIDTH-4)/5 - 4, WIDTH, (WIDTH-4)/5) collectionViewLayout:layout];
NSMutableArray *filterNameArray = [[NSMutableArray alloc] initWithCapacity:CameraFilterCount];
for (NSInteger index = 0; index < CameraFilterCount; index++) {
UIImage *image = [UIImage imageNamed:@"filter0"];
[filterNameArray addObject:image];
}
_cameraFilterView.cameraFilterDelegate = self;
_cameraFilterView.picArray = filterNameArray;
[self.view addSubview:_cameraFilterView];
}
```

在controller中实现XKCameraFilterViewDelegate的方法：

```object-c
#pragma mark cameraFilterView delegate
- (void)switchCameraFilter:(NSInteger)index {
[self.cameraManager removeAllTargets];
switch (index) {
case 0:
_filter = [[GPUImageBilateralFilter alloc] init];
break;
case 1:
_filter = [[GPUImageHueFilter alloc] init];
break;
case 2:
_filter = [[GPUImageColorInvertFilter alloc] init];
break;
case 3:
_filter = [[GPUImageSepiaFilter alloc] init];
break;
case 4: {
_filter = [[GPUImageGaussianBlurPositionFilter alloc] init];
[(GPUImageGaussianBlurPositionFilter*)_filter setBlurRadius:40.0/320.0];
}
break;
case 5:
_filter = [[GPUImageMedianFilter alloc] init];
break;
case 6:
_filter = [[GPUImageVignetteFilter alloc] init];
break;
case 7:
_filter = [[GPUImageKuwaharaRadius3Filter alloc] init];
break;
default:
_filter = [[GPUImageBilateralFilter alloc] init];
break;
}
[self.cameraManager addTarget:_filter];
if (_gpuImageView != nil) {
[_gpuImageView removeFromSuperview];
}
[_filter addTarget:_gpuImageView];
[self.cameraView.preview addSubview:_gpuImageView];
}
```

点击每个滤镜，会有实时预览效果，filterView可以左右滚动。