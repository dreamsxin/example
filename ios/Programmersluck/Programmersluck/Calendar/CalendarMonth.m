//
//  CalendarMonth.m
//  Calendar
//
//  Created by Lloyd Bottomley on 29/04/10.
//  Copyright 2010 Savage Media Pty Ltd. All rights reserved.
//

#import "CalendarMonth.h"
#import "CalendarLogic.h"
#import "OCLunarDate.h"
#import "CalendarLocalization.h"

#define HEADLABLEHEIGHTSCALE            0.7     //日历顶部标签占顶部总高度的比例           年月标签/顶部
#define HEADFOUNTSIZESCALE              0.4     //日历顶部标签字体大小与顶部总高度的比例     年月字体/顶部标签
#define HEADWEEKDAYHEIGHTSCALE          0.6     //星期行的高度占顶部总高度的比例           星期行/顶部
#define HEADWEEKDAYFOUNTSIZESCALE       0.8     //星期行标签字体大小与星期行总高度的比例     星期字体/星期标签
#define CALENDARDAYFOUNTSIZESCALE       0.5     //日期按钮标签的字体与日期按钮高度的比例     日期字体/期日按钮
#define CALENDARDAYMARKSCALE            0.3     //日期按钮的标记与日期按钮大小的比例         日期标记/期日按钮
#define CALENDARDAYMARKHEIGHT           2       //日期按钮的条标记高度                    日期标记高度
#define CALENDARDAYBUTTOMFOUNTSIZESCALE 0.25    //日期按钮底部标签的字体与日期按钮高度的比例   日期底部字体/期日按钮

@implementation CalendarMonth


#pragma mark -
#pragma mark Getters / setters

@synthesize calendarLogic;
@synthesize datesIndex;
@synthesize buttonsIndex;
@synthesize markDict;
@synthesize numberOfDaysInWeek;
@synthesize selectedButton;
@synthesize selectedDate;
@synthesize headerFrame;
@synthesize calendarFrame;
@synthesize numberOfWeeks;
@synthesize calendarDayWidth;
@synthesize calendarDayHeight;

