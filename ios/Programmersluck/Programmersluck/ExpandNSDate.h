//
//  ExpandNSDate.h
//  LiftStatistics
//
//  Created by cuibo on 7/9/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface NSDate(Expand)

//将date转换到当前时区
- (NSDate *)switchToCurrentTimeZone;

//返回当前日期间隔指定天数的日期
- (NSDate *)intervalDays:(NSInteger)inter;

//根据给定的strymd，返回当前日期的年月日
- (NSString *)getDateFormatterForStr:(NSString *)strymd;

@end

