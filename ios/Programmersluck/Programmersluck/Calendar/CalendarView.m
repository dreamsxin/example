//
//  CalendarView.m
//  LiftStatistics
//
//  Created by cuibo on 7/8/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "CalendarView.h"
#import "CalendarLogic.h"
#import "CalendarMonth.h"

#define BUTTONSIZEWIDTHSCALE  6.5  //按钮宽度和视图的比例

@interface CalendarView()
    <UIGestureRecognizerDelegate>

@property (nonatomic, strong) CalendarLogic *calendarLogic;
@property (nonatomic, strong) CalendarMonth *calendarView;
@property (nonatomic, strong) CalendarMonth *calendarViewNew;
@property (nonatomic, strong) UIButton *leftButton;
@property (nonatomic, strong) UIButton *rightButton;

//月历滑动动画完成
- (void)animationMonthSlideComplete;

@end


@implementation CalendarView

@synthesize calendarViewDelegate;
@synthesize calendarViewScrollDelegate;
@synthesize calendarLogic;
@synthesize calendarView;
@synthesize calendarViewNew;
@synthesize selectedDate;
@synthesize leftButton;
@synthesize rightButton;

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) 
    {
        //self.bounds = CGRectMake(0, 0, 320, 324);     //边界
        self.frame = frame;
        self.clearsContextBeforeDrawing = NO;           //不自动清除绘图上下文
        self.opaque = YES;                              //不透明
        self.clipsToBounds = YES;                       //边界剪切

        //代理默认为空
        calendarViewDelegate = nil;

        //默认选中今天
        selectedDate = [CalendarLogic dateForToday];

        //创建日期逻辑对象
        CalendarLogic *aCalendarLogic = [[CalendarLogic alloc] initWithDelegate:self 
                                                                  referenceDate:selectedDate];
        self.calendarLogic = aCalendarLogic;
        
        //创建日历的一个月历视图，指定范围和日期逻辑
        calemdarViewFrame = CGRectMake(0, 0, self.frame.size.width, self.frame.size.height);
        CalendarMonth *aCalendarView = [[CalendarMonth alloc] initWithFrame:calemdarViewFrame 
                                                                      logic:calendarLogic];
        [aCalendarView selectButtonForDate:selectedDate];   //选择按钮设为selectedDate日期
        [self addSubview:aCalendarView];                    //添加到当前视图
        self.calendarView = aCalendarView;                  //保存到全局
        _isViewLoaded = YES;                                //滚动加载完成
/*
        //导航按钮位置
        CGRect calendarViewHeadFrame = aCalendarView.headerFrame;
        CGSize buttonSize = CGSizeMake(calemdarViewFrame.size.width/BUTTONSIZEWIDTHSCALE, 
                                       calendarViewHeadFrame.size.height);

        CGRect leftButtonFrame = CGRectMake(0, 0, buttonSize.width, buttonSize.height);
        CGRect rightButtonFrame = CGRectMake(calemdarViewFrame.size.width-buttonSize.width, 0, 
                                             buttonSize.width, buttonSize.height);
        
        //创建左按钮，并设置位置，贴图，响应方法为selectPreviousMonth，添加到视图，保存到全局
        UIButton *aLeftButton = [UIButton buttonWithType:UIButtonTypeCustom];
        aLeftButton.frame = leftButtonFrame;
        [aLeftButton setImage:[UIImage imageNamed:@"CalendarArrowLeft.png"] 
                     forState:UIControlStateNormal];
        [aLeftButton addTarget:calendarLogic 
                        action:@selector(selectPreviousMonth) 
              forControlEvents:UIControlEventTouchUpInside];
        [self addSubview:aLeftButton];
        self.leftButton = aLeftButton;
        
        //创建左按钮，并设置位置，贴图，响应方法为selectNextMonth，添加到视图，保存到全局
        UIButton *aRightButton = [UIButton buttonWithType:UIButtonTypeCustom];
        aRightButton.frame = rightButtonFrame;
        [aRightButton setImage:[UIImage imageNamed:@"CalendarArrowRight.png"]
                      forState:UIControlStateNormal];
        [aRightButton addTarget:calendarLogic 
                         action:@selector(selectNextMonth) 
               forControlEvents:UIControlEventTouchUpInside];
        [self addSubview:aRightButton];
        self.rightButton = aRightButton;
 */
        //关注拖动手势
        UIPanGestureRecognizer *panGestureRecognizer;
        panGestureRecognizer = [[UIPanGestureRecognizer alloc] initWithTarget:self 
                                                                       action:@selector(handlePanFrom:)];
        panGestureRecognizer.delegate = self;
        [self addGestureRecognizer:panGestureRecognizer];
    }
    return self;
}