// Calendar object init
//初始化，frame－大小。aLogic－关联的日历逻辑
- (id)initWithFrame:(CGRect)frame logic:(CalendarLogic *)aLogic 
{
	// Size is static
	numberOfWeeks = 6;                                            //高度（日期个数）
	selectedButton = -1;                                          //被选中按钮为－1

    //初始化标记数组
    self.markDict = [[NSMutableDictionary alloc] init];
    //创建一个当前用户的日历对象（NSCalendar用于处理时间相关问题。比如比较时间前后、计算日期所的周别等。）
	NSCalendar *calendar = [NSCalendar currentCalendar];
    //创建一个日期组件，并赋予当前日期
	NSDateComponents *components = [calendar components:(NSDayCalendarUnit | NSMonthCalendarUnit | NSYearCalendarUnit) fromDate:[NSDate date]];
    //返回components指定的时间
	NSDate *todayDate = [calendar dateFromComponents:components];

    if ((self = [super initWithFrame:frame])) 
    {
        // Initialization code
		self.backgroundColor = [UIColor clearColor];  // Red should show up fails.//红色背景，但不显示。
		self.opaque = YES;                          //透明
		self.clipsToBounds = YES;                   //边界裁切
		self.clearsContextBeforeDrawing = NO;       //不自动清除绘图上下文

        //日历组件的宽度和高度
        UILabel *aLabel;
        //CGFloat headerHeight = frame.size.height/numberOfWeeks;                     //日历头高度
        CGFloat headerHeight = 20;
        headerFrame = CGRectMake(0, 0, frame.size.width, headerHeight);             //日历头位置和大小
        calendarFrame = CGRectMake(0, headerHeight, frame.size.width, 
                                   frame.size.height - headerFrame.size.height);    //日历部分位置和大小

        //创建图像视图，上边栏
		UIImageView *headerBackground = [[UIImageView alloc] initWithImage:[UIImage imageNamed:@"CalendarBackground.png"]];
		[headerBackground setFrame:headerFrame];
		[self addSubview:headerBackground];
		//创建图像视图，月历内容
		UIImageView *calendarBackground = [[UIImageView alloc] initWithImage:[UIImage imageNamed:@"CalendarBackground.png"]];
		[calendarBackground setFrame:calendarFrame];
		[self addSubview:calendarBackground];

		//创建一个格式化日期类
		NSDateFormatter *formatter = [[NSDateFormatter alloc] init];
        //获得星期缩写的数组
		NSArray *daySymbols = [formatter shortWeekdaySymbols];
        //每周几天
		self.numberOfDaysInWeek = [daySymbols count];

        //日历中日期的宽度和高度
        self.calendarDayWidth = calendarFrame.size.width / self.numberOfDaysInWeek; //日历中天的宽度
        self.calendarDayHeight = calendarFrame.size.height / numberOfWeeks;         //日历中天的高度
		//创建顶部标题，用于显示年份
        /*不要日历头
		aLabel = [[UILabel alloc] initWithFrame:CGRectMake(0, 0, headerFrame.size.width, headerFrame.size.height*HEADLABLEHEIGHTSCALE)];
		aLabel.backgroundColor = [UIColor clearColor];
		//aLabel.textAlignment = UITextAlignmentCenter;
        aLabel.textAlignment = NSTextAlignmentCenter;
		aLabel.font = [UIFont boldSystemFontOfSize:
                       headerFrame.size.height*HEADFOUNTSIZESCALE];                 //字体大小
		aLabel.textColor = [UIColor colorWithPatternImage:
                            [UIImage imageNamed:@"CalendarTitleColor.png"]];        //使用图像填充
		aLabel.shadowColor = [UIColor whiteColor];                                  //阴影颜色
		aLabel.shadowOffset = CGSizeMake(0, 1);                                     //阴影偏移
        
		[formatter setDateFormat:CL_YearAndMonthDateFormat];                        //日期格式
		aLabel.text = [formatter stringFromDate:aLogic.referenceDate];              //返回指定格式的日期
		[self addSubview:aLabel];                                                   //添加到视图
         */
        //分割线视图
		//UIView *lineView = [[UIView alloc] initWithFrame:CGRectMake(0, headerFrame.size.height-1, headerFrame.size.width, 1)];
		//lineView.backgroundColor = [UIColor lightGrayColor];
		//[self addSubview:lineView];

		// Setup weekday names
        //添加星期名称
        //获得第一个星期索引
		NSInteger firstWeekday = [calendar firstWeekday] - 1;
		for (NSInteger aWeekday = 0; aWeekday < numberOfDaysInWeek; aWeekday ++) 
        {
            //符号索引为当前循环次数＋第一个星期索引
 			NSInteger symbolIndex = aWeekday + firstWeekday;
            //如果符号索引大于星期个数，则循环到队列开始位置
			if (symbolIndex >= numberOfDaysInWeek) 
            {
				symbolIndex -= numberOfDaysInWeek;
			}

            //获得星期符号，并计算显示位置
			NSString *symbol = [daySymbols objectAtIndex:symbolIndex];
			CGFloat positionX = (aWeekday * calendarDayWidth) - 1;
            CGFloat weekdayHeight = headerFrame.size.height*HEADWEEKDAYHEIGHTSCALE; //星期高度行
			CGRect aFrame = CGRectMake(positionX, headerFrame.size.height-weekdayHeight-2,
                                       calendarDayWidth, weekdayHeight);
			
            //创建标签，并添加到视图
			aLabel = [[UILabel alloc] initWithFrame:aFrame];
			aLabel.backgroundColor = [UIColor clearColor];
			//aLabel.textAlignment = UITextAlignmentCenter;
            aLabel.textAlignment = NSTextAlignmentCenter;
			aLabel.text = symbol;
            //周末高亮
            if(aWeekday == 0 || aWeekday == numberOfDaysInWeek-1)
            {
                aLabel.textColor = [UIColor colorWithRed:0.5 green:0.2 blue:0 alpha:1];
            }
            else
            {
                aLabel.textColor = [UIColor darkGrayColor];
            }
            
			aLabel.font = [UIFont boldSystemFontOfSize:weekdayHeight* HEADWEEKDAYFOUNTSIZESCALE];     //字体大小
			aLabel.shadowColor = [UIColor whiteColor];
			aLabel.shadowOffset = CGSizeMake(0, 1);
			[self addSubview:aLabel];
		}

		// Build calendar buttons (6 weeks of 7 days)
        //建立日历按钮（宽高：6周7天）
		NSMutableArray *aDatesIndex = [[NSMutableArray alloc] init];    //日期索引
		NSMutableArray *aButtonsIndex = [[NSMutableArray alloc] init];  //按钮索引
		
        //每次循环绘制一列
		for (NSInteger aWeek = 0; aWeek < numberOfWeeks; aWeek ++) 
        {
            //当前行y坐标
			CGFloat positionY = (aWeek * calendarDayHeight) + headerFrame.size.height;
			
            //每次循环绘制一行
			for (NSInteger aWeekday = 1; aWeekday <= numberOfDaysInWeek; aWeekday ++) 
            {
                //当前列x坐标
				CGFloat positionX = ((aWeekday - 1) * calendarDayWidth) - 1;
                //当前日期位置
				CGRect dayFrame = CGRectMake(positionX, positionY, calendarDayWidth, calendarDayHeight);
                //根据行列获得日期
				NSDate *dayDate = [CalendarLogic dateForWeekday:aWeekday 
														 onWeek:aWeek 
												  referenceDate:[aLogic referenceDate]];
                //转换成格式化日期
				NSDateComponents *dayComponents = [calendar 
												   components:NSDayCalendarUnit fromDate:dayDate];

                //创建一个日期按钮，样式为UIButtonTypeCustom
				UIButton *dayButton = [UIButton buttonWithType:UIButtonTypeCustom];
				dayButton.opaque = YES;                                         //不透明
				dayButton.clipsToBounds = NO;                                   //边界裁切
				dayButton.clearsContextBeforeDrawing = NO;                      //重绘
				dayButton.frame = dayFrame;                                     //位置和大小
				dayButton.titleLabel.shadowOffset = CGSizeMake(0, 1);           //标题阴影
				dayButton.titleLabel.font = [UIFont boldSystemFontOfSize:calendarDayHeight*CALENDARDAYFOUNTSIZESCALE];                                 //标题字体
				dayButton.tag = [aDatesIndex count];                            //tag(当前个数)
				dayButton.adjustsImageWhenHighlighted = NO;                     //变化时发光
				dayButton.adjustsImageWhenDisabled = NO;                        //变化时禁用
				//dayButton.showsTouchWhenHighlighted = YES;                    //点击时发光
                
                //设置日期颜色
				UIColor *titleColor = [UIColor colorWithPatternImage:
                                       [UIImage imageNamed:@"CalendarTitleColor.png"]];
                //如果日期和当前日期间隔大于1个月（不是本月的日期），则使用不同的颜色
				if ([aLogic distanceOfDateFromCurrentMonth:dayDate] != 0) 
                {
					titleColor = [UIColor colorWithPatternImage:
                                  [UIImage imageNamed:@"CalendarTitleDimColor.png"]];
                    //取消标题阴影
                    dayButton.titleLabel.shadowOffset = CGSizeMake(0, 0);
				}

				// Normal
                //添加 UIControlStateNormal－正常状态 时的标题
				[dayButton setTitle:[NSString stringWithFormat:@"%d", [dayComponents day]] 
						   forState:UIControlStateNormal];
				
				// Selected
                //选中状态时标题颜色和阴影颜色
				[dayButton setTitleColor:titleColor forState:UIControlStateSelected];
				[dayButton setTitleShadowColor:[UIColor blackColor] forState:UIControlStateSelected];
				
                //是否是今天
				if ([dayDate compare:todayDate] != NSOrderedSame) 
                {
					// Normal
                    //正常状态时，按钮标题颜色，阴影。按钮背景
					[dayButton setTitleColor:titleColor forState:UIControlStateNormal];
					[dayButton setTitleShadowColor:[UIColor blackColor] forState:UIControlStateNormal];
					[dayButton setBackgroundImage:[UIImage imageNamed:@"CalendarDayTile.png"] forState:UIControlStateNormal];

					// Selected
                    //选中状态时，按钮背景
					[dayButton setBackgroundImage:[UIImage imageNamed:@"CalendarDaySelected.png"] forState:UIControlStateSelected];
					
				}
                else 
                {
					// Normal
                    //正常状态时，按钮标题颜色，阴影。按钮背景
					[dayButton setTitleColor:titleColor forState:UIControlStateNormal];
					[dayButton setTitleShadowColor:[UIColor blackColor] forState:UIControlStateNormal];
					[dayButton setBackgroundImage:[UIImage imageNamed:@"CalendarDayToday.png"] forState:UIControlStateNormal];
                    
                    //创建图像视图，今天
                    UIImageView *todayMark = [[UIImageView alloc] initWithImage:[UIImage imageNamed:@"CalendarDayTodayMark.png"]];
                    //[todayMark setFrame:];
                    [todayMark setContentMode:UIViewContentModeTopLeft];
                    todayMark.frame = dayButton.frame;
                    [self addSubview:todayMark];

					// Selected
                    //选中状态时，按钮背景
					[dayButton setBackgroundImage:[UIImage imageNamed:@"CalendarDayTodaySelected.png"] forState:UIControlStateSelected];
				}

                
                //添加阴历和节日信息
                UILabel *bottomLabel = [[UILabel alloc] init];
                CGFloat bottomLabelHeight = calendarDayHeight*CALENDARDAYBUTTOMFOUNTSIZESCALE;
                bottomLabel.text = [OCLunarDate lunarDateWithNSDate:dayDate].priorityLabel;
                bottomLabel.font = [UIFont boldSystemFontOfSize:bottomLabelHeight];
                bottomLabel.frame = CGRectMake(0,
                                        dayButton.frame.size.height - bottomLabelHeight - 2,
                                        dayButton.frame.size.width,
                                        bottomLabelHeight+2);
                bottomLabel.textAlignment = NSTextAlignmentCenter;
                //根据标签类型修改文本颜色
                if([OCLunarDate lunarDateWithNSDate:dayDate].priorityLabelType == kPriorityLabelTypeDay)
                {
                    bottomLabel.textColor = [UIColor colorWithRed:0.3 green:0.3 blue:0.3 alpha:1];
                }
                else
                {
                    bottomLabel.textColor = [UIColor colorWithRed:0.5 green:0.2 blue:0 alpha:1];
                }
                bottomLabel.backgroundColor = [UIColor clearColor];
                //插入到视图
                [dayButton insertSubview:bottomLabel atIndex:0];
                
                
				//给按钮添加响应动作
				[dayButton addTarget:self action:@selector(dayButtonPressed:) forControlEvents:UIControlEventTouchUpInside];
                //添加到视图
				[self addSubview:dayButton];
				
				// Save
				[aDatesIndex addObject:dayDate];        //日期添加到日期数组
				[aButtonsIndex addObject:dayButton];    //按钮添加到按钮数组
			}
		}

		// save
		self.calendarLogic = aLogic;                //保存日历逻辑
		self.datesIndex = [aDatesIndex copy];       //保存日期数组
		self.buttonsIndex = [aButtonsIndex copy];   //保存按钮数组
    }
    return self;
}

