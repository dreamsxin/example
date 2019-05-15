//
//  CalendarLogic.m
//  Calendar
//
//  Created by Lloyd Bottomley on 29/04/10.
//  Copyright 2010 Savage Media Pty Ltd. All rights reserved.
//

#import "CalendarLogic.h"


@implementation CalendarLogic

//@synthesize calendarLogicDelegate;
@synthesize calendarLogicDelegate;
@synthesize referenceDate;

//设置基准日期
- (void)setReferenceDate:(NSDate *)aDate 
{
	//为空，则通知代理 日期被选择为空，然后返回
    if (aDate == nil) 
    {
		[calendarLogicDelegate calendarLogic:self dateSelected:nil];
		return;
	}

	// Calculate direction of month switches
    //计算与当前日期的距离
	NSInteger distance = [self distanceOfDateFromCurrentMonth:aDate];

    //创建一个创建一个日期组件，并赋予要设置的日期
	NSDateComponents *components = [[NSCalendar currentCalendar] components:(NSDayCalendarUnit | NSMonthCalendarUnit | NSYearCalendarUnit) fromDate:aDate];
    //返回日期（统一格式）
	referenceDate = [[NSCalendar currentCalendar] dateFromComponents:components];

	// Message delegate
    //通知代理 日期被选择为aDate
	[calendarLogicDelegate calendarLogic:self dateSelected:aDate];

	// month switch?
    //距离不等于0则需要交换月历视图
	if (distance != 0) 
    {
		// Changed so tell delegate
        // 通知代理 交换月历到距离distance
		[calendarLogicDelegate calendarLogic:self monthChangeDirection:distance];
	}
}

#pragma mark -
#pragma mark Initialization

//初始化，aDelegate－代理，aDate－基准日期
- (id)initWithDelegate:(id <CalendarLogicDelegate>)aDelegate referenceDate:(NSDate *)aDate 
{
    if ((self = [super init])) 
    {
        // Initialization code
		self.calendarLogicDelegate = aDelegate;

        //创建一个创建一个日期组件，并赋予要设置的日期
		NSDateComponents *components = [[NSCalendar currentCalendar] components:(NSDayCalendarUnit | NSMonthCalendarUnit | NSYearCalendarUnit) fromDate:aDate];
        //返回日期（统一格式）
		referenceDate = [[NSCalendar currentCalendar] dateFromComponents:components];
    }
    return self;
}



#pragma mark -
#pragma mark Date computations

//返回当前日期
+ (NSDate *)dateForToday
{
	NSCalendar *calendar = [NSCalendar currentCalendar];
	NSDateComponents *components = [calendar components:(NSDayCalendarUnit | NSMonthCalendarUnit | NSYearCalendarUnit) fromDate:[NSDate date]];
	NSDate *todayDate = [calendar dateFromComponents:components];
	
	return todayDate;
}

//构造一个日期。aWeekday－星期几，aWeek－第几周（今年），aReferenceDate－日期（只含月和年）
+ (NSDate *)dateForWeekday:(NSInteger)aWeekday 
                    onWeek:(NSInteger)aWeek 
             referenceDate:(NSDate *)aReferenceDate 
{
    //使用参数aReferenceDate构造日期组件
	NSDateComponents *components = [[NSCalendar currentCalendar] 
                                    components:(NSMonthCalendarUnit | NSYearCalendarUnit) 
                                      fromDate:aReferenceDate];
	
	NSInteger aMonth = [components month];
	NSInteger aYear = [components year];
	
	return [self dateForWeekday:(NSInteger)aWeekday 
						 onWeek:(NSInteger)aWeek 
						ofMonth:(NSInteger)aMonth 
						 ofYear:(NSInteger)aYear];
}

//构造一个日期。aWeekday－星期几，aWeek－第几周（今年），aMonth－月，aYear－年
+ (NSDate *)dateForWeekday:(NSInteger)aWeekday 
					onWeek:(NSInteger)aWeek 
				   ofMonth:(NSInteger)aMonth 
					ofYear:(NSInteger)aYear 
{
	NSCalendar *calendar = [NSCalendar currentCalendar];            //当前日历
	
	// Select first 'firstWeekDay' in this month
    //构造一个NSDateComponents
	NSDateComponents *firstStartDayComponents = [[NSDateComponents alloc] init];
	[firstStartDayComponents setMonth:aMonth];
	[firstStartDayComponents setYear:aYear];
	[firstStartDayComponents setWeekday:[calendar firstWeekday]];   //星期几
	[firstStartDayComponents setWeekdayOrdinal:1];                  //这个月的第几周
    //得到日期
	NSDate *firstDayDate = [calendar dateFromComponents:firstStartDayComponents];

	// Grab just the day part.
	firstStartDayComponents = [calendar components:NSDayCalendarUnit fromDate:firstDayDate];
	NSInteger numberOfDaysInWeek = [calendar maximumRangeOfUnit:NSWeekdayCalendarUnit].length;
	NSInteger firstDay = [firstStartDayComponents day] - numberOfDaysInWeek;

	// Correct for day landing on the firstWeekday
	if ((firstDay - 1) == -numberOfDaysInWeek) {
		firstDay = 1;
	}

	NSDateComponents *components = [[NSDateComponents alloc] init];
	[components setYear:aYear];
	[components setMonth:aMonth];
	[components setDay:(aWeek * numberOfDaysInWeek) + firstDay + (aWeekday - 1)];
	
	return [calendar dateFromComponents:components];
}

