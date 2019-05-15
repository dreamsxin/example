//
//  OCLunarDate.m
//  Programmersluck
//
//  Created by cuibo on 3/15/13.
//  Copyright (c) 2013 cuibo. All rights reserved.
//

#import "OCLunarDate.h"
#import "lunardate.h"
#import "solarterm.h"
#import "chinesefestivals.h"

@interface OCLunarDate()

@property(assign, nonatomic)NSInteger iday;                 //日
@property(assign, nonatomic)NSInteger imonth;               //月
@property(assign, nonatomic)NSInteger iyear;                //年
@property(strong, nonatomic)NSString *day;                  //日
@property(strong, nonatomic)NSString *month;                //月
@property(strong, nonatomic)NSString *zodiac;               //黄道
@property(strong, nonatomic)NSString *tiangan;              //天干
@property(strong, nonatomic)NSString *dizhi;                //地支
@property(strong, nonatomic)NSString *solarterm;            //节气
@property(strong, nonatomic)NSString *festival;             //（阳历）节日
@property(strong, nonatomic)NSString *lunarFestival;        //阴历节日
@property(strong, nonatomic)NSString *priorityLabel;                //优先标签
@property(assign, nonatomic)kPriorityLabelType priorityLabelType;   //优先标签类型

@end

@implementation OCLunarDate

//初始化一个农历日期，使用公历的年月日
+ (OCLunarDate *)lunarDateWithYear:(NSInteger)year month:(NSInteger)month day:(NSInteger)day
{
    return [[OCLunarDate alloc] initWithYear:year month:month day:day];
}

//初始化一个农历日期，使用公历的年月日
+ (OCLunarDate *)lunarDateWithNSDate:(NSDate *)date
{
    //创建一个当前用户的日历对象（NSCalendar用于处理时间相关问题。比如比较时间前后、计算日期所的周别等。）
	NSCalendar *calendar = [NSCalendar currentCalendar];
    //创建一个日期组件，并赋予当前日期
	NSDateComponents *components = [calendar components:(NSDayCalendarUnit | NSMonthCalendarUnit | NSYearCalendarUnit) fromDate:date];
    
    return [OCLunarDate lunarDateWithYear:components.year month:components.month day:components.day];
}

//初始化一个农历日期，使用公历的年月日
- (id)initWithYear:(NSInteger)year month:(NSInteger)month day:(NSInteger)day
{
    self = [super init];
    if (self)
    {
        const char *cstr;
        LunarDate lunarDate = lunardate_from_solar(year, month, day);

        //年月日
        self.iday = lunarDate.day;
        self.imonth = lunarDate.month;
        self.iyear = lunarDate.year;
        
        //日 月 年（属）
        cstr = lunardate_day(lunarDate.day);
        if(cstr)    self.day = [NSString stringWithUTF8String:cstr];
        else        self.day = nil;
        cstr = lunardate_month(lunarDate.month);
        if(cstr)    self.month = [NSString stringWithUTF8String:cstr];
        else        self.month = nil;
        cstr = lunardate_zodiac(lunarDate.year);
        if(cstr)    self.zodiac = [NSString stringWithUTF8String:cstr];
        else        self.zodiac = nil;

        //天干 地支
        cstr = lunardate_tiangan(lunarDate.year);
        if(cstr)    self.tiangan = [NSString stringWithUTF8String:cstr];
        else        self.tiangan = nil;
        cstr = lunardate_dizhi(lunarDate.year);
        if(cstr)    self.dizhi = [NSString stringWithUTF8String:cstr];
        else        self.dizhi = nil;

        //节气
        cstr = solarterm_name(solarterm_index(year, month, day));
        if(cstr)    self.solarterm = [NSString stringWithUTF8String:cstr];
        else        self.solarterm = nil;

        //阳历节日 阴历节日
        cstr = festival(month, day);
        if(cstr)    self.festival = [NSString stringWithUTF8String:cstr];
        else        self.festival = nil;
        cstr = lunarFestival(lunarDate.month, lunarDate.day);
        if(cstr)    self.lunarFestival = [NSString stringWithUTF8String:cstr];
        else        self.lunarFestival = nil;
        
        //优先的农历标签
        self.priorityLabel = [self priorityLunarLabel];
    }
    return self;
}

//返回优先标签
- (NSString *)priorityLunarLabel
{
    //阳历节日
    if(self.festival != nil)
    {
        self.priorityLabelType = kPriorityLabelTypeFestival;
        return self.festival;
    }
    //阴历节日
    else if(self.lunarFestival != nil)
    {
        self.priorityLabelType = kPriorityLabelTypeLunarFestival;
        return self.lunarFestival;
    }
    //节气
    else if(self.solarterm != nil)
    {
        self.priorityLabelType = kPriorityLabelTypeSolarterm;
        return self.solarterm;
    }
    //日期
    else if(self.day != nil)
    {
        self.priorityLabelType = kPriorityLabelTypeDay;
        return self.day;
    }
    //无效
    else
    {
        self.priorityLabelType = kPriorityLabelTypeNull;
        return nil;
    }
}

@end


