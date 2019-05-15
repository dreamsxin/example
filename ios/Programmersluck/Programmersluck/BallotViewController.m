//
//  SecondViewController.m
//  Programmersluck
//
//  Created by cuibo on 3/15/13.
//  Copyright (c) 2013 cuibo. All rights reserved.
//

#import "BallotViewController.h"
#import "ExpandNSDate.h"
#import "ZCRandom.h"
#import "BallotItemViewController.h"
#import "ADmobBanner.h"
#import "MyAdvertisingView.h"

#define LuckRateSum 3120    //概率总和

typedef enum
{
    kCurrentStatusDidNotStart = 0,
    kCurrentStatusCompleted,
    kCurrentStatusAreBalloting,
}kCurrentStatus;

@interface BallotViewController ()

@property (strong, nonatomic) IBOutlet UIView *myOptionsSegmented;
@property(assign, nonatomic)NSInteger currentOptionsSegmented;  
@property(strong, nonatomic)IBOutlet UILabel *todayLabel;
@property(strong, nonatomic)IBOutlet UIView *ballotContainerView;
@property(strong, nonatomic)BallotItemViewController *frontBallot;
@property(strong, nonatomic)BallotItemViewController *backBallot;
@property(assign, nonatomic)NSInteger ballotNumber;
@property(assign, nonatomic)NSInteger currentStatus;

@property(strong, nonatomic)NSArray *results;
@property(strong, nonatomic)NSArray *luckRate;
@property(strong, nonatomic)NSArray *resultsColor;

@property(strong, nonatomic)ZCRandom *zcRandom;
@property(assign, nonatomic)NSInteger lastRandIndex;

//我的广告条（占位）
@property(strong, nonatomic)IBOutlet MyAdvertisingView *myAdmobBanner;

@end

@implementation BallotViewController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        self.title = NSLocalizedString(@"抽签", @"抽签");
        self.tabBarItem.image = [UIImage imageNamed:@"ticket"];
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
	// Do any additional setup after loading the view, typically from a nib.
    
    self.results = [[NSArray alloc] initWithObjects:@"超大吉", @"大吉", @"吉", @"小吉", @"未知", @"小胸", @"胸", @"大胸", @"超大胸", nil];
    self.resultsColor = [[NSArray alloc] initWithObjects:
                         [UIColor colorWithRed:1.0 green:0 blue:0 alpha:1],
                         [UIColor colorWithRed:0.9 green:0 blue:0 alpha:1],
                         [UIColor colorWithRed:0.8 green:0 blue:0 alpha:1],
                         [UIColor colorWithRed:0.7 green:0 blue:0 alpha:1],
                         [UIColor colorWithRed:0.6 green:0 blue:0 alpha:1],
                         [UIColor colorWithRed:0.5 green:0 blue:0 alpha:1],
                         [UIColor colorWithRed:0.4 green:0 blue:0 alpha:1],
                         [UIColor colorWithRed:0.3 green:0 blue:0 alpha:1],
                         [UIColor colorWithRed:0.2 green:0 blue:0 alpha:1],
                         nil];
    // 吉凶概率分布，总数为 LuckRateSum
    self.luckRate =[[NSArray alloc] initWithObjects:
                                [NSNumber numberWithInt:10],    //10
                                [NSNumber numberWithInt:110],   //100
                                [NSNumber numberWithInt:610],   //500
                                [NSNumber numberWithInt:1510],  //900
                                [NSNumber numberWithInt:1810],  //300
                                [NSNumber numberWithInt:2510],  //700
                                [NSNumber numberWithInt:3010],  //500
                                [NSNumber numberWithInt:3110],  //100
                                [NSNumber numberWithInt:3120],  //10
                                nil];
    self.zcRandom = [[ZCRandom alloc] init];
    unsigned long timeSince = [[NSDate date] timeIntervalSince1970];
    [self.zcRandom srandom:timeSince];

    //创建两个标签
    self.frontBallot = [[BallotItemViewController alloc] initWithNibName:@"BallotItemViewController" bundle:nil];
    self.frontBallot.itemLabel.font = [UIFont boldSystemFontOfSize:80];
    self.frontBallot.itemLabel.textColor = [UIColor whiteColor];
    self.frontBallot.itemLabel.textAlignment = NSTextAlignmentCenter;
    [self.ballotContainerView addSubview:self.frontBallot.view];
    //2
    self.backBallot = [[BallotItemViewController alloc] initWithNibName:@"BallotItemViewController" bundle:nil];
    self.backBallot.itemLabel.font = [UIFont boldSystemFontOfSize:80];
    self.backBallot.itemLabel.textColor = [UIColor whiteColor];
    self.backBallot.itemLabel.textAlignment = NSTextAlignmentCenter;
    [self.ballotContainerView addSubview:self.backBallot.view];

    //重置两个标签
    [self resetBallotView];
    
    //刷新广告
    [self updateAdsState];
    //显示广告占位
    //self.myAdmobBanner.hidden = YES;
}

