//
//  ADmobBanner.m
//  LiftStatistics
//
//  Created by cuibo on 7/16/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "ADmobBanner.h"
#import "GADBannerView.h"

@interface ADmobBanner()

//广告条大小
@property(assign, nonatomic)CGSize gadSize;

//添加到视图底部
- (void)addToView:(UIViewController *)viewControl;

@end


@implementation ADmobBanner

@synthesize gadSize = _gadSize;

//初始化并添加到指定视图控制器
- (id)initWithViewController:(UIViewController *)viewControl
{
    self = [super init];
    if (self) 
    {
        [self addToView:viewControl];
    }
    return self;
}

//添加到视图底部
- (void)addToView:(UIViewController *)viewControl
{
    //admob相关
    // 在屏幕底部创建标准尺寸的视图。
    self.gadSize = CGSizeMake(GAD_SIZE_320x50.width, GAD_SIZE_320x50.height);

    //bannerView_ = [[GADBannerView alloc]
    //               initWithFrame:CGRectMake(viewControl.view.frame.size.width-self.gadSize.width,
    //                                        viewControl.view.frame.size.height-self.gadSize.height,
    //                                        self.gadSize.width,
    //                                        self.gadSize.height)];
    
    bannerView_ = [[GADBannerView alloc]
                   initWithFrame:CGRectMake(0,
                                            viewControl.view.bounds.size.height-self.gadSize.height,
                                            viewControl.view.bounds.size.width,
                                            self.gadSize.height)];

    //底部,左右对其，自动宽度
    bannerView_.autoresizingMask = UIViewAutoresizingFlexibleTopMargin|
        UIViewAutoresizingFlexibleLeftMargin|UIViewAutoresizingFlexibleRightMargin|
        UIViewAutoresizingFlexibleWidth;

    // 指定广告的“单元标识符”，也就是您的 AdMob 发布商 ID。
    bannerView_.adUnitID = @"a151505c37c746d";

    // 告知运行时文件，在将用户转至广告的展示位置之后恢复哪个 UIViewController 
    // 并将其添加至视图层级结构。
    bannerView_.rootViewController = viewControl;
    [viewControl.view addSubview:bannerView_];

    //测试广告
    GADRequest *request = [GADRequest request];
    request.testDevices = [NSArray arrayWithObjects:
                           GAD_SIMULATOR_ID,                            // 模拟器
                           @"TEST_DEVICE_ID",                           // 测试 iOS 设备
                           nil];

    // 启动一般性请求并在其中加载广告。
    [bannerView_ loadRequest:[GADRequest request]];
/**/
}

//移除广告（admob6.2.1,程序切换到后台，仍然占用cpu，2％～8％，不知道是否是bug，移除后正常。）
- (void)removeAds
{
    [bannerView_ removeFromSuperview];
    bannerView_ = nil;
}

//隐藏／显示广告条
- (void)hiddenADmobBanner:(BOOL)hid
{
    bannerView_.hidden = hid;
}

@end

