//
//  CalendarLogic.h
//  Calendar
//
//  Created by Lloyd Bottomley on 29/04/10.
//  Copyright 2010 Savage Media Pty Ltd. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "CalendarLogicDelegate.h"

@interface CalendarLogic : NSObject 
{
	id <CalendarLogicDelegate> calendarLogicDelegate;   //日历逻辑委托
	NSDate *referenceDate;                              //基准日期
}

@property (nonatomic, strong) id<CalendarLogicDelegate> calendarLogicDelegate;
@property (nonatomic, strong) NSDate *referenceDate;

//初始化，aDelegate－日历逻辑代理。aDate－基准日期
- (id)initWithDelegate:(id <CalendarLogicDelegate>)aDelegate referenceDate:(NSDate *)aDate;

//获得今天的日期
+ (NSDate *)dateForToday;
//构造一个日期。aWeekday－星期几，aWeek－第几周（今年），aReferenceDate－日期（只含月和年）
+ (NSDate *)dateForWeekday:(NSInteger)aWeekday 
					onWeek:(NSInteger)aWeek 
				   ofMonth:(NSInteger)aMonth 
					ofYear:(NSInteger)aYear;
//构造一个日期。aWeekday－星期几，aWeek－第几周（今年），aMonth－月，aYear－年
+ (NSDate *)dateForWeekday:(NSInteger)aWeekday 
                    onWeek:(NSInteger)aWeek 
             referenceDate:(NSDate *)aReferenceDate;
//返回日期aDate在日历中的索引
- (NSInteger)indexOfCalendarDate:(NSDate *)aDate;
//aDate和当前月份的距离
- (NSInteger)distanceOfDateFromCurrentMonth:(NSDate *)aDate;
//选择到前一个月份
- (void)selectPreviousMonth;
//选择到下一个月份
- (void)selectNextMonth;

@end

