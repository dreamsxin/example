//
//  CalendarViewDelegate.h
//  LiftStatistics
//
//  Created by cuibo on 7/9/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//
//  日历视图代理
//

#import <Foundation/Foundation.h>

@class CalendarView;  //日历视图

@protocol CalendarViewDelegate

//选择的日期发生变化，aCalendarView－一个日历视图,aDate-变化的日期
- (void)calendarView:(CalendarView *)aCalendarView dateDidChange:(NSDate *)aDate;

//展示的月份发生变化，aCalendarView－一个日历视图,aDate-变化的日期
- (void)calendarView:(CalendarView *)aCalendarView monthDidChange:(NSDate *)aDate;

@end

