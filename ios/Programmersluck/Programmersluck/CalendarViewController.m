//
//  FirstViewController.m
//  Programmersluck
//
//  Created by cuibo on 3/15/13.
//  Copyright (c) 2013 cuibo. All rights reserved.
//

#import "CalendarViewController.h"
#import "CalendarView.h"
#import "CalendarViewDelegate.h"
#import "CalendarViewScrollDelegate.h"
#import "FortuneViewController.h"
#import "ExpandNSDate.h"
#import "LunarCalendarView.h"

@interface CalendarViewController ()
<
    CalendarViewDelegate,
    CalendarViewScrollDelegate
>

@property(strong, nonatomic)IBOutlet UIView *titleView;                    //顶部标题视图
@property(strong, nonatomic)IBOutlet UILabel *dateTitle;                   //顶部日期标题
@property(strong, nonatomic)IBOutlet UIView *topCalendarView;              //顶部的日历视图(整个日历)
@property(strong, nonatomic)IBOutlet UIView *calendarControlView;          //日历控制视图(下拉操作部分)
@property (strong, nonatomic) IBOutlet UILabel *calendarControlViewItem;   //日历控制视图(下拉操作部分)状态标签(v/^)
@property(strong, nonatomic)IBOutlet UIView *calendarContainerView;        //日历容器视图(显示日历)
@property(strong, nonatomic)CalendarView *calendarView;                    //日历视图
@property(strong, nonatomic)IBOutlet LunarCalendarView *lunarCalendarView; //阴历日历视图(显示阴历)
@property(strong, nonatomic)IBOutlet UIScrollView *fortuneScrollView;      //运势容器视图
@property(strong, nonatomic)FortuneViewController *fortuneViewController;  //运势视图控制器

//触摸位置
@property(assign, nonatomic)CGPoint toucheBeganLoca;
@property(assign, nonatomic)CGPoint toucheBeganViewLoca;
//压入标记
@property(assign, nonatomic)BOOL displayViewPush;

@end


@implementation CalendarViewController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        self.title = NSLocalizedString(@"黄历", @"黄历");
        self.tabBarItem.image = [UIImage imageNamed:@"calendar"];
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
	// Do any additional setup after loading the view, typically from a nib.
    //添加日历视图
    self.calendarView = [[CalendarView alloc] initWithFrame:self.calendarContainerView.frame];
    //设置日历视图代理
    self.calendarView.calendarViewDelegate = self;
    self.calendarView.calendarViewScrollDelegate = self;
    [self.calendarContainerView addSubview:self.calendarView];

    //设置滚动视图
    self.fortuneViewController = [[FortuneViewController alloc] initWithNibName:@"FortuneViewController" bundle:nil];
    [self.fortuneScrollView addSubview:self.fortuneViewController.view];
    
    //更新
    [self updateView];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

//更新视图内所有内容
- (void)updateView
{
    NSDate *selectDate = self.calendarView.selectedDate;
    //顶部视图标签
    self.dateTitle.text = [selectDate getDateFormatterForStr:@"yyyy年MM月dd日"];
    //阴历日历视图
    [self.lunarCalendarView updateView:selectDate];
    //运势视图
    [self.fortuneViewController updateView:selectDate];
    //滚动视图滚动到顶部
    [self.fortuneScrollView setContentOffset:CGPointMake(0, 0) animated:YES];
    //滚动视图尺寸
    self.fortuneScrollView.contentSize = self.fortuneViewController.view.frame.size;
}

//日历到今天
- (IBAction)toDayButton:(id)sender
{
    //格式化日期（去掉时间）
    NSDateComponents * components = [[NSCalendar currentCalendar] components:(NSYearCalendarUnit | NSMonthCalendarUnit | NSDayCalendarUnit) fromDate:[NSDate date]];
    [self.calendarView setSelectedDate:[[NSCalendar currentCalendar] dateFromComponents:components]];
}

#pragma -
#pragma - CalendarViewDelegate

//选择的日期发生变化，aCalendarView－一个日历视图,aDate-变化的日期
- (void)calendarView:(CalendarView *)aCalendarView dateDidChange:(NSDate *)aDate
{
    //更新
    [self updateView];
}

//展示的月份发生变化，aCalendarView－一个日历视图,aDate-变化的日期
- (void)calendarView:(CalendarView *)aCalendarView monthDidChange:(NSDate *)aDate
{
    //改变选择的日期
    [aCalendarView setSelectedDate:aDate];
}