//手势开始
- (BOOL)gestureRecognizerShouldBegin:(UIGestureRecognizer *)gestureRecognizer
{
    CGPoint location = [gestureRecognizer locationInView:self];
    _handlePanLastLocation = location;
    return YES; //始终关注这个手势
}

//是否要求识别
- (BOOL)gestureRecognizer:(UIGestureRecognizer *)gestureRecognizer shouldReceiveTouch:(UITouch *)touch
{
    return YES; //始终关注这个手势
}

//移动手势处理
- (void)handlePanFrom:(UIPanGestureRecognizer *)recognizer 
{
    //手指目前的位置
    CGPoint location = [recognizer locationInView:self];
    // 是否手势结束
    if(recognizer.state == UIGestureRecognizerStateEnded)
    {
        //滚动是否结束
        if(_isViewLoaded == NO)
            return;
        _isViewLoaded = NO;

        //速度
        _panVelocity = [recognizer velocityInView:self];
        //方向
        if(location.x - _handlePanLastLocation.x > 0)
        {
            [self.calendarLogic selectPreviousMonth];
        }
        else
        {
            [self.calendarLogic selectNextMonth];
        }
    }
}

//设定选定日期
- (void)setSelectedDate:(NSDate *)aDate
{
    //设置当前选定日期
	selectedDate = aDate;
    
	[calendarLogic setReferenceDate:aDate];     //日历逻辑 设置参考日期
	[calendarView selectButtonForDate:aDate];   //日历视图 被选中按钮设为指定日期
}

//在指定日期上添加一个标记，aDate－日期，imgName－用做标记的图片，loca－相对位置
- (void)addMarkToDate:(NSDate *)aDate 
           ImageNamed:(NSString *)imgName 
             Location:(CalendarMonthMarkLocation)loca
{
    //设置月历视图，添加一个标记
    [calendarView addMarkToDate:aDate ImageNamed:imgName Location:loca];
}

//删除指定日期上指定位置的标记，aDate－日期，loca－相对位置
- (void)removeMarkToDate:(NSDate *)aDate 
                Location:(CalendarMonthMarkLocation)loca
{
    //设置月历视图，添加一个标记
    [calendarView removeMarkToDate:aDate Location:loca];
}

//获取当前日历视图中的所有日期，NSArray－但前日历中的所有日期(NSDate类型，格林尼治标准时间)，按照顺序排列。
- (NSArray *)getAllDatesInView
{
    return [calendarView datesIndex];
}

//内容视图大小
- (CGSize)contentSizeForViewInPopoverView 
{
	return CGSizeMake(calemdarViewFrame.size.width, calemdarViewFrame.size.height);
}

#pragma mark -
#pragma mark UI events

//用作清除期日（清除选择的日期）
- (void)actionClearDate:(id)sender 
{
	self.selectedDate = nil;                //选择日期为空
	[calendarView selectButtonForDate:nil]; //选择按钮设为nil日期
	
	// Delegate called later.
    //通知日历视图控制器self，时期变化为nil。
	[calendarViewDelegate calendarView:self dateDidChange:nil];
}

#pragma mark -
#pragma mark CalendarLogic delegate

