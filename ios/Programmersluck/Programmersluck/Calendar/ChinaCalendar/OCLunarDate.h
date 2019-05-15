//
//  OCLunarDate.h
//  Programmersluck
//
//  Created by cuibo on 3/15/13.
//  Copyright (c) 2013 cuibo. All rights reserved.
//

#import <Foundation/Foundation.h>

typedef enum
{
    kPriorityLabelTypeNull = -1,
    kPriorityLabelTypeDay  =  0,
    kPriorityLabelTypeSolarterm,
    kPriorityLabelTypeLunarFestival,
    kPriorityLabelTypeFestival,
}kPriorityLabelType;

@interface OCLunarDate : NSObject

//初始化一个农历日期，使用公历的年月日
+ (OCLunarDate *)lunarDateWithYear:(NSInteger)year month:(NSInteger)month day:(NSInteger)day;
//初始化一个农历日期，使用公历日期
+ (OCLunarDate *)lunarDateWithNSDate:(NSDate *)date;

@property(readonly, assign, nonatomic)NSInteger iday;           //日
@property(readonly, assign, nonatomic)NSInteger imonth;         //月
@property(readonly, assign, nonatomic)NSInteger iyear;          //年

@property(readonly, strong, nonatomic)NSString *day;            //日
@property(readonly, strong, nonatomic)NSString *month;          //月
@property(readonly, strong, nonatomic)NSString *zodiac;         //黄道(属)

@property(readonly, strong, nonatomic)NSString *tiangan;        //天干
@property(readonly, strong, nonatomic)NSString *dizhi;          //地支

@property(readonly, strong, nonatomic)NSString *solarterm;      //节气

@property(readonly, strong, nonatomic)NSString *festival;       //（阳历）节日
@property(readonly, strong, nonatomic)NSString *lunarFestival;  //阴历节日

@property(readonly, strong, nonatomic)NSString *priorityLabel;                  //优先标签
@property(readonly, assign, nonatomic)kPriorityLabelType priorityLabelType;     //优先标签类型

@end

