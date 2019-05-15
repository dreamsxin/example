//
//  CalendarMonth.h
//  Calendar
//
//  Created by Lloyd Bottomley on 29/04/10.
//  Copyright 2010 Savage Media Pty Ltd. All rights reserved.
//
//  月历视图
//

#import <UIKit/UIKit.h>
#import "CalendarMonthMarkLocation.h"

@class CalendarLogic;

@interface CalendarMonth : UIView 
{
	CalendarLogic *calendarLogic;           //日历逻辑
	NSArray *datesIndex;                    //日期数组
	NSArray *buttonsIndex;                  //按钮数组
    NSMutableDictionary *markDict;          //标记字典（日期作为索引）
	
	NSInteger numberOfDaysInWeek;       //每周几天
    NSInteger numberOfWeeks;            //当前月历页面中有几周（行数）
	NSInteger selectedButton;           //选中的按钮索引
	NSDate *selectedDate;               //选中的日期
    
    CGRect headerFrame;                 //日历头大小和位置
    CGRect calendarFrame;               //日历大小和位置
    CGFloat calendarDayWidth;           //日历中天的宽度
    CGFloat calendarDayHeight;          //日历中天的高度
}

@property (nonatomic, strong) CalendarLogic *calendarLogic;
@property (nonatomic, strong) NSArray *datesIndex;
@property (nonatomic, strong) NSArray *buttonsIndex;
@property (nonatomic, strong) NSMutableDictionary *markDict;;
@property (nonatomic, assign) NSInteger numberOfDaysInWeek;
@property (nonatomic, assign) NSInteger numberOfWeeks;
@property (nonatomic, assign) NSInteger selectedButton;
@property (nonatomic, strong) NSDate *selectedDate;
@property (nonatomic, assign) CGRect headerFrame;
@property (nonatomic, assign) CGRect calendarFrame;
@property (nonatomic, assign) CGFloat calendarDayWidth;
@property (nonatomic, assign) CGFloat calendarDayHeight;


//初始化，frame－大小。aLogic－关联的日历逻辑
- (id)initWithFrame:(CGRect)frame logic:(CalendarLogic *)aLogic;

//选中指定日期的按钮
- (void)selectButtonForDate:(NSDate *)aDate;

//通过日期查找视图中的日期索引，不存在则返－1
- (NSInteger )findIndexForDate:(NSDate *)aDate;

//在指定日期上添加一个标记，aDate－日期，imgName－用做标记的图片，loca－相对位置
- (void)addMarkToDate:(NSDate *)aDate 
           ImageNamed:(NSString *)imgName 
             Location:(CalendarMonthMarkLocation)loca;

//删除指定日期上指定位置的标记，aDate－日期，loca－相对位置
- (void)removeMarkToDate:(NSDate *)aDate 
             Location:(CalendarMonthMarkLocation)loca;

@end



