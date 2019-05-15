//
//  LunarCalendarView.m
//  Programmersluck
//
//  Created by cuibo on 3/19/13.
//  Copyright (c) 2013 cuibo. All rights reserved.
//

#import "LunarCalendarView.h"
#import "ExpandNSDate.h"
#import "OCLunarDate.h"

@interface LunarCalendarView()

@property(strong, nonatomic)IBOutlet UILabel *lunarLabel;   //农历
@property(strong, nonatomic)IBOutlet UILabel *monthLabel;   //月
@property(strong, nonatomic)IBOutlet UILabel *dayLabel;     //日
@property(strong, nonatomic)IBOutlet UILabel *weekLabel;    //周
@property(strong, nonatomic)IBOutlet UILabel *yearLabel;    //年

@end

@implementation LunarCalendarView

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code
    }
    return self;
}

//刷新视图
- (void)updateView:(NSDate *)aDate
{
    //阳历信息
	//NSCalendar *calendar = [NSCalendar currentCalendar];
	//NSDateComponents *components = [calendar components:(NSDayCalendarUnit | NSMonthCalendarUnit | NSYearCalendarUnit) fromDate:aDate];
    
    //阴历信息
    OCLunarDate *lunarDate = [OCLunarDate lunarDateWithNSDate:aDate];
    //NSLog(@"%@ %@ %@ %@ %@ %@ %@ %@", lunarDate.day, lunarDate.month, lunarDate.zodiac,
    //      lunarDate.tiangan, lunarDate.dizhi, lunarDate.solarterm,
    //      lunarDate.festival, lunarDate.lunarFestival);
    
    //标题
    //self.dateTitle.text = [aDate getDateFormatterForStr:@"yyyy年 MMMM d日 EEEE"];
    //self.lunarDateTitle.text = [NSString stringWithFormat:@"%@%@ %@%@年 [%@]", lunarDate.month, lunarDate.day, lunarDate.tiangan, lunarDate.dizhi, lunarDate.zodiac];
    //[NSString stringWithFormat:@"%d %d %d", components.year, components.month, components.week];

    self.lunarLabel.text = @"农历";
    self.monthLabel.text = lunarDate.month;
    self.dayLabel.text = lunarDate.day;
    self.weekLabel.text = [aDate getDateFormatterForStr:@"EEEE"];
    self.yearLabel.text = [NSString stringWithFormat:@"%@%@%@年", lunarDate.tiangan, lunarDate.dizhi, lunarDate.zodiac];
}

@end