//日期被选择，aLogic－日历逻辑，aDate－选择的日期
- (void)calendarLogic:(CalendarLogic *)aLogic dateSelected:(NSDate *)aDate 
{
    //保存选择的日期
	selectedDate = aDate;
    //如果日历逻辑的当前月 到本月的日期距离等于0
	if ([calendarLogic distanceOfDateFromCurrentMonth:selectedDate] == 0) 
    {
		[calendarView selectButtonForDate:selectedDate];    //选择按钮设为selectedDate日期
	}
	//通知日历视图控制器self，时期变化为selectedDate。
	[calendarViewDelegate calendarView:self dateDidChange:aDate];
}

//月份变化，aLogic－日历逻辑，aDirection－月份变化方向（正负）
- (void)calendarLogic:(CalendarLogic *)aLogic monthChangeDirection:(NSInteger)aDirection 
{
	BOOL animate = YES;   //是否使用动画

    //根据方向计算距离（月历宽）
	CGFloat distance = calemdarViewFrame.size.width;
	if (aDirection < 0) 
    {
		distance = -distance;
	}

    //忽略用户事件
	leftButton.userInteractionEnabled = NO;
	rightButton.userInteractionEnabled = NO;

    //创建一个新的月历视图
	CalendarMonth *aCalendarView = [[CalendarMonth alloc] 
                                    initWithFrame:CGRectMake(distance, 0, calemdarViewFrame.size.width, calemdarViewFrame.size.height)
                                            logic:aLogic];
    //忽略用户事件
	aCalendarView.userInteractionEnabled = NO;
    //如果日历逻辑的当前月 到本月的日期距离等于0
	if ([calendarLogic distanceOfDateFromCurrentMonth:selectedDate] == 0) 
    {
		[aCalendarView selectButtonForDate:selectedDate];   //选择按钮设为selectedDate日期
	}
    //插入视图aCalendarView到calendarView视图的上一层
	[self insertSubview:aCalendarView belowSubview:calendarView];
	//将aCalendarView保存到新日历视图
	self.calendarViewNew = aCalendarView;

    //如果当前视图已加载完成
	if (animate) 
    {
        //创建一个视图切换动画
		[UIView beginAnimations:NULL context:nil];
		[UIView setAnimationDelegate:self];
        //结束后条用animationMonthSlideComplete
		[UIView setAnimationDidStopSelector:@selector(animationMonthSlideComplete)];
        //移动速度换算成动画时间
        CGFloat velocity;
        velocity = 0.8 - (fabs(_panVelocity.x)/10000 * 3);
        velocity<0.15?velocity=0.05:0;
		[UIView setAnimationDuration:velocity];
		[UIView setAnimationCurve:UIViewAnimationCurveEaseInOut];
	}

    //设置月历视图的偏移
	calendarView.frame = CGRectOffset(calendarView.frame, -distance, 0);
	aCalendarView.frame = CGRectOffset(aCalendarView.frame, -distance, 0);

    //如果当前视图已加载完成
	if (animate) 
    {
        //提交动画
		[UIView commitAnimations];
	} 
    else 
    {
        //没使用动画，则直接调用结束处理
		[self animationMonthSlideComplete];
	}
}

//月历滑动动画完成
- (void)animationMonthSlideComplete 
{
	// Get rid of the old one.
    //移除月历视图（旧的）
	[calendarView removeFromSuperview];

	// replace
    //刷新
    //月历视图等于新月历视图
	self.calendarView = calendarViewNew;
    //新月历视图等于nil
	self.calendarViewNew = nil;

    //开始接受用户事件
	leftButton.userInteractionEnabled = YES;
	rightButton.userInteractionEnabled = YES;
	calendarView.userInteractionEnabled = YES;
    
    //通知代理滚动完成
    [calendarViewScrollDelegate calendarViewDidScroll:self AllDatesInView:[calendarView datesIndex]];
    //展示的月份发生变化，aCalendarView－一个日历视图,aDate-变化的日期
    [calendarViewDelegate calendarView:self monthDidChange:self.calendarLogic.referenceDate];
    
    _isViewLoaded = YES;
}


/*
// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void)drawRect:(CGRect)rect
{
    // Drawing code
}
*/

@end