//返回指定日期的索引（在当前日历页中）
- (NSInteger)indexOfCalendarDate:(NSDate *)aDate 
{
	NSCalendar *calendar = [NSCalendar currentCalendar];
	
	// Split
	NSDateComponents *components = [calendar components:NSWeekdayCalendarUnit | 
                                                        NSMonthCalendarUnit | 
                                                        NSWeekCalendarUnit | 
                                                        NSYearCalendarUnit 
                                               fromDate:aDate];

	// Select this month in this year.
	NSDateComponents *firstDayComponents = [[NSDateComponents alloc] init];
	[firstDayComponents setMonth:[components month]];
	[firstDayComponents setYear:[components year]];
	NSDate *firstDayDate = [calendar dateFromComponents:firstDayComponents];
	
	// Turn into week of a year.
	NSDateComponents *firstWeekComponents = [calendar components:NSWeekCalendarUnit 
                                                        fromDate:firstDayDate];
	NSInteger firstWeek = [firstWeekComponents week];
	if (firstWeek > [components week]) 
    {
		firstWeek = firstWeek - 52;
	}
	NSInteger weekday = [components weekday];
	if (weekday < (NSInteger)[calendar firstWeekday]) 
    {
		weekday = weekday + 7;
	}
	
	return (weekday + (([components week] - firstWeek) * 7)) - [calendar firstWeekday];
}

//返回指定日期aDate于当前基准日期的间隔月份（正负＋距离）
- (NSInteger)distanceOfDateFromCurrentMonth:(NSDate *)aDate 
{
	if (aDate == nil) 
    {
		return -1;
	}
	
	NSInteger distance = 0;
	NSCalendar *calendar = [NSCalendar currentCalendar];
	
	NSDateComponents *monthComponents = [calendar components:(NSMonthCalendarUnit | NSYearCalendarUnit)
                                                    fromDate:referenceDate];
	NSDate *firstDayInMonth = [calendar dateFromComponents:monthComponents];
	[monthComponents setDay:[calendar rangeOfUnit:NSDayCalendarUnit
										   inUnit:NSMonthCalendarUnit
										  forDate:referenceDate].length];
	NSDate *lastDayInMonth = [calendar dateFromComponents:monthComponents];
	
	// Lower
	NSInteger distanceFromFirstDay = [[calendar components:NSDayCalendarUnit 
                                                  fromDate:firstDayInMonth 
                                                    toDate:aDate 
                                                   options:0] day];
	if (distanceFromFirstDay < 0) 
    {
		distance = distanceFromFirstDay;
	}
	
	// Greater
	NSInteger distanceFromLastDay = [[calendar components:NSDayCalendarUnit 
                                                 fromDate:lastDayInMonth 
                                                   toDate:aDate 
                                                  options:0] day];
	if (distanceFromLastDay > 0) 
    {
		distance = distanceFromLastDay;
	}
	
	return distance;
}

//选择前一个月
- (void)selectPreviousMonth 
{
	NSCalendar *calendar = [NSCalendar currentCalendar];
	
	NSDateComponents *components = [[NSDateComponents alloc] init];
	[components setMonth:-1];
	

	referenceDate = [calendar dateByAddingComponents:components toDate:referenceDate options:0];
	[calendarLogicDelegate calendarLogic:self monthChangeDirection:-1];	
}

//选择下一个月
- (void)selectNextMonth 
{
	NSCalendar *calendar = [NSCalendar currentCalendar];
	
	NSDateComponents *components = [[NSDateComponents alloc] init];
	[components setMonth:1];
	
	referenceDate = [calendar dateByAddingComponents:components toDate:referenceDate options:0];
	[calendarLogicDelegate calendarLogic:self monthChangeDirection:1];
}

@end


