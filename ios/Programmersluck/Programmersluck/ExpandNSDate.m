//
//  ExpandNSDate.m
//  LiftStatistics
//
//  Created by cuibo on 7/9/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "ExpandNSDate.h"

@implementation NSDate(Expand)

//将date转换到当前时区
- (NSDate *)switchToCurrentTimeZone
{
    NSTimeZone *zone = [NSTimeZone systemTimeZone];
    NSInteger interval = [zone secondsFromGMTForDate: self];
    NSDate *localeDate = [self  dateByAddingTimeInterval: interval];
    return localeDate;
}

//返回当前日期间隔指定天数的日期
- (NSDate *)intervalDays:(NSInteger)inter
{
    return [self dateByAddingTimeInterval:inter*24*3600];
}

//根据给定的strymd，返回当前日期的年月日
static NSDateFormatter* dateFormatter;
- (NSString *)getDateFormatterForStr:(NSString *)strymd
{
	if (!dateFormatter)
    {
		dateFormatter = [[NSDateFormatter alloc] init];
	}
    
    //返回字符年
    [dateFormatter setDateFormat:strymd];
    return [dateFormatter stringFromDate:self];
}

@end