#pragma -
#pragma - CalendarViewScrollDelegate

//日历换页结束，aCalendarView－一个日历视图,dates-当前视图中的所有日期
- (void)calendarViewDidScroll:(CalendarView *)aCalendarView AllDatesInView:(NSArray *)dates
{
    ;
}


#pragma -
#pragma - 日历压入弹出管理

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    UITouch *touch = [touches anyObject];
    
    if(touch.view == self.calendarControlView)
    {
        self.toucheBeganLoca = [touch locationInView:self.view];
        self.toucheBeganViewLoca = self.topCalendarView.frame.origin;
    }

}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    UITouch *touch = [touches anyObject];
    
    if(touch.view == self.calendarControlView)
    {
        //移动展示视图
        CGRect calendarframe;
        calendarframe = self.topCalendarView.frame;
        
        CGPoint curloca;
        CGFloat distance;
        
        curloca = [touch locationInView:self.view];
        distance = curloca.y - self.toucheBeganLoca.y;

        if(self.toucheBeganViewLoca.y+distance > self.titleView.frame.size.height)
            return;
        //移动日历视图
        self.topCalendarView.frame = CGRectMake(calendarframe.origin.x,
                                self.toucheBeganViewLoca.y+distance,
                                calendarframe.size.width,
                                calendarframe.size.height);
        [self extrudeFortuneScrollView];
    }
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    UITouch *touch = [touches anyObject];
    
    if(touch.view == self.calendarControlView)
    {
        NSInteger distance;
        
        distance = self.toucheBeganViewLoca.y - self.view.frame.origin.y;
        if(distance == 0)
        {
            [self pushCalendarView:!self.displayViewPush animated:YES];
        }
        else if(distance > 0)
        {
            [self pushCalendarView:YES animated:YES];
        }
        else
        {
            [self pushCalendarView:NO animated:YES];
        }
    }
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
    [self touchesEnded:touches withEvent:event];
}

//压入／弹出日历视图。
- (void)pushCalendarView:(BOOL)push animated:(BOOL)animated
{
    //移动展示视图
    CGRect calendarframe;
    calendarframe = self.topCalendarView.frame;
    
	if (animated)
    {
        //创建一个视图切换动画
		[UIView beginAnimations:NULL context:nil];
		[UIView setAnimationDelegate:self];
        //结束后条用animationMonthSlideComplete
		//[UIView setAnimationDidStopSelector:@selector(animationMonthSlideComplete)];
		[UIView setAnimationDuration:0.4];
		[UIView setAnimationCurve:UIViewAnimationCurveEaseInOut];
	}
    
    if(push)
    {
        self.topCalendarView.frame = CGRectMake(calendarframe.origin.x,
                                -calendarframe.size.height + self.calendarControlView.frame.size.height + self.titleView.frame.size.height,
                                calendarframe.size.width,
                                calendarframe.size.height);
        self.displayViewPush = YES;
        self.calendarControlViewItem.text = @"v";
    }
    else
    {
        self.topCalendarView.frame = CGRectMake(calendarframe.origin.x,
                                self.titleView.frame.size.height,
                                calendarframe.size.width,
                                calendarframe.size.height);
        self.displayViewPush = NO;
        self.calendarControlViewItem.text = @"^";
    }
    
    [self extrudeFortuneScrollView];
    
	if(animated)
    {
        //提交动画
		[UIView commitAnimations];
	}
}

//拉伸运势滚动视图
- (void)extrudeFortuneScrollView
{
    //改变运势滚动视图(根据日历视图的位置)
    CGRect fortuneframe;
    fortuneframe = self.fortuneScrollView.frame;
    //初始化y轴为阳历日历底部
    CGFloat y = self.topCalendarView.frame.origin.y + self.calendarContainerView.frame.size.height;
    //如果y轴在阴历日历的上方了，则y轴固定到阴历日历的下方
    if(y < (self.lunarCalendarView.frame.origin.y+self.lunarCalendarView.frame.size.height))
    {
        y = (self.lunarCalendarView.frame.origin.y+self.lunarCalendarView.frame.size.height);
    }
    self.fortuneScrollView.frame = CGRectMake(self.topCalendarView.frame.origin.x,
                                              y,
                                              self.topCalendarView.frame.size.width,
                                              self.view.frame.size.height - y);
}

@end