#pragma mark -
#pragma mark UI Controls

//通过日期查找视图中的日期索引，不存在则返－1
- (NSInteger )findIndexForDate:(NSDate *)aDate
{
    for(NSInteger i=0; i<[datesIndex count]; i++)
    {
        NSDate *date = [datesIndex objectAtIndex:i];
        if([date compare:aDate] == NSOrderedSame)
        {
            return i;
        }
    }
    return -1;
}

//删除指定日期上指定位置的标记，aDate－日期，loca－相对位置
- (void)removeMarkToDate:(NSDate *)aDate 
                Location:(CalendarMonthMarkLocation)loca
{
    UIImageView *remark = [self.markDict objectForKey:aDate];
    if(remark != nil)
    {
        [remark removeFromSuperview];
        [self.markDict removeObjectForKey:aDate];
    }
}

//在指定日期上添加一个标记，aDate－日期，imgName－用做标记的图片，loca－相对位置
- (void)addMarkToDate:(NSDate *)aDate 
           ImageNamed:(NSString *)imgName 
             Location:(CalendarMonthMarkLocation)loca
{
    NSInteger dateIndex = [self findIndexForDate:aDate];
    //指定日期不存在
    if(dateIndex == -1)
    {
        return;
    }
    UIButton *button = [buttonsIndex objectAtIndex:dateIndex];
    //如果当前位置存在值,则删除
    [self removeMarkToDate:aDate Location:kCalendarMonthMarkLocationDefault];
    UIImageView *mark = [[UIImageView alloc] initWithImage:[UIImage imageNamed:imgName]];
    switch (loca)
    {
        //右上
        case kCalendarMonthMarkLocationTopRight:
        {
            break;
        }
        //右下
        case kCalendarMonthMarkLocationBottomRight:
        {
            break;
        }
        //默认
        default:
        {
            mark.frame = CGRectMake(0,
                                    button.frame.size.height/2,
                                    button.frame.size.width, 
                                    CALENDARDAYMARKHEIGHT);
            mark.autoresizingMask = UIViewAutoresizingNone;
            break;
        }
    }
    //保存
    [self.markDict setObject:mark forKey:aDate];
    //插入到视图
    [button insertSubview:mark atIndex:0];
}

