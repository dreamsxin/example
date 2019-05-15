//
//  DataManager.h
//  Programmersluck
//
//  Created by cuibo on 3/17/13.
//  Copyright (c) 2013 cuibo. All rights reserved.
//

#import <Foundation/Foundation.h>

//活动标题／内容键
#define kActivitiesTitle     @"Title"
#define kActivitiesDetail    @"Detail"

@interface FortuneDataManager : NSObject

//获取适宜列表
@property(strong, nonatomic)NSArray *appropriateList;
//获取忌讳列表
@property(strong, nonatomic)NSArray *tabooList;

//座位朝向
@property(strong, nonatomic)NSString *direction;

//最佳饮品
@property(strong, nonatomic)NSArray *drinks;

//女神亲近指数
@property(strong, nonatomic)NSString *goddess;

//无敌幸运猿
@property(strong, nonatomic)NSString *luckyApe;

//使用日期更新数据
- (void)updateWithDate:(NSDate *)date;

@end


