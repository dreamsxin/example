//
//  CalendarView.h
//  LiftStatistics
//
//  Created by cuibo on 7/8/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//
//  日历视图
//  对外接口，需要将日历作为视图时使用。
//

#import <UIKit/UIKit.h>
#import "CalendarLogicDelegate.h"
#import "CalendarViewDelegate.h"
#import "CalendarViewScrollDelegate.h"
#import "CalendarMonthMarkLocation.h"

@class CalendarLogic;   //日历逻辑
@class CalendarMonth;   //日历月历（一个月视图）

//CalendarLogicDelegate - 日历逻辑代理协议
@interface CalendarView : UIView
    <CalendarLogicDelegate>
{
    //日历视图代理
	id <CalendarViewDelegate> calendarViewDelegate;
    id <CalendarViewScrollDelegate> calendarViewScrollDelegate;
    
	CalendarLogic *calendarLogic;       //日历逻辑
	CalendarMonth *calendarView;        //月历视图
	CalendarMonth *calendarViewNew;     //新的月历视图
	NSDate *selectedDate;               //选中的日期
	UIButton *leftButton;               //向左按钮
	UIButton *rightButton;              //向右按钮
    CGRect calemdarViewFrame;           //日历位置和大小
    BOOL _isViewLoaded;                 //翻页滚动是否完成
    CGPoint _panVelocity;               //移动收拾移动速度

    CGPoint _handlePanLastLocation;     //移动手势上一次的位置
}

//设置代理
@property (nonatomic, strong) id <CalendarViewDelegate> calendarViewDelegate;
//设置视图滚动（翻页）代理
@property (nonatomic, strong) id <CalendarViewScrollDelegate> calendarViewScrollDelegate;
//设置当前选择
@property (nonatomic, strong) NSDate *selectedDate;

//获取当前日历视图中的所有日期，NSArray－但前日历中的所有日期(NSDate类型，格林尼治标准时间)，按照顺序排列。
- (NSArray *)getAllDatesInView;

//在指定日期上添加一个标记，aDate－日期，imgName－用做标记的图片，loca－相对位置
- (void)addMarkToDate:(NSDate *)aDate 
           ImageNamed:(NSString *)imgName 
             Location:(CalendarMonthMarkLocation)loca;

//删除指定日期上指定位置的标记，aDate－日期，loca－相对位置
- (void)removeMarkToDate:(NSDate *)aDate 
                Location:(CalendarMonthMarkLocation)loca;

@end


