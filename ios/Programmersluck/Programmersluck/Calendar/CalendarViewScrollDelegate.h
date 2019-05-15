//
//  CalendarViewScrollDelegate.h
//  LiftStatistics
//
//  Created by cuibo on 7/11/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

@class CalendarView;  //日历视图

//日历滚动代理协议
@protocol CalendarViewScrollDelegate

//日历换页结束，aCalendarView－一个日历视图,dates-当前视图中的所有日期
- (void)calendarViewDidScroll:(CalendarView *)aCalendarView AllDatesInView:(NSArray *)dates;

@end

