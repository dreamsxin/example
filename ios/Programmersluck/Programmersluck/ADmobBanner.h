//
//  ADmobBanner.h
//  LiftStatistics
//
//  Created by cuibo on 7/16/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

@class GADBannerView;

@interface ADmobBanner : NSObject
{
    // 将其中一个声明为实例变量
    GADBannerView *bannerView_;
    //广告条大小
    CGSize _gadSize;
}

//广告条大小
@property(readonly, nonatomic)CGSize gadSize;

//初始化并添加到指定视图控制器底部
- (id)initWithViewController:(UIViewController *)viewControl;

//移除广告（admob6.2.1,程序切换到后台，仍然占用cpu，2％～8％，不知道是否是bug，移除后正常。所以程序切换到后台时移除掉广告）
- (void)removeAds;

//隐藏／显示广告条
- (void)hiddenADmobBanner:(BOOL)hid;

@end