//日期被点击
- (void)dayButtonPressed:(id)sender 
{
    //设置日历逻辑的参考日期为被选择的日期（被选择按钮的tag对应的日期）
	[calendarLogic setReferenceDate:[datesIndex objectAtIndex:[sender tag]]];
}

//选中指定日期
- (void)selectButtonForDate:(NSDate *)aDate 
{
    //如果当前有选择，则先取消
	if (selectedButton >= 0) 
    {
        //获得今天的期日
		NSDate *todayDate = [CalendarLogic dateForToday];
        //获得当前选择的按钮（通过当前选择按钮索引）
		UIButton *button = [buttonsIndex objectAtIndex:selectedButton];
		//获得当前选择按钮的位置
		CGRect selectedFrame = button.frame;
        //如果当前选择的不是今天
		if ([selectedDate compare:todayDate] != NSOrderedSame) 
        {
            //计算弹起后位置
			//selectedFrame.origin.y = selectedFrame.origin.y + 1;
			//selectedFrame.size.width = calendarDayWidth;
			//selectedFrame.size.height = calendarDayHeight;
		}
		
        //取消选择
		button.selected = NO;           //按钮处于未选中状态，样式会随之更改
        //重置位置（弹起）
		button.frame = selectedFrame;
		
        //当前选择置空
		self.selectedButton = -1;
		self.selectedDate = nil;
	}

    //如果要选中的期日不为空
	if (aDate != nil) 
    {
		// Save
        //保存当前选择的按钮索引和日期
		self.selectedButton = [calendarLogic indexOfCalendarDate:aDate];
		self.selectedDate = aDate;
		
        //获得今天的日期和当前被选中的按钮
		NSDate *todayDate = [CalendarLogic dateForToday];
		UIButton *button = [buttonsIndex objectAtIndex:selectedButton]; //通过刚保存的按钮索引获得
        //获得被选中按钮的位置
		CGRect selectedFrame = button.frame;
        //如果被选中的不是今天
		if ([aDate compare:todayDate] != NSOrderedSame) 
        {
            //计算压入后位置
			//selectedFrame.origin.y = selectedFrame.origin.y - 1;
			//selectedFrame.size.width = calendarDayWidth + 1;
			//selectedFrame.size.height = calendarDayHeight + 1;
		}

        //按钮被选择
		button.selected = YES;          //按钮处于被选中状态，样式会随之更改
		button.frame = selectedFrame;
        //将button显示在当前视图的前面
		//[self bringSubviewToFront:button];
	}
}

@end