- (void)viewDidAppear:(BOOL)animated
{
    self.todayLabel.text = [[NSDate date] getDateFormatterForStr:@"yyyy年MM月dd日\nEEEE"];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

//切换标签
- (IBAction)optionsSegmentedAction:(id)sender
{
    self.currentOptionsSegmented = ((UIView *)sender).tag;
    
    //修改标签颜色
    for(UIView *myControl in [self.myOptionsSegmented subviews])
    {
        if([myControl isKindOfClass:[UIControl class]])
        {
            //标签颜色，当前标签－白色。否则－黑色。
            UIColor *color;
            if(myControl == sender)
                color = [UIColor whiteColor];
            else
                color = [UIColor blackColor];

            UIView *myView = [myControl viewWithTag:10001];
            if([myView isKindOfClass:[UILabel class]])
            {
                UILabel *label = (UILabel *)myView;
                label.textColor = color;
            }
        }
    }
    
    //创建一个视图切换动画
    [UIView beginAnimations:NULL context:nil];
    [UIView setAnimationDelegate:self];
    //结束后条用animationMonthSlideComplete
    //[UIView setAnimationDidStopSelector:@selector(pushAnimationComplete)];
    [UIView setAnimationDuration:0.3];
    [UIView setAnimationCurve:UIViewAnimationCurveEaseInOut];
    //修改指示器位置
    UIView *pointView = [self.myOptionsSegmented viewWithTag:20001];
    pointView.frame = CGRectMake(pointView.frame.size.width * self.currentOptionsSegmented,
                                 pointView.frame.origin.y,
                                 pointView.frame.size.width,
                                 pointView.frame.size.height);
    //提交动画
    [UIView commitAnimations];
    
    [self resetBallotView];
}

//重置标签视图
- (void)resetBallotView
{
    //展示视图
    CGRect ballotViewframe = self.ballotContainerView.frame;
    CGRect ballotframe = CGRectMake(0, ballotViewframe.size.height,
                                    ballotViewframe.size.width, ballotViewframe.size.height);
    self.frontBallot.view.frame = ballotframe;
    self.backBallot.view.frame = ballotframe;

    //没开始
    self.currentStatus = kCurrentStatusDidNotStart;
}

//开始抽签
- (IBAction)startBallotButton:(id)sender
{
    //正在抽签则屏蔽
    if(self.currentStatus == kCurrentStatusAreBalloting)
    {
        return;
    }
    //刚抽完则清空抽签视图并返回
    if(self.currentStatus == kCurrentStatusCompleted)
    {
        [self resetBallotView];
        return;
    }
    //状态为正在抽签
    self.currentStatus = kCurrentStatusAreBalloting;
    [self optionsSegmentedEnabled:NO];

    //抽签次数
    self.ballotNumber = 20;
    self.lastRandIndex = -1;
    [self pushBallot];
}

//压入一个签
- (void)pushBallot
{
    //展示视图
    CGRect ballotViewframe = self.ballotContainerView.frame;

    //创建标签
    self.frontBallot.view.frame = CGRectMake(0, ballotViewframe.size.height,
                             ballotViewframe.size.width, ballotViewframe.size.height);
    //取一个签
    NSInteger randInt;
    randInt = [self.zcRandom random:LuckRateSum];
    for(NSInteger i=0; i<[self.luckRate count]; i++)
    {
        NSNumber *num = [self.luckRate objectAtIndex:i];
        if(randInt < [num integerValue])
        {
            //不能和上一次重复
            if(self.lastRandIndex != i)
            {
                self.frontBallot.itemLabel.text = [self.results objectAtIndex:i];
                self.frontBallot.itemLabel.textColor = [self.resultsColor objectAtIndex:i];
                self.lastRandIndex = i;
                break;
            }
            else
            {
                randInt = [self.zcRandom random:LuckRateSum];
            }
        }
    }

    //创建一个视图切换动画
    [UIView beginAnimations:NULL context:nil];
    [UIView setAnimationDelegate:self];
    //结束后条用animationMonthSlideComplete
    [UIView setAnimationDidStopSelector:@selector(pushAnimationComplete)];
    CGFloat duration;
    //小于10则开始减速
    if(self.ballotNumber > 10)
    {
        duration = 0.2;
    }
    else
    {
        duration = (1.2-self.ballotNumber/10.0);
    }
    [UIView setAnimationDuration:duration];
    [UIView setAnimationCurve:UIViewAnimationCurveEaseInOut];

    self.frontBallot.view.frame = CGRectMake(0, 0, ballotViewframe.size.width, ballotViewframe.size.height);
    
    //提交动画
    [UIView commitAnimations];
}

//压入动画结束
- (void)pushAnimationComplete
{
    if(self.ballotNumber > 0)
    {
        self.ballotNumber --;
        BallotItemViewController *tBallot = self.backBallot;
        self.backBallot = self.frontBallot;
        self.frontBallot = tBallot;
        //交换层次
        int currentIndex = [self.ballotContainerView.subviews indexOfObject:self.frontBallot.view];
        [self.ballotContainerView exchangeSubviewAtIndex:currentIndex withSubviewAtIndex:currentIndex+1];
        [self pushBallot];
    }
    else
    {
        //状态为完成
        self.currentStatus = kCurrentStatusCompleted;
        [self optionsSegmentedEnabled:YES];
    }
}

//开启和禁用标签
- (void)optionsSegmentedEnabled:(BOOL)enadled
{
    self.myOptionsSegmented.userInteractionEnabled = enadled;
    //修改标签颜色
    for(UIView *myControl in [self.myOptionsSegmented subviews])
    {
        if([myControl isKindOfClass:[UIControl class]])
        {
            //标签颜色，当前标签－白色。否则－黑色。
            UIView *myView = [myControl viewWithTag:10001];
            if([myView isKindOfClass:[UILabel class]])
            {
                UILabel *label = (UILabel *)myView;
                UIColor *color;
                if(self.currentOptionsSegmented == myControl.tag)
                {
                    if(enadled == YES)
                        color = [UIColor whiteColor];
                    else
                        color = [UIColor colorWithRed:0.6 green:0.6 blue:0.6 alpha:1];
                }
                else
                {
                    if(enadled == YES)
                        color = [UIColor blackColor];
                    else
                        color = [UIColor colorWithRed:0.4 green:0.4 blue:0.4 alpha:1];
                }

                label.textColor = color;
            }
        }
    }
}

//处理通知(应用程序不在处于活动状态)
- (void)applicationWillResignActive:(NSNotification *)notification
{
    //移除广告（不在后台占用cpu）
    [_admobBanner removeAds];
    _admobBanner = nil;
}

//处理通知(应用程序进入活动状态)
- (void)applicationDidBecomeActive:(NSNotification *)notification
{
    //刷新广告
    [self updateAdsState];
}

//更新广告窗口，用于显示／关闭广告
- (void)updateAdsState
{
    //已经添加过则不在添加
    if(_admobBanner != nil)
        return;
    //添加广告
    _admobBanner = [[ADmobBanner alloc] initWithViewController:self];
    //显示广告占位
    self.myAdmobBanner.hidden = NO;
}


@end


