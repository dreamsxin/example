//
//  LoadData.h
//  Programmersluck
//
//  Created by cuibo on 3/19/13.
//  Copyright (c) 2013 cuibo. All rights reserved.
//

#import <Foundation/Foundation.h>

//事件键值
#define kLoadDataActivitiesName @"name"
#define kLoadDataActivitiesGood @"good"
#define kLoadDataActivitiesBad  @"bad"

//方向事件键值
#define kLoadDataDirectionsEventPrefixes    @"prefixes"
#define kLoadDataDirectionsEventSuffix      @"suffix"

//特殊日期键值
#define kLoadDataSpecialsEventDate          @"date"
#define kLoadDataSpecialsEventType          @"type"
#define kLoadDataSpecialsEventName          @"name"
#define kLoadDataSpecialsEventDescription   @"description"


@interface LoadData : NSObject

@property(strong, nonatomic)NSArray *directionsList;        //方向列表
@property(strong, nonatomic)NSArray *directionsEventList;   //方向事件列表
@property(strong, nonatomic)NSArray *activitiesList;        //事件列表
@property(strong, nonatomic)NSArray *drinksList;            //饮品列表
@property(strong, nonatomic)NSArray *luckyWorkList;         //幸运工作列表
@property(strong, nonatomic)NSArray *specialsEventList;     //特殊日期事件列表

@end

