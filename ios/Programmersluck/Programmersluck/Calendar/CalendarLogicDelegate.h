//
//  CalendarLogicDelegate.h
//  Calendar
//
//  Created by Lloyd Bottomley on 29/04/10.
//  Copyright 2010 Savage Media Pty Ltd. All rights reserved.
//

@class CalendarLogic;

//日历逻辑代理，将日期选择，月份变化（翻页）等实现交由代理完成
@protocol CalendarLogicDelegate

//日期被选择，aLogic－日历逻辑，aDate－选择的日期
- (void)calendarLogic:(CalendarLogic *)aLogic dateSelected:(NSDate *)aDate;
//月份变化，aLogic－日历逻辑，aDirection－月份变化方向（正负）
- (void)calendarLogic:(CalendarLogic *)aLogic monthChangeDirection:(NSInteger)aDirection;

@end

